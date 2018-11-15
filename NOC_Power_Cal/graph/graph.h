//
// Created by zhanGGe on 2018/9/26.
//

#ifndef NOC_POWER_CAL_GRAGH_H
#define NOC_POWER_CAL_GRAGH_H

#endif //NOC_POWER_CAL_GRAGH_H

#include <iostream>
#include <list>
#include <algorithm>
#include <vector>

using namespace std;

class Graph{
    int V;  // No. of vertices in graph
    list<int> *adj;

    void printAllPathsUtil(int u, int d, vector<bool> &visited, vector<vector<int>> &paths, vector<int> &path, int hops);
public:
    Graph(int V);
    void addEdge(int u, int v);
    vector<vector<int>> printAllPaths(int s, int d, int hops);
};
