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
            if (ptr->edges != _ptr->edges)
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
        std::cout << "id: " << id << ". Sons: ";
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
