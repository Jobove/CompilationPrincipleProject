//
// Created by Kasugano_Sora on 2023/5/16.
//

#ifndef COMPILATIONPRINCIPLE_NFA_H
#define COMPILATIONPRINCIPLE_NFA_H

#include <string>
#include <set>
#include <map>
#include <vector>

using std::string;

struct State {
    int id;

    bool is_receive;

    State *left;

    State *right;

    char symbol;

    State(int is_receive_, char symbol_);

    void set_next(State *nxt);

    void set_next(State &nxt);
};

struct Fragment {
    State * in;

    State * out;

    Fragment(State *in_, State *out_);

    Fragment(State &in_, State &out_);

    void destroy() const;

    static void destroy_walk(State *pos, std::set<State *> &bin, State *end);
};

class NFA {
private:
    static Fragment *any(Fragment *operand);

    static Fragment *alter(Fragment *operand1, Fragment *operand2);

    static Fragment *combine(Fragment *operand1, Fragment *operand2);

    void refactor();

protected:
    Fragment *nfa;

    std::map<int, State *> id_to_state;

    std::map<State const *, int> state_to_id;

    std::vector<std::set<State const *>> son;
public:
    explicit NFA(const std::string &postfix);

    ~NFA();

    virtual void output();
};

#endif //COMPILATIONPRINCIPLE_NFA_H
