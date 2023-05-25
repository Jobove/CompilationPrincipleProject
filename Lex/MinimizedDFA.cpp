//
// Created by Kasugano_Sora on 2023/5/18.
//

#include <iostream>
#include "MinimizedDFA.h"
#include "DisjointSet.h"

MinimizedDFA::MinimizedDFA(const std::string &postfix) : DFA(postfix) {
    DisjointSet disjointSet(id_to_node.size());

    for (auto &[id, ptr]: id_to_node) {
        for (auto &[_id, _ptr]: id_to_node) {
            if (ptr->edges != _ptr->edges or ptr->is_receive != _ptr->is_receive)
                continue;

            disjointSet.merge(id, _id);
        }
    }

    int count = 0;
    std::map<int, int> former_id_to_id;
    for (int i = 1; i <= id_to_node.size(); ++i) {
        if (disjointSet.find(i) != i)
            continue;

        int id = ++count;
        auto *now = new Node{
                .id = id,
                .is_receive = id_to_node[i]->is_receive
        };

        former_id_to_id[i] = id;
        id_to_minimized_node[id] = now;
        minimized_node_to_id[now] = id;
    }

    for (auto &[former_id, node] : id_to_node) {
        int u = former_id_to_id[disjointSet.find(former_id)];

        for (auto &[ch, former_v] : node->edges) {
            int v = former_id_to_id[disjointSet.find(node_to_id[former_v])];
            id_to_minimized_node[u]->edges[ch] = id_to_minimized_node[v];
        }
    }

    minimized_node = id_to_minimized_node[1];
}

void MinimizedDFA::output() {
    DFA::output();

    std::cout << "<---------------Outputting Minimized DFA Result-------------->" << std::endl;
    for (auto &[id, p]: id_to_minimized_node) {
        std::cout << "id: " << (p->is_receive ? "(receive) " : "") << id << ". Sons: ";
        for (auto &[ch, v]: id_to_minimized_node[id]->edges) {
            std::cout << '(' << ch << ", " << minimized_node_to_id[v] << ") ";
        }
        std::cout << std::endl;
    }
    std::cout << "<----------Finished Outputting Minimized DFA Result---------->" << std::endl;
}

bool MinimizedDFA::match(const string &str) {
    bool result = true;
    auto *p = minimized_node;

    for (auto const &ch : str) {
        if (p == nullptr) {
            result = false;
            break;
        }

        auto edges = p->edges;
        auto next = edges[ch];
        p = next;
    }

    return result and p and p->is_receive;
}

std::map<int, std::map<char, int>> MinimizedDFA::get_adjacent_list() {
    std::map<int, std::map<char, int>> result;

    for (auto &[id, p]: id_to_minimized_node) {
        for (auto &[ch, v]: p->edges) {
            result[id][ch] = minimized_node_to_id[v];
        }
    }

    return result;
}

std::map<int, Node *> const &MinimizedDFA::get_id_to_minimized_node() const {
    return id_to_minimized_node;
}

int MinimizedDFA::count_end() const {
    int res = 0;

    for (auto &[id, state]: id_to_minimized_node)
        res += state->is_receive;

    return res;
}

std::set<int> MinimizedDFA::get_end() const {
    std::set<int> res;
    for (auto &[id, state]: id_to_minimized_node)
        if (state->is_receive)
            res.insert(id);

    return res;
}
