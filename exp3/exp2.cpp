#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <climits>
#include <algorithm>
#include<windows.h>
using namespace std;

// 无穷大定义
const int INF = INT_MAX / 2;
// 图1的节点数（A-H共8个节点，编号0-7）
const int NODE_NUM = 8;
// 节点名映射
const char nodeName[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

// 构建图1的邻接矩阵
vector<vector<int>> buildAdjMatrix() {
    vector<vector<int>> matrix(NODE_NUM, vector<int>(NODE_NUM, INF));
    // 初始化自环权值为0
    for (int i = 0; i < NODE_NUM; ++i) {
        matrix[i][i] = 0;
    }
    // 赋值边权（根据图1的边）
    matrix[0][1] = matrix[1][0] = 4;    // A-B
    matrix[0][3] = matrix[3][0] = 6;    // A-D
    matrix[0][6] = matrix[6][0] = 7;    // A-G
    matrix[1][2] = matrix[2][1] = 12;   // B-C
    matrix[2][4] = matrix[4][2] = 1;    // C-E
    matrix[2][5] = matrix[5][2] = 2;    // C-F
    matrix[2][7] = matrix[7][2] = 10;   // C-H
    matrix[3][4] = matrix[4][3] = 13;   // D-E
    matrix[3][6] = matrix[6][3] = 2;    // D-G
    matrix[4][5] = matrix[5][4] = 5;    // E-F
    matrix[4][6] = matrix[6][4] = 11;   // E-G
    matrix[4][7] = matrix[7][4] = 8;    // E-H
    matrix[5][7] = matrix[7][5] = 3;    // F-H
    matrix[6][7] = matrix[7][6] = 14;   // G-H
    return matrix;
}

// 广度优先搜索（BFS）
void BFS(const vector<vector<int>>& matrix, int start) {
    vector<bool> visited(NODE_NUM, false);
    queue<int> q;
    q.push(start);
    visited[start] = true;
    cout << "BFS遍历顺序（从" << nodeName[start] << "出发）：";
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        cout << nodeName[u] << " ";
        for (int v = 0; v < NODE_NUM; ++v) {
            if (matrix[u][v] != INF && matrix[u][v] != 0 && !visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
    cout << endl;
}

// 深度优先搜索（DFS，递归版）
void DFS_recursive(const vector<vector<int>>& matrix, int u, vector<bool>& visited) {
    visited[u] = true;
    cout << nodeName[u] << " ";
    for (int v = 0; v < NODE_NUM; ++v) {
        if (matrix[u][v] != INF && matrix[u][v] != 0 && !visited[v]) {
            DFS_recursive(matrix, v, visited);
        }
    }
}

// DFS封装
void DFS(const vector<vector<int>>& matrix, int start) {
    vector<bool> visited(NODE_NUM, false);
    cout << "DFS遍历顺序（从" << nodeName[start] << "出发）：";
    DFS_recursive(matrix, start, visited);
    cout << endl;
}

// Dijkstra算法求最短路径
void Dijkstra(const vector<vector<int>>& matrix, int start) {
    vector<int> dist(NODE_NUM, INF);  // 起点到各节点的距离
    vector<bool> visited(NODE_NUM, false);
    dist[start] = 0;

    for (int i = 0; i < NODE_NUM - 1; ++i) {
        // 找到未访问的距离最小的节点
        int minDist = INF, u = -1;
        for (int j = 0; j < NODE_NUM; ++j) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }
        if (u == -1) break;
        visited[u] = true;

        // 松弛操作
        for (int v = 0; v < NODE_NUM; ++v) {
            if (!visited[v] && matrix[u][v] != INF && dist[u] + matrix[u][v] < dist[v]) {
                dist[v] = dist[u] + matrix[u][v];
            }
        }
    }

    // 输出结果
    cout << "Dijkstra最短路径（从" << nodeName[start] << "出发）：" << endl;
    for (int i = 0; i < NODE_NUM; ++i) {
        cout << nodeName[start] << "->" << nodeName[i] << "：";
        if (dist[i] == INF) {
            cout << "不可达";
        } else {
            cout << dist[i];
        }
        cout << endl;
    }
}

// Prim算法求最小生成树
void Prim(const vector<vector<int>>& matrix, int start) {
    vector<int> key(NODE_NUM, INF);    // 保存各节点到生成树的最小权值
    vector<int> parent(NODE_NUM, -1);  // 生成树的父节点
    vector<bool> inMST(NODE_NUM, false);
    key[start] = 0;
    int totalWeight = 0;

    for (int i = 0; i < NODE_NUM - 1; ++i) {
        // 找到未加入MST的key最小的节点
        int minKey = INF, u = -1;
        for (int j = 0; j < NODE_NUM; ++j) {
            if (!inMST[j] && key[j] < minKey) {
                minKey = key[j];
                u = j;
            }
        }
        if (u == -1) break;
        inMST[u] = true;
        totalWeight += key[u];

        // 更新邻接节点的key值
        for (int v = 0; v < NODE_NUM; ++v) {
            if (matrix[u][v] != INF && !inMST[v] && matrix[u][v] < key[v]) {
                key[v] = matrix[u][v];
                parent[v] = u;
            }
        }
    }

    // 输出最小生成树的边和总权值
    cout << "Prim最小生成树的边（从" << nodeName[start] << "出发）：" << endl;
    for (int i = 0; i < NODE_NUM; ++i) {
        if (parent[i] != -1) {
            cout << nodeName[parent[i]] << "-" << nodeName[i] << " (权值：" << matrix[parent[i]][i] << ")" << endl;
        }
    }
    cout << "最小生成树总权值：" << totalWeight << endl;
}

// 主函数测试图1的算法
int main() {
    // 构建图1的邻接矩阵
    SetConsoleOutputCP(CP_UTF8);
    vector<vector<int>> adjMatrix = buildAdjMatrix();
    int start = 0;  // 从A节点（编号0）出发

    // 测试BFS
    BFS(adjMatrix, start);

    // 测试DFS
    DFS(adjMatrix, start);

    // 测试Dijkstra最短路径
    Dijkstra(adjMatrix, start);

    // 测试Prim最小生成树
    Prim(adjMatrix, start);

    return 0;
}