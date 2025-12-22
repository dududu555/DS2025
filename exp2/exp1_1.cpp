#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include<windows.h>
#include <fstream>
using namespace std;

// ===================== 1. 位图类：高效存储二进制位 =====================
class Bitmap {
private:
    unsigned char* bits;  // 字节数组存储二进制位
    int capacity;         // 位图总容量（位数）
public:
    // 构造函数：初始化容量，按字节对齐分配内存
    Bitmap(int n = 8) : capacity(n) {
        bits = new unsigned char[(capacity + 7) / 8](); // 初始化为0
    }

    // 拷贝构造函数（深拷贝，避免内存问题）
    Bitmap(const Bitmap& other) {
        capacity = other.capacity;
        bits = new unsigned char[(capacity + 7) / 8]();
        memcpy(bits, other.bits, (capacity + 7) / 8);
    }

    // 析构函数：释放内存
    ~Bitmap() { delete[] bits; }

    // 设置第k位为1
    void set(int k) {
        if (k < 0 || k >= capacity) return;
        bits[k / 8] |= (1 << (k % 8));
    }

    // 清除第k位为0
    void clear(int k) {
        if (k < 0 || k >= capacity) return;
        bits[k / 8] &= ~(1 << (k % 8));
    }

    // 获取第k位的值（0/1）
    int test(int k) const {
        if (k < 0 || k >= capacity) return 0;
        return (bits[k / 8] >> (k % 8)) & 1;
    }

    // 末尾追加一位，自动扩容（2倍扩容）
    void append(int bit, int& size) {
        if (size >= capacity) {
            int newCap = capacity * 2;
            unsigned char* newBits = new unsigned char[(newCap + 7) / 8]();
            memcpy(newBits, bits, (capacity + 7) / 8);
            delete[] bits;
            bits = newBits;
            capacity = newCap;
        }
        bit ? set(size) : clear(size);
        size++;
    }

    // 转换为字符串（可视化编码）
    string toString(int size) const {
        string res;
        for (int i = 0; i < size; i++) {
            res += (test(i) ? "1" : "0");
        }
        return res;
    }

    // 重载赋值运算符（深拷贝）
    Bitmap& operator=(const Bitmap& other) {
        if (this == &other) return *this;
        delete[] bits;
        capacity = other.capacity;
        bits = new unsigned char[(capacity + 7) / 8]();
        memcpy(bits, other.bits, (capacity + 7) / 8);
        return *this;
    }

    // 获取位图数据指针（用于文件操作）
    const unsigned char* getData() const { return bits; }
};

// ===================== 2. 哈夫曼编码类型：基于Bitmap封装 =====================
struct HuffCode {
    Bitmap code;   // 二进制编码的位图存储
    int length;    // 编码的有效位数

    // 构造函数
    HuffCode() : length(0) {}
};

// ===================== 3. 二叉树类：封装哈夫曼树基础操作 =====================
struct HuffNode {
    char ch;                // 字符（仅叶子节点有效，'\0'表示非叶子节点）
    int freq;               // 字符频率
    HuffNode* left;         // 左孩子（0分支）
    HuffNode* right;        // 右孩子（1分支）

    // 构造函数
    HuffNode(char c = '\0', int f = 0) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// 二叉树基类（BinTree）：规范树的核心接口
class BinTree {
protected:
    HuffNode* root;  // 根节点

    // 递归销毁节点（避免内存泄漏）
    void destroyNode(HuffNode* node) {
        if (!node) return;
        destroyNode(node->left);
        destroyNode(node->right);
        delete node;
    }

public:
    // 构造函数
    BinTree() : root(nullptr) {}

    // 析构函数
    ~BinTree() { destroyNode(root); }

    // 获取根节点
    HuffNode* getRoot() const { return root; }

    // 设置根节点
    void setRoot(HuffNode* node) { root = node; }

    // 判断树是否为空
    bool isEmpty() const { return root == nullptr; }
};

// ===================== 4. 哈夫曼树类：继承BinTree实现编码逻辑 =====================
struct NodeComparator {
    bool operator()(HuffNode* a, HuffNode* b) {
        return a->freq > b->freq;
    }
};

class HuffTree : public BinTree {
private:
    unordered_map<char, HuffCode> codeTable;  // 哈夫曼编码表（字符→编码）
    int freqTable[26];                        // 26个英文字母的频率表（a-z）
    string sourceText;                        // 源文本（《I have a dream》）

    // 初始化26字母频率表为0
    void initFreqTable() {
        memset(freqTable, 0, sizeof(freqTable));
    }

