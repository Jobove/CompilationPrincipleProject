//
// Created by Kasugano_Sora on 2023/5/16.
//

#include <stack>
#include <queue>
#include <iostream>
#include <functional>
#include "NFA.h"

bool is_operator(char ch) {
    static std::set<char> const operators{
            '.',
            '|',
            '?',
            '*',
            '+'
    };

    return operators.find(ch) != operators.end();
}

State::State(int is_receive_, char symbol_) : is_receive(is_receive_), symbol(symbol_) {
    static int count = 0;
    id = ++count;
    left = right = nullptr;
}

void State::set_next(State *nxt) {
    if (left and right)
        throw std::runtime_error("More than 2 outward edge.");
    State *&now = (left ? right : left);

    now = nxt;
}

void State::set_next(State &nxt) {
    set_next(&nxt);
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
    if (pos->left and bin.find(pos->left) == bin.end() and pos->left != end)
        destroy_walk(pos->left, bin, end);
    if (pos->right and bin.find(pos->right) == bin.end() and pos->right != end)
        destroy_walk(pos->left, bin, end);
}

Fragment *NFA::any(Fragment *operand) {
    auto *state1 = new State(false, 0),
            *state2 = new State(true, 0);

    state1->set_next(operand->in);
    operand->out->set_next(operand->in);
    operand->out->set_next(state2);
    state1->set_next(state2);

    return new Fragment(state1, state2);
}

Fragment *NFA::alter(Fragment *operand1, Fragment *operand2) {
    auto *state1 = new State(false, 0),
            *state2 = new State(true, 0);

    state1->set_next(operand1->in);
    state1->set_next(operand2->in);
    operand1->out->set_next(state2);
    operand2->out->set_next(state2);

    return new Fragment(state1, state2);
}

Fragment *NFA::combine(Fragment *operand1, Fragment *operand2) {
    auto *state1 = new State(false, 0),
            *state2 = new State(false, 0),
            *state3 = new State(false, 0),
            *state4 = new State(true, 0);

    state1->set_next(operand1->in);
    operand1->out->set_next(state2);
    state2->set_next(state3);
    state3->set_next(operand2->in);
    operand2->out->set_next(state4);

    return new Fragment(state1, state4);
}

NFA::NFA(const std::string &postfix) {
    std::stack<Fragment *> fragments;

    for (auto ch: postfix) {
        if (not is_operator(ch)) {
            auto *begin = new State(false, ch),
                    *end = new State(true, 0);

            begin->set_next(end);
            fragments.push(new Fragment(begin, end));
            continue;
        }

        switch (ch) {
            case '*': {
                auto top = fragments.top();
                fragments.pop();
                fragments.push(any(top));

                delete top;
                break;
            }
            case '|': {
                Fragment *operand1, *operand2;
                operand1 = fragments.top();
                fragments.pop();
                operand2 = fragments.top();
                fragments.pop();

                fragments.push(alter(operand1, operand2));

                delete operand1;
                delete operand2;
                break;
            }
            case '.': {
                Fragment *operand1, *operand2;
                operand2 = fragments.top();
                fragments.pop();
                operand1 = fragments.top();
                fragments.pop();

                fragments.push(combine(operand1, operand2));

                delete operand1;
                delete operand2;
                break;
            }
            case '+': {
                auto *operand = fragments.top();
                fragments.pop();
                auto *begin = new State(false, 0),
                        *end = new State(true, 0);

                begin->set_next(operand->in);
                operand->out->set_next(end);
                end->set_next(begin);

                fragments.push(new Fragment(begin, end));

                delete operand;
                break;
            }
            case '?': {
                auto *operand = fragments.top();
                fragments.pop();
                auto *begin = new State(false, 0),
                        *end = new State(true, 0);

                begin->set_next(operand->in);
                operand->out->set_next(end);
                begin->set_next(end);
                fragments.push(new Fragment(begin, end));

                delete operand;
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
    nfa->destroy();
    delete nfa->in;
    delete nfa->out;
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

        if (pos->left) {
            if (not visited[pos->left->id])
                q.push(pos->left);
            son[id].insert(pos->left);
            debug[pos->left] = id;
            visited[pos->left->id] = true;
        }
        if (pos->right) {
            if (not visited[pos->right->id])
                q.push(pos->right);
            son[id].insert(pos->right);
            debug[pos->right] = id;
            visited[pos->right->id] = true;
        }
    }

    son.shrink_to_fit();
}

void NFA::output() {
    std::cout << "<--------------------Outputting NFA Result-------------------->" << std::endl;
    for (auto &[id, State]: id_to_state) {
        std::cout << "id: " << id << ". Sons: ";
        for (auto const &i: son[id]) {
            std::cout << '(' << (i->symbol ? i->symbol : '#') << ", " << state_to_id[i] << ") ";
        }
        std::cout << std::endl;
    }
    std::cout << "<---------------Finished Outputting NFA Result--------------->" << std::endl;
}