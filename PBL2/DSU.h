#pragma once

#include <vector>

class DSU {
    std::vector<int> parent;
public:
    DSU(int size) {
        parent.resize(size);
        for (int i = 0; i < size; ++i)
            parent[i] = i;
    }
    int find(int i) {
        return (parent[i] == i) ? i : (parent[i] = find(parent[i]));
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j)
            parent[root_i] = root_j;
    }
};