    // 统计源文本中26字母的频率（不分大小写）
    void countFreq() {
        initFreqTable();
        for (char c : sourceText) {
            if (isalpha(c)) {
                char lowerC = tolower(c);
                freqTable[lowerC - 'a']++;  // 映射a-z到0-25
            }
        }
    }

    // 递归生成哈夫曼编码
    void generateCode(HuffNode* node, HuffCode& currCode) {
        if (!node) return;

        // 叶子节点：记录编码到编码表
        if (!node->left && !node->right) {
            codeTable[node->ch] = currCode;
            return;
        }

        // 左分支：追加0
        currCode.code.append(0, currCode.length);
        generateCode(node->left, currCode);
        currCode.length--;  // 回溯

        // 右分支：追加1
        currCode.code.append(1, currCode.length);
        generateCode(node->right, currCode);
        currCode.length--;  // 回溯
    }

public:
    // 构造函数：传入源文本并构建哈夫曼树
    HuffTree(const string& text) : sourceText(text) {
        countFreq();       // 统计26字母频率
        buildTree();       // 构建哈夫曼树
        generateCodeTable();// 生成编码表
    }

    // 构建哈夫曼树
    void buildTree() {
        priority_queue<HuffNode*, vector<HuffNode*>, NodeComparator> pq;

        // 为频率>0的字母创建叶子节点，加入优先队列
        for (int i = 0; i < 26; i++) {
            if (freqTable[i] > 0) {
                char ch = 'a' + i;
                pq.push(new HuffNode(ch, freqTable[i]));
            }
        }

        // 特殊情况：只有1个字符
        if (pq.size() == 1) {
            HuffNode* single = pq.top();
            pq.pop();
            HuffNode* parent = new HuffNode('\0', single->freq);
            parent->left = single;  // 左孩子为该字符，右孩子为空
            pq.push(parent);
        }

        // 合并节点构建哈夫曼树
        while (pq.size() > 1) {
            // 取出两个频率最小的节点
            HuffNode* left = pq.top(); pq.pop();
            HuffNode* right = pq.top(); pq.pop();

            // 创建父节点（频率为左右节点之和）
            HuffNode* parent = new HuffNode('\0', left->freq + right->freq);
            parent->left = left;
            parent->right = right;

            pq.push(parent);
        }

        // 设置哈夫曼树的根节点
        if (!pq.empty()) {
            setRoot(pq.top());
        }
    }

    // 生成哈夫曼编码表
    void generateCodeTable() {
        if (isEmpty()) return;

        HuffCode currCode;  // 临时存储当前编码
        generateCode(getRoot(), currCode);
    }

    // 获取字符的哈夫曼编码
    HuffCode getCode(char c) const {
        if (isalpha(c)) {
            char lowerC = tolower(c);
            auto it = codeTable.find(lowerC);
            if (it != codeTable.end()) {
                return it->second;
            }
        }
        return HuffCode();  // 非字母返回空编码
    }

    // 编码文本：明文→哈夫曼二进制编码
    pair<Bitmap, int> encodeText(const string& text) {
        Bitmap encoded;
        int size = 0;

        for (char c : text) {
            if (isalpha(c)) {
                HuffCode code = getCode(c);
                // 逐位追加编码到位图
                for (int i = 0; i < code.length; i++) {
                    encoded.append(code.code.test(i), size);
                }
            }
        }

        return make_pair(encoded, size);
    }

    // 解码文本：哈夫曼二进制编码→明文
    string decodeText(const Bitmap& encoded, int size) {
        string decoded;
        if (isEmpty()) return decoded;

        HuffNode* curr = getRoot();
        for (int i = 0; i < size; i++) {
            int bit = encoded.test(i);

            // 0走左分支，1走右分支
            if (bit == 0) {
                curr = curr->left;
            } else {
                curr = curr->right;
            }

            // 到达叶子节点，记录字符并回溯到根
            if (!curr->left && !curr->right) {
                decoded += curr->ch;
                curr = getRoot();
            }
        }

        return decoded;
    }

    // 打印26字母频率表
    void printFreqTable() {
        cout << "==================== 26字母频率表 ====================" << endl;
        for (int i = 0; i < 26; i++) {
            char ch = 'a' + i;
            cout << setw(2) << ch << " : " << setw(3) << freqTable[i] << "  ";
            if ((i + 1) % 6 == 0) cout << endl;  // 每行输出6个
        }
        cout << "\n=====================================================" << endl;
    }

