//
// Created by zhanGGe on 2018/9/26.
//
#include "graph.h"
Graph::Graph(int V) {
    this->V = V;
    adj = new list<int>[V];
}

void Graph::addEdge(int u, int v) {
    adj[u].push_back(v); //add v to u's list
}


vector<vector<int>> Graph::printAllPaths(int s, int d, int hops) {
    vector<bool>  visited(V, false);
    vector<vector<int>> paths;
    vector<int> path;
    printAllPathsUtil(s, d, visited, paths, path, hops);

    return paths;
}

void Graph::printAllPathsUtil(int u, int d, vector<bool> &visited, vector<vector<int>> &paths, vector<int> &path, int hops) {
    visited[u] = true;
    path.push_back(u);
    if (u == d){
        paths.push_back(path);
    }else{
        list<int>::iterator i;
        for (i = adj[u].begin();  i != adj[u].end(); ++i) {
            if(!visited[*i] && path.size() <= hops){
                printAllPathsUtil(*i, d, visited, paths, path, hops);
            }
        }
    }
    visited[u] = false;
    path.pop_back();
}