//
// Created by Kasugano_Sora on 2023/5/17.
//

#ifndef COMPILATIONPRINCIPLE_DISJOINTSET_H
#define COMPILATIONPRINCIPLE_DISJOINTSET_H

#include <vector>


class DisjointSet {
private:
    std::vector<int> father;

    std::vector<int> size;

public:
    explicit DisjointSet(std::size_t sz);

    void merge(int i, int j);

    int find(int i);
};


#endif //COMPILATIONPRINCIPLE_DISJOINTSET_H
