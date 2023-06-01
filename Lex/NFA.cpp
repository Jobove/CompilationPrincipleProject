//
// Created by Kasugano_Sora on 2023/5/16.
//

#include <stack>
#include <queue>
#include <iostream>
#include <functional>
#include <string>
#include "NFA.h"

bool is_operator(char ch) {
    static std::set<char> const operators{
            '&',
            '|',
            '?',
            '*',
            '+'
    };

    return operators.find(ch) != operators.end();
}

State::State(int is_receive_) : is_receive(is_receive_) {
    static int count = 0;
    id = ++count;
    former_id = 0;
}

void State::set_next(State *nxt, char ch) {
    if (not ch) {
        epsilon_out.insert(nxt);
        return;
    }
    if (edges[ch] != nullptr)
        throw std::runtime_error("set_next");

    edges[ch] = nxt;
}

void State::set_next(State &nxt, char ch) {
    set_next(&nxt, ch);
}

Fragment::Fragment(State *in_, State *out_) : in(in_), out(out_) {}

Fragment::Fragment(State &in_, State &out_) : in(&in_), out(&out_) {}

void Fragment::destroy() const {
    std::set<State *> bin;

    destroy_walk(in, bin, out);
    bin.erase(in);
    bin.erase(out);

    for (auto i: bin)
        delete i;
}

void Fragment::destroy_walk(State *pos, std::set<State *> &bin, State *end) {
    bin.emplace(pos);
    for (auto &[ch, p]: pos->edges) {
        if (bin.find(p) != bin.end() or p == end)
            continue;
        destroy_walk(p, bin, end);
    }
    for (auto p: pos->epsilon_out) {
        if (bin.find(p) != bin.end() or p == end)
            continue;
        destroy_walk(p, bin, end);
    }
}

Fragment *NFA::any(Fragment *operand) {
    auto *state1 = new State(false),
            *state2 = new State(true);

    state1->set_next(operand->in, 0);
    operand->out->set_next(operand->in, 0);
    operand->out->set_next(state2, 0);
    state1->set_next(state2, 0);

    operand->in->is_receive = false;
    operand->out->is_receive = false;

    return new Fragment(state1, state2);
}

Fragment *NFA::alter(Fragment *operand1, Fragment *operand2) {
    auto *state1 = new State(false),
            *state2 = new State(true);

    state1->set_next(operand1->in, 0);
    state1->set_next(operand2->in, 0);
    operand1->out->set_next(state2, 0);
    operand2->out->set_next(state2, 0);

    operand1->in->is_receive = false;
    operand2->in->is_receive = false;
    operand1->out->is_receive = false;
    operand2->out->is_receive = false;

    return new Fragment(state1, state2);
}

Fragment *NFA::combine(Fragment *operand1, Fragment *operand2) {
    auto *state1 = new State(false),
            *state2 = new State(false),
            *state3 = new State(false),
            *state4 = new State(true);

    state1->set_next(operand1->in, 0);
    operand1->out->set_next(state2, 0);
    state2->set_next(state3, 0);
    state3->set_next(operand2->in, 0);
    operand2->out->set_next(state4, 0);

    operand1->in->is_receive = false;
    operand2->in->is_receive = false;
    operand1->out->is_receive = false;
    operand2->out->is_receive = false;

    return new Fragment(state1, state4);
}

