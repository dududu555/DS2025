#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

// 复数类定义
class Complex {
private:
    double real;  // 实部
    double imag;  // 虚部

public:
    // 构造函数
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}
    
    // 获取实部
    double getReal() const { return real; }
    
    // 获取虚部
    double getImag() const { return imag; }
    
    // 设置实部
    void setReal(double r) { real = r; }
    
    // 设置虚部
    void setImag(double i) { imag = i; }
    
    // 计算复数的模
    double modulus() const {
        return sqrt(real * real + imag * imag);
    }
    
    // 重载相等运算符
    bool operator==(const Complex& other) const {
        return (real == other.real) && (imag == other.imag);
    }
    
    // 重载不等运算符
    bool operator!=(const Complex& other) const {
        return !(*this == other);
    }
    
    // 重载输出运算符
    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << fixed << setprecision(2);
        os << "(" << c.real << ", " << c.imag << ")";
        return os;
    }
};

// 生成随机复数向量
vector<Complex> generateRandomComplexVector(int size, double minVal, double maxVal) {
    vector<Complex> vec;
    vec.reserve(size);
    
    for (int i = 0; i < size; ++i) {
        double real = minVal + (maxVal - minVal) * rand() / RAND_MAX;
        double imag = minVal + (maxVal - minVal) * rand() / RAND_MAX;
        vec.emplace_back(real, imag);
    }
    
    return vec;
}

// 向量置乱
void shuffleVector(vector<Complex>& vec) {
    for (int i = vec.size() - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(vec[i], vec[j]);
    }
}

// 查找元素（实部和虚部均相同）
int findElement(const vector<Complex>& vec, const Complex& target) {
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] == target) {
            return i;
        }
    }
    return -1;  // 未找到
}

// 插入元素
void insertElement(vector<Complex>& vec, int index, const Complex& elem) {
    if (index >= 0 && index <= vec.size()) {
        vec.insert(vec.begin() + index, elem);
    }
}

// 删除元素
bool deleteElement(vector<Complex>& vec, const Complex& elem) {
    auto it = find(vec.begin(), vec.end(), elem);
    if (it != vec.end()) {
        vec.erase(it);
        return true;
    }
    return false;
}

// 向量唯一化
void uniqueVector(vector<Complex>& vec) {
    sort(vec.begin(), vec.end(), [](const Complex& a, const Complex& b) {
        if (a.modulus() != b.modulus()) {
            return a.modulus() < b.modulus();
        }
        return a.getReal() < b.getReal();
    });
    
    auto last = unique(vec.begin(), vec.end());
    vec.erase(last, vec.end());
}

// 起泡排序（以模为基准，模相同则以实部为基准）
void bubbleSort(vector<Complex>& vec) {
    int n = vec.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (vec[j].modulus() > vec[j+1].modulus() || 
                (vec[j].modulus() == vec[j+1].modulus() && vec[j].getReal() > vec[j+1].getReal())) {
                swap(vec[j], vec[j+1]);
            }
        }
    }
}

// 归并排序辅助函数
void merge(vector<Complex>& vec, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    vector<Complex> L(n1), R(n2);
    
    for (int i = 0; i < n1; ++i)
        L[i] = vec[left + i];
    for (int j = 0; j < n2; ++j)
        R[j] = vec[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        if (L[i].modulus() < R[j].modulus() || 
            (L[i].modulus() == R[j].modulus() && L[i].getReal() < R[j].getReal())) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }
    
    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}

// 归并排序主函数
void mergeSort(vector<Complex>& vec, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        mergeSort(vec, left, mid);
        mergeSort(vec, mid + 1, right);
        
        merge(vec, left, mid, right);
    }
}

// 区间查找：查找模介于[m1, m2)的所有元素
vector<Complex> rangeSearch(const vector<Complex>& sortedVec, double m1, double m2) {
    vector<Complex> result;
    
    for (const auto& c : sortedVec) {
        double mod = c.modulus();
        if (mod >= m1 && mod < m2) {
            result.push_back(c);
        } else if (mod >= m2) {
            break;  // 因为向量已排序，后面的元素模更大，可提前退出
        }
    }
    
    return result;
}