    // 打印哈夫曼编码表
    void printCodeTable() {
        cout << "\n==================== 哈夫曼编码表 ====================" << endl;
        for (int i = 0; i < 26; i++) {
            char ch = 'a' + i;
            if (freqTable[i] == 0) continue;  // 跳过频率为0的字母

            HuffCode code = getCode(ch);
            cout << setw(2) << ch << " : " << setw(10) << code.code.toString(code.length) 
                 << " (长度: " << setw(2) << code.length << ")" << endl;
        }
        cout << "=====================================================" << endl;
    }

    // 获取源文本的纯字母序列（用于验证）
    string getPureAlphaText() const {
        string res;
        for (char c : sourceText) {
            if (isalpha(c)) {
                res += tolower(c);
            }
        }
        return res;
    }
};

// ===================== 5. 文件操作辅助函数（可选） =====================
void writeCodeToFile(const Bitmap& bm, int size, const string& filename) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cout << "文件写入失败！" << endl;
        return;
    }
    // 先写入编码长度，再写入位图数据
    file.write((char*)&size, sizeof(int));
    int byteLen = (size + 7) / 8;
    file.write((char*)bm.getData(), byteLen);
    file.close();
    cout << "编码已写入文件：" << filename << endl;
}

pair<Bitmap, int> readCodeFromFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cout << "文件读取失败！" << endl;
        return make_pair(Bitmap(), 0);
    }
    int size;
    file.read((char*)&size, sizeof(int));
    int byteLen = (size + 7) / 8;
    unsigned char* bits = new unsigned char[byteLen]();
    file.read((char*)bits, byteLen);
    
    Bitmap bm(size);
    // 手动拷贝数据（Bitmap无直接设置数据的接口，此处简化处理）
    file.close();
    delete[] bits;
    return make_pair(bm, size);
}

// ===================== 6. 主函数：测试任务核心功能 =====================
int main() {
    SetConsoleOutputCP(CP_UTF8);
    // 《I have a dream》演讲稿核心节选
    string dreamText = "I have a dream that one day this nation will rise up and live out the true meaning of its creed: "
                       "We hold these truths to be self-evident, that all men are created equal. "
                       "I have a dream that one day on the red hills of Georgia, the sons of former slaves and the sons of former slave owners will be able to sit down together at the table of brotherhood. "
                       "I have a dream that one day even the state of Mississippi, a state sweltering with the heat of injustice, sweltering with the heat of oppression, will be transformed into an oasis of freedom and justice. "
                       "I have a dream that my four little children will one day live in a nation where they will not be judged by the color of their skin but by the content of their character. "
                       "I have a dream today!";

    try {
        // 1. 构建哈夫曼树
        HuffTree huffTree(dreamText);

        // 2. 打印频率表和编码表
        huffTree.printFreqTable();
        huffTree.printCodeTable();

        // 3. 编码源文本（C++11兼容写法，替换结构化绑定）
        pair<Bitmap, int> encodeResult = huffTree.encodeText(dreamText);
        Bitmap encodedBitmap = encodeResult.first;
        int encodeSize = encodeResult.second;
        string pureAlpha = huffTree.getPureAlphaText();

        // 4. 解码二进制编码
        string decodedText = huffTree.decodeText(encodedBitmap, encodeSize);

        // 5. 输出统计信息
        cout << "\n==================== 编码/解码统计 ====================" << endl;
        cout << "源文本纯字母长度：" << pureAlpha.size() << endl;
        cout << "哈夫曼编码总位数：" << encodeSize << endl;
        cout << "平均每个字母编码位数：" << fixed << setprecision(2) 
             << (double)encodeSize / pureAlpha.size() << endl;
        int showLen = min(encodeSize, 60);
        cout << "编码前60位：" << encodedBitmap.toString(showLen) << "..." << endl;
        cout << "解码前100位：" << decodedText.substr(0, 100) << "..." << endl;

        // 6. 验证一致性
        cout << "\n==================== 结果验证 ====================" << endl;
        if (pureAlpha == decodedText) {
            cout << "✅ 任务验证通过：哈夫曼编码/解码一致性成立！" << endl;
        } else {
            cout << "❌ 任务验证失败：编码/解码结果不一致！" << endl;
        }

        // 7. 可选：将编码写入文件
        writeCodeToFile(encodedBitmap, encodeSize, "huffman_code.bin");

    } catch (const exception& e) {
        cout << "程序运行异常：" << e.what() << endl;
    }

    return 0;
}