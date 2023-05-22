//
// Created by Kasugano_Sora on 2023/5/16.
//

#include "DFA.h"
#include <functional>
#include <iostream>
#include <queue>

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

    auto const &dfs = [&](int root, int id) {
        std::function<void(int, int)> function = [&](int root, int id) {
            visited[id] = true;

            auto p = id_to_state[id];

            for (auto v : p->epsilon_out) {
                int const next = state_to_id[v];

                reach_epsilon[root].insert(next);
                if (not visited[next])
                    function(root, next);
            }
        };
        function(root, id);
    };

    for (auto &[id, state]: id_to_state) {
        for (auto &[ch, v] : state->edges) {
            if (not ch)
                throw std::runtime_error("DFA");

            int next = state_to_id[v];
            not_epsilon[id] = std::make_pair(ch, next);
        }
    }

    for (int i = 1; i <= size; ++i) {
        visited.clear();
        dfs(i, i);
        reach_epsilon[i].insert(i);
    }

    int count = 0;
    std::queue<Node *> q;
    std::map<std::set<int>, int> set_to_id;
    q.push(new Node{
        ++count,
        false,
        std::map<char, Node *>(),
        1,
        reach_epsilon[1]
    });
    id_to_node[1] = q.front();
    node_to_id[q.front()] = 1;
    set_to_id[reach_epsilon[1]] = 1;

    while (not q.empty()) {
        auto p = q.front();
        q.pop();

        for (auto ch : char_set) {
            std::set<int> to;

            for (auto in : p->epsilon) {
                if (id_to_state[in]->edges.find(ch) == id_to_state[in]->edges.end())
                    continue;

                auto idx = id_to_state[in]->edges[ch]->id;
                auto tmp = reach_epsilon[idx];
                to.merge(tmp);
            }
            if (to.empty())
                continue;
//            std::set<int> set;
//            while (true) {
//                bool bo = false;
//
//                for (auto in : to) {
//                    if (id_to_state[in]->edges.find(ch) == id_to_state[in]->edges.end() or set.find(in) != set.end())
//                        continue;
//
//                    set.insert(in);
//                    to.merge(reach_epsilon[id_to_state[in]->edges[ch]->id]);
//                    bo = true;
//                }
//
//                if (not bo)
//                    break;
//            }

            if (set_to_id.find(to) == set_to_id.end()) {
                int id = ++count;
                auto *now = new Node{
                        id,
                        false,
                        std::map<char, Node *>(),
                        0,
                        to
                };

                for (auto i : to) {
                    now->is_receive |= (id_to_state[i] == nfa->out);
                }
                q.push(now);
                id_to_node[id] = now;
                node_to_id[now] = id;
                set_to_id[to] = id;
            }

            p->edges[ch] = id_to_node[set_to_id[to]];
        }

        /*
        for (auto &[former_id, pair] : not_epsilon) {
            if (p->epsilon.find(former_id) == p->epsilon.end())
                continue;

            std::set<int> to = reach_epsilon[pair.second];

            if (set_to_id.find(to) == set_to_id.end()) {
                auto *now = new Node{
                        ++count,
                        false,
                        std::map<char, Node *>(),
                        0,
                        to
                };
                q.push(now);
                id_to_node[now->id] = now;
                node_to_id[now] = now->id;
                set_to_id[to] = now->id;
            }
            p->edges[pair.first] = id_to_node[set_to_id[to]];
        }
         */
    }
}

void DFA::output() {
    NFA::output();

    std::cout << "<--------------------Outputting DFA Result------------------->" << std::endl;
    for (auto &[id, p]: id_to_node) {
        std::cout << "id: " << (p->is_receive ? "(receive) " : "") << id << ". Sons: ";
        for (auto &[ch, v]: id_to_node[id]->edges) {
            std::cout << '(' << ch << ", " << node_to_id[v] << ") ";
        }
        std::cout << std::endl;
    }
    std::cout << "<---------------Finished Outputting DFA Result--------------->" << std::endl;
}