// 打印向量
void printVector(const vector<Complex>& vec, int maxElements = 10) {
    int n = vec.size();
    cout << "向量大小: " << n << endl;
    if (n == 0) return;
    
    cout << "元素" << (n > maxElements ? " (前" + to_string(maxElements) + "个)" : "") << ": ";
    for (int i = 0; i < min(n, maxElements); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

// 测试排序算法性能
void testSortingPerformance(int size) {
    // 生成随机向量
    vector<Complex> vec = generateRandomComplexVector(size, -1000, 1000);
    
    // 测试顺序情况（先排序）
    vector<Complex> sortedVec = vec;
    mergeSort(sortedVec, 0, sortedVec.size() - 1);
    
    // 测试逆序情况
    vector<Complex> reversedVec = sortedVec;
    reverse(reversedVec.begin(), reversedVec.end());
    
    // 测试乱序情况（原始向量）
    vector<Complex> shuffledVec = vec;
    
    // 计时变量
    clock_t start, end;
    double bubbleTime, mergeTime;
    
    cout << "\n=== 排序性能测试 (向量大小: " << size << ") ===" << endl;
    
    // 测试顺序情况
    cout << "\n顺序情况:" << endl;
    
    vector<Complex> temp = sortedVec;
    start = clock();
    bubbleSort(temp);
    end = clock();
    bubbleTime = double(end - start) / CLOCKS_PER_SEC;
    cout << "起泡排序时间: " << fixed << setprecision(6) << bubbleTime << "秒" << endl;
    
    temp = sortedVec;
    start = clock();
    mergeSort(temp, 0, temp.size() - 1);
    end = clock();
    mergeTime = double(end - start) / CLOCKS_PER_SEC;
    cout << "归并排序时间: " << fixed << setprecision(6) << mergeTime << "秒" << endl;
    
    // 测试乱序情况
    cout << "\n乱序情况:" << endl;
    
    temp = shuffledVec;
    start = clock();
    bubbleSort(temp);
    end = clock();
    bubbleTime = double(end - start) / CLOCKS_PER_SEC;
    cout << "起泡排序时间: " << fixed << setprecision(6) << bubbleTime << "秒" << endl;
    
    temp = shuffledVec;
    start = clock();
    mergeSort(temp, 0, temp.size() - 1);
    end = clock();
    mergeTime = double(end - start) / CLOCKS_PER_SEC;
    cout << "归并排序时间: " << fixed << setprecision(6) << mergeTime << "秒" << endl;
    
    // 测试逆序情况
    cout << "\n逆序情况:" << endl;
    
    temp = reversedVec;
    start = clock();
    bubbleSort(temp);
    end = clock();
    bubbleTime = double(end - start) / CLOCKS_PER_SEC;
    cout << "起泡排序时间: " << fixed << setprecision(6) << bubbleTime << "秒" << endl;
    
    temp = reversedVec;
    start = clock();
    mergeSort(temp, 0, temp.size() - 1);
    end = clock();
    mergeTime = double(end - start) / CLOCKS_PER_SEC;
    cout << "归并排序时间: " << fixed << setprecision(6) << mergeTime << "秒" << endl;
}

int main() {
    srand(time(0));  // 初始化随机数生成器
    
    // 生成随机复数向量
    int size = 20;
    vector<Complex> complexVec = generateRandomComplexVector(size, -10, 10);
    
    cout << "=== 初始随机复数向量 ===" << endl;
    printVector(complexVec);
    
    // 测试置乱操作
    shuffleVector(complexVec);
    cout << "\n=== 置乱后的向量 ===" << endl;
    printVector(complexVec);
    
    // 测试查找操作
    if (!complexVec.empty()) {
        Complex target = complexVec[rand() % complexVec.size()];
        int index = findElement(complexVec, target);
        cout << "\n=== 查找操作 ===" << endl;
        cout << "查找元素 " << target << ": " << (index != -1 ? "找到，索引为 " + to_string(index) : "未找到") << endl;
    }
    
    // 测试插入操作
    Complex newElem(100, 200);
    insertElement(complexVec, 3, newElem);
    cout << "\n=== 插入元素 " << newElem << " 后的向量 ===" << endl;
    printVector(complexVec);
    
    // 测试删除操作
    bool deleted = deleteElement(complexVec, newElem);
    cout << "\n=== 删除元素 " << newElem << " 后的向量 ===" << endl;
    cout << (deleted ? "删除成功" : "删除失败") << endl;
    printVector(complexVec);
    
    // 测试唯一化操作
    uniqueVector(complexVec);
    cout << "\n=== 唯一化后的向量 ===" << endl;
    printVector(complexVec);
    
    // 测试排序性能
    testSortingPerformance(1000);  // 使用1000个元素进行性能测试
    
    // 测试区间查找
    vector<Complex> sortedVec = generateRandomComplexVector(50, 0, 10);
    mergeSort(sortedVec, 0, sortedVec.size() - 1);  // 先排序
    cout << "\n=== 区间查找测试 ===" << endl;
    cout << "排序后的向量: ";
    printVector(sortedVec);
    
    double m1 = 3.0, m2 = 7.0;
    vector<Complex> result = rangeSearch(sortedVec, m1, m2);
    cout << "模介于 [" << m1 << ", " << m2 << ") 的元素:" << endl;
    printVector(result);
    
    return 0;
}