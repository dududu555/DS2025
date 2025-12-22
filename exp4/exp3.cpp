#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include<windows.h>

// 边界框结构体：包含坐标(x1,y1为左上角，x2,y2为右下角)、置信度score
struct BBox {
    float x1, y1, x2, y2;
    float score;

    BBox(float x1_ = 0, float y1_ = 0, float x2_ = 0, float y2_ = 0, float score_ = 0)
        : x1(x1_), y1(y1_), x2(x2_), y2(y2_), score(score_) {}
};

// 计算交并比(IoU)：NMS的核心指标
float calculateIoU(const BBox& a, const BBox& b) {
    float interX1 = std::max(a.x1, b.x1);
    float interY1 = std::max(a.y1, b.y1);
    float interX2 = std::min(a.x2, b.x2);
    float interY2 = std::min(a.y2, b.y2);

    if (interX1 >= interX2 || interY1 >= interY2) return 0.0f;

    float interArea = (interX2 - interX1) * (interY2 - interY1);
    float aArea = (a.x2 - a.x1) * (a.y2 - a.y1);
    float bArea = (b.x2 - b.x1) * (b.y2 - b.y1);

    return interArea / (aArea + bArea - interArea);
}

// 1. 冒泡排序：O(n²)，稳定
template <typename T>
void bubbleSort(std::vector<T>& vec, bool (*cmp)(const T&, const T&)) {
    int n = vec.size();
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            if (cmp(vec[j + 1], vec[j])) {
                std::swap(vec[j], vec[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

// 2. 插入排序：O(n²)，稳定，适合小规模/接近有序数据
template <typename T>
void insertionSort(std::vector<T>& vec, bool (*cmp)(const T&, const T&)) {
    int n = vec.size();
    for (int i = 1; i < n; ++i) {
        T key = vec[i];
        int j = i - 1;
        while (j >= 0 && cmp(key, vec[j])) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = key;
    }
}

// 归并排序辅助函数
template <typename T>
void merge(std::vector<T>& vec, int left, int mid, int right, bool (*cmp)(const T&, const T&)) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    std::vector<T> L(n1), R(n2);

    for (int i = 0; i < n1; ++i) L[i] = vec[left + i];
    for (int j = 0; j < n2; ++j) R[j] = vec[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (cmp(L[i], R[j])) {
            vec[k] = L[i];
            i++;
        } else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) { vec[k] = L[i]; i++; k++; }
    while (j < n2) { vec[k] = R[j]; j++; k++; }
}

// 3. 归并排序：O(nlogn)，稳定，空间O(n)
template <typename T>
void mergeSort(std::vector<T>& vec, bool (*cmp)(const T&, const T&), int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(vec, cmp, left, mid);
    mergeSort(vec, cmp, mid + 1, right);
    merge(vec, left, mid, right, cmp);
}

template <typename T>
void mergeSort(std::vector<T>& vec, bool (*cmp)(const T&, const T&)) {
    if (vec.empty()) return;
    mergeSort(vec, cmp, 0, vec.size() - 1);
}

// 快速排序辅助函数
template <typename T>
int partition(std::vector<T>& vec, int low, int high, bool (*cmp)(const T&, const T&)) {
    T pivot = vec[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (cmp(vec[j], pivot)) {
            i++;
            std::swap(vec[i], vec[j]);
        }
    }
    std::swap(vec[i + 1], vec[high]);
    return i + 1;
}

// 4. 快速排序：O(nlogn)，不稳定，平均性能最优
template <typename T>
void quickSort(std::vector<T>& vec, bool (*cmp)(const T&, const T&), int low, int high) {
    if (low < high) {
        int pi = partition(vec, low, high, cmp);
        quickSort(vec, cmp, low, pi - 1);
        quickSort(vec, cmp, pi + 1, high);
    }
}

template <typename T>
void quickSort(std::vector<T>& vec, bool (*cmp)(const T&, const T&)) {
    if (vec.empty()) return;
    quickSort(vec, cmp, 0, vec.size() - 1);
}

// 比较函数：按置信度降序排序
bool compareBBox(const BBox& a, const BBox& b) {
    return a.score > b.score;
}

// NMS算法实现
std::vector<BBox> nonMaximumSuppression(std::vector<BBox> bboxes, float iouThresh, 
                                        void (*sortFunc)(std::vector<BBox>&, bool (*)(const BBox&, const BBox&))) {
    if (bboxes.empty()) return {};

    // 使用指定排序算法按置信度排序
    sortFunc(bboxes, compareBBox);

    std::vector<BBox> keep;
    while (!bboxes.empty()) {
        BBox top = bboxes[0];
        keep.push_back(top);
        std::vector<BBox> remaining;

        // 过滤IoU超过阈值的框
        for (size_t i = 1; i < bboxes.size(); ++i) {
            if (calculateIoU(top, bboxes[i]) < iouThresh) {
                remaining.push_back(bboxes[i]);
            }
        }
        bboxes = remaining;
    }
    return keep;
}

// 生成随机分布的边界框
std::vector<BBox> generateRandomBBoxes(int num, float imgW = 640, float imgH = 640) {
    std::vector<BBox> bboxes;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> coordDist(0, std::min(imgW, imgH));
    std::uniform_real_distribution<float> sizeDist(10, 100);
    std::uniform_real_distribution<float> scoreDist(0, 1);

    for (int i = 0; i < num; ++i) {
        float x1 = coordDist(gen);
        float y1 = coordDist(gen);
        float x2 = x1 + sizeDist(gen);
        float y2 = y1 + sizeDist(gen);
        // 确保边界框不超出图像范围
        x2 = std::min(x2, imgW);
        y2 = std::min(y2, imgH);
        bboxes.emplace_back(x1, y1, x2, y2, scoreDist(gen));
    }
    return bboxes;
}

// 生成聚集分布的边界框（模拟实际检测的重叠框）
std::vector<BBox> generateClusteredBBoxes(int num, int clusterNum = 5, float imgW = 640, float imgH = 640) {
    std::vector<BBox> bboxes;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> clusterCoordDist(50, std::min(imgW, imgH) - 50);
    std::uniform_real_distribution<float> sizeDist(20, 150);
    std::uniform_real_distribution<float> offsetDist(-30, 30);
    std::uniform_real_distribution<float> scoreDist(0, 1);

    // 生成聚类中心
    std::vector<BBox> clusters;
    for (int i = 0; i < clusterNum; ++i) {
        float x1 = clusterCoordDist(gen);
        float y1 = clusterCoordDist(gen);
        float x2 = x1 + sizeDist(gen);
        float y2 = y1 + sizeDist(gen);
        clusters.emplace_back(x1, y1, x2, y2, scoreDist(gen));
    }

    // 围绕聚类中心生成重叠框
    for (int i = 0; i < num; ++i) {
        BBox& cluster = clusters[i % clusterNum];
        float x1 = cluster.x1 + offsetDist(gen);
        float y1 = cluster.y1 + offsetDist(gen);
        float x2 = cluster.x2 + offsetDist(gen);
        float y2 = cluster.y2 + offsetDist(gen);
        // 确保边界框合法
        x1 = std::max(0.0f, x1);
        y1 = std::max(0.0f, y1);
        x2 = std::min(x2, imgW);
        y2 = std::min(y2, imgH);
        if (x1 >= x2 || y1 >= y2) {
            x1 = cluster.x1;
            y1 = cluster.y1;
            x2 = cluster.x2;
            y2 = cluster.y2;
        }
        bboxes.emplace_back(x1, y1, x2, y2, scoreDist(gen));
    }
    return bboxes;
}

// 性能测试函数：测试排序+NMS的耗时
void testPerformance(const std::string& sortName, void (*sortFunc)(std::vector<BBox>&, bool (*)(const BBox&, const BBox&)),
                     const std::vector<BBox>& bboxes, float iouThresh) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<BBox> result = nonMaximumSuppression(bboxes, iouThresh, sortFunc);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << std::left << std::setw(15) << sortName 
              << "耗时: " << std::fixed << std::setprecision(3) << duration.count() << " ms, "
              << "保留框数量: " << result.size() << std::endl;
}

// 主函数：测试入口
int main() {
    SetConsoleOutputCP(CP_UTF8);
    // 测试配置
    const int dataSizes[] = {100, 1000, 5000, 10000}; // 不同数据规模
    const float iouThresh = 0.5f; // NMS的IoU阈值
    const int clusterNum = 5;     // 聚集分布的聚类数

    // 排序算法列表
    std::vector<std::pair<std::string, void (*)(std::vector<BBox>&, bool (*)(const BBox&, const BBox&))>> sortAlgos = {
        {"冒泡排序", bubbleSort<BBox>},
        {"插入排序", insertionSort<BBox>},
        {"归并排序", mergeSort<BBox>},
        {"快速排序", quickSort<BBox>}
    };

    // 遍历不同数据规模测试
    for (int size : dataSizes) {
        std::cout << "=====================================" << std::endl;
        std::cout << "数据规模: " << size << " 个边界框" << std::endl;
        std::cout << "-------------------------------------" << std::endl;

        // 测试随机分布数据
        std::cout << "【随机分布数据】" << std::endl;
        std::vector<BBox> randomBBoxes = generateRandomBBoxes(size);
        for (auto& algo : sortAlgos) {
            testPerformance(algo.first, algo.second, randomBBoxes, iouThresh);
        }

        // 测试聚集分布数据
        std::cout << "【聚集分布数据】" << std::endl;
        std::vector<BBox> clusteredBBoxes = generateClusteredBBoxes(size, clusterNum);
        for (auto& algo : sortAlgos) {
            testPerformance(algo.first, algo.second, clusteredBBoxes, iouThresh);
        }
        std::cout << std::endl;
    }

    return 0;
}