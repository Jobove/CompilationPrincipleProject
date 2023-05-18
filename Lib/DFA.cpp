//
// Created by Kasugano_Sora on 2023/5/16.
//

#include "DFA.h"
#include <functional>
#include <iostream>

//void DFA::dfs(std::map<int, std::set<int>> &reach_epsilon, std::map<int, bool> &visited, int root, int id) {
//    visited[id] = true;
//
//    auto p = id_to_state[id];
//
//    std::cout << p->symbol << std::endl;
//
//    if (p->symbol)
//        return;
//
//    if (p->left) {
//        int const left = state_to_id[p->left];
//
//        reach_epsilon[id].insert(left);
//        if (not visited[left])
//            dfs(reach_epsilon, visited, root, left);
//    }
//
//    if (p->right) {
//        int const right = state_to_id[p->right];
//
//        reach_epsilon[id].insert(right);
//        if (not visited[right])
//            dfs(reach_epsilon, visited, root, right);
//    }
//}

DFA::DFA(const std::string &postfix) : NFA(postfix) {
    std::map<int, std::set<int>> reach_epsilon;
    std::map<int, std::pair<char, int>> not_epsilon;
    std::map<int, bool> visited;
//    DisjointSet disjointSet(id_to_state.size());

    auto const &dfs = [&](int root, int id) {
        std::function<void(int, int)> function = [&](int root, int id) {
            visited[id] = true;

            auto p = id_to_state[id];

            if (p->symbol)
                return;

            if (p->left) {
                int const left = state_to_id[p->left];

                reach_epsilon[root].insert(left);
//                reach_epsilon[id_to_state[root]->id].insert(p->left->id);
                if (not visited[left])
                    function(root, left);
            }

            if (p->right) {
                int const right = state_to_id[p->right];

                reach_epsilon[root].insert(right);
//                reach_epsilon[id_to_state[root]->id].insert(p->right->id);
                if (not visited[right])
                    function(root, right);
            }
        };
        function(root, id);
    };

    for (auto &[id, state]: id_to_state) {
        if (not state->symbol)
            continue;

        int next = state_to_id[state->left];
        not_epsilon[id] = std::make_pair(state->symbol, next);
    }

    for (auto &[u, pair]: not_epsilon) {
        int v = pair.second;

        visited.clear();
        dfs(v, v);
    }

    dfs(1, 1);

    int count = 0;
    std::map<int, int> former_id_to_id;

    for (auto &[former_id, epsilon]: reach_epsilon) {
        former_id_to_id[former_id] = ++count;

        int id = count;
        auto *now = new Node{
                id,
                std::map<char, Node *>(),
                former_id,
                epsilon
        };

        id_to_node[id] = now;
        node_to_id[now] = id;
    }

    for (auto &[former_id, pair]: not_epsilon) {
        for (auto &[p, id]: node_to_id) {
            if (!(p->former_id == former_id or p->epsilon.find(former_id) != p->epsilon.end()))
                continue;

            p->edges[pair.first] = id_to_node[former_id_to_id[pair.second]];
        }
    }
}

void DFA::output() {
    NFA::output();

    std::cout << "<--------------------Outputting DFA Result------------------->" << std::endl;
    for (auto &[id, p]: id_to_node) {
        std::cout << "id: " << id << ". Sons: ";
        for (auto &[ch, v]: id_to_node[id]->edges) {
            std::cout << '(' << ch << ", " << node_to_id[v] << ") ";
        }
        std::cout << std::endl;
    }
    std::cout << "<---------------Finished Outputting DFA Result--------------->" << std::endl;
}
