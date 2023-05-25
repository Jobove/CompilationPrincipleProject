//
// Created by Kasugano_Sora on 2023/5/18.
//

#ifndef COMPILATIONPRINCIPLE_MINIMIZEDDFA_H
#define COMPILATIONPRINCIPLE_MINIMIZEDDFA_H

#include "DFA.h"
#include <set>

class MinimizedDFA : DFA {
private:
    Node *minimized_node;

    std::map<int, Node *> id_to_minimized_node;

    std::map<Node *, int> minimized_node_to_id;

public:
    explicit MinimizedDFA(const std::string &postfix);

    void output() override;

    bool match(const string &str);

    std::map<int, std::map<char, int>> get_adjacent_list();

    [[nodiscard]] std::map<int, Node *> const &get_id_to_minimized_node() const;

    [[nodiscard]] int count_end() const;

    [[nodiscard]] std::set<int> get_end() const;
};


#endif //COMPILATIONPRINCIPLE_MINIMIZEDDFA_H