NFA::NFA(const std::string &postfix) {
    std::stack<Fragment *> fragments;

    for (auto ch: postfix) {
        if (not is_operator(ch)) {
            auto *begin = new State(false),
                    *end = new State(true);

            begin->set_next(end, ch);
            fragments.push(new Fragment(begin, end));
            continue;
        }

        switch (ch) {
            case '*': {
                auto top = fragments.top();
                fragments.pop();
                fragments.push(any(top));

                //delete top;
                break;
            }
            case '|': {
                Fragment *operand1, *operand2;
                operand1 = fragments.top();
                fragments.pop();
                operand2 = fragments.top();
                fragments.pop();

                fragments.push(alter(operand1, operand2));

                //delete operand1;
                //delete operand2;
                break;
            }
            case '&': {
                Fragment *operand1, *operand2;
                operand2 = fragments.top();
                fragments.pop();
                operand1 = fragments.top();
                fragments.pop();

                fragments.push(combine(operand1, operand2));

                //delete operand1;
                //delete operand2;
                break;
            }
            case '+': {
                auto *operand = fragments.top();
                fragments.pop();
                auto *begin = new State(false),
                        *end = new State(true);

                begin->set_next(operand->in, 0);
                operand->out->set_next(end, 0);
                end->set_next(begin, 0);

                operand->in->is_receive = false;
                operand->out->is_receive = false;

                fragments.push(new Fragment(begin, end));

                //delete operand;
                break;
            }
            case '?': {
                auto *operand = fragments.top();
                fragments.pop();
                auto *begin = new State(false),
                        *end = new State(true);

                begin->set_next(operand->in, 0);
                operand->out->set_next(end, 0);
                begin->set_next(end, 0);
                fragments.push(new Fragment(begin, end));

                operand->in->is_receive = false;
                operand->out->is_receive = false;

                //delete operand;
                break;
            }
            default: {
                throw std::runtime_error("Invalid operator!");
            }
        }
    }

    nfa = fragments.top();
    refactor();
}

NFA::~NFA() {
//    nfa->destroy();
    //delete nfa->in;
    //delete nfa->out;
}

void NFA::refactor() {
    std::queue<State *> q;
    std::map<int, bool> visited;
    int cnt = 0;
    q.push(nfa->in);
    visited[nfa->in->id] = true;

    std::map<State const *, int> debug;

    while (not q.empty()) {
        State *pos = q.front();
        q.pop();

        int id = ++cnt;
        id_to_state[id] = pos;

        state_to_id[pos] = id;

        if (id + 1 > son.size())
            son.resize(id + 1);

        for (auto &[ch, p]: pos->edges) {
            if (not visited[p->id])
                q.push(p);

            char_set.insert(ch);
            son[id].insert(p);
            debug[p] = id;
            visited[p->id] = true;
        }
        for (auto p: pos->epsilon_out) {
            if (not visited[p->id])
                q.push(p);

            son[id].insert(p);
            debug[p] = id;
            visited[p->id] = true;
        }
    }

    for (auto &[id, state]: id_to_state) {
        state->former_id = state->id;
        state->id = id;
    }

    son.shrink_to_fit();
    size = cnt;
}

void NFA::output() {
    std::cout << "<--------------------Outputting NFA Result-------------------->" << std::endl;
    for (auto &[id, state]: id_to_state) {
        std::cout << "id: " << id << " , former id: " << state->id << ". Sons: ";
        for (auto const &[ch, p]: state->edges) {
            std::cout << '(' << ch << ", " << state_to_id[p] << ", " << p->id << ") ";
        }
        for (auto const p: state->epsilon_out) {
            std::cout << '(' << "#" << ", " << state_to_id[p] << ", " << p->id << ") ";
        }
//        for (auto const &i: son[id]) {
//            std::cout << '(' << (i->symbol ? i->symbol : '#') << ", " << state_to_id[i] << ") ";
//        }
        std::cout << std::endl;
    }
    std::cout << "<---------------Finished Outputting NFA Result--------------->" << std::endl;
}

std::map<int, std::map<char, string>> NFA::get_adjacent_list() {
    std::map<int, std::map<char, string>> result;

    for (auto &[id, p]: id_to_state) {
        for (auto &[ch, v]: p->edges) {
            result[id][ch] = (result[id][ch].empty() ? "" : result[id][ch] + ", ") + std::to_string(state_to_id[v]);
        }
        for (auto &v: p->epsilon_out) {
            result[id]['#'] = (result[id]['#'].empty() ? "" : result[id]['#'] + ", ") + std::to_string(state_to_id[v]);
        }
    }

    return result;
}
