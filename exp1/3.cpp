#include <iostream>
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// 计算柱状图中最大矩形面积
int largestRectangleArea(vector<int>& heights) {
    stack<int> stk;  // 单调栈，存储索引，保持高度递增
    heights.push_back(0);  // 加入哨兵，便于处理最后元素
    int maxArea = 0;
    
    for (int i = 0; i < heights.size(); ++i) {
        // 当前高度小于栈顶高度时，弹出栈顶并计算面积
        while (!stk.empty() && heights[i] < heights[stk.top()]) {
            int height = heights[stk.top()];
            stk.pop();
            // 计算宽度：栈为空时宽度为i，否则为i - 栈顶索引 - 1
            int width = stk.empty() ? i : i - stk.top() - 1;
            maxArea = max(maxArea, height * width);
        }
        stk.push(i);
    }
    heights.pop_back();  // 恢复原数组
    return maxArea;
}

// 生成随机测试数据
vector<int> generateRandomHeights(int size) {
    vector<int> heights(size);
    for (int i = 0; i < size; ++i) {
        heights[i] = rand() % 10001;  // 0-104范围的随机数
    }
    return heights;
}

int main() {
    srand(time(0));  // 初始化随机数种子
    // 生成10组随机测试数据并测试
    cout << "\n随机测试数据结果：" << endl;
    for (int i = 0; i < 10; ++i) {
        int size = rand() % 100000 + 1;  // 1-105范围的随机大小
        vector<int> heights = generateRandomHeights(size);
        // 只输出前10个元素（避免过大输出）
        cout << "第" << i+1 << "组 (大小: " << size << "): ";
        for (int j = 0; j < min(10, size); ++j) {
            cout << heights[j] << " ";
        }
        if (size > 10) cout << "...";
        int area = largestRectangleArea(heights);
        cout << " -> 最大面积: " << area << endl;
    }
    return 0;
}