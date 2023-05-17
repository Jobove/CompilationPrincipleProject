//
// Created by Kasugano_Sora on 2023/5/17.
//

#include "DisjointSet.h"

DisjointSet::DisjointSet(std::size_t sz) {
    father.resize(sz + 1);
    size.resize(sz + 1, 1);

    for (int i = 0; i < father.size(); ++i) {
        father[i] = i;
    }
}

void DisjointSet::merge(int i, int j) {
    int x = find(i), y = find(j);

    if (size[x] <= size[y]) {
        father[x] = y;
    } else
        father[y] = x;
    if (size[x] == size[y] and x != y) {
        ++size[y];
    }
}

int DisjointSet::find(int i) {
    return father[i] == i ? i : find(father[i]);
}
