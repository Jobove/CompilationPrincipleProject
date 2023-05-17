//
// Created by Kasugano_Sora on 2023/5/16.
//

#ifndef COMPILATIONPRINCIPLE_DFA_H
#define COMPILATIONPRINCIPLE_DFA_H

#include "NFA.h"

struct Node {
    int id;

    std::map<char, Node *> edges;

    int former_id;

    std::set<int> epsilon;
};

class DFA : NFA {
private:
    bool minimized = false;

    Node *node = nullptr;

    std::map<int, Node *> id_to_node;

    std::map<Node *, int> node_to_id;

//    void dfs(std::map<int, std::set<int>> &reach_epsilon, std::map<int, bool> &visited, int root, int id);
public:
    explicit DFA(const std::string &postfix);

    void output() override;
};


#endif //COMPILATIONPRINCIPLE_DFA_H
