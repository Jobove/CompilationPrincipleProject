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

    int former_id;

    bool is_receive;

    std::map<char, State *> edges;

    std::set<State *> epsilon_out;

    State(int is_receive_);

    void set_next(State *nxt, char ch);

    void set_next(State &nxt, char ch);
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

    std::set<char> char_set;

    int size;
public:
    explicit NFA(const std::string &postfix);

    ~NFA();

    virtual void output();

    virtual std::map<int, std::map<char, string>> get_adjacent_list();
};

#endif //COMPILATIONPRINCIPLE_NFA_H
