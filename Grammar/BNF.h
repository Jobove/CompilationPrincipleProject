//
// Created by Kasugano Sora on 2023/5/23.
//

#ifndef COMPILATIONPRINCIPLE_BNF_H
#define COMPILATIONPRINCIPLE_BNF_H


#include <map>
#include <string>
#include <set>
#include <vector>
#include "utils.h"

using std::set;
using std::string;
using std::map;

struct TreeNode {
    MyTuple data;

    std::vector<TreeNode> child;
};

class BNF {
private:

    set<string> visited;

    set<string> terminal;

    set<string> non_terminal;

    string starter;

    map<string, set<string>> first;

    map<string, set<string>> follow;

    map<string, map<string, set<std::vector<string>>>> LL1;

    static string const empty;

    TreeNode root;

    void extract_left_common_factor(string const &suffix);

    std::map<string, std::vector<std::vector<string>>> expressions;

    void judge(string const &str);

public:
    [[nodiscard]] set<string> get_terminal() const;

    [[nodiscard]] set<string> get_non_terminal() const;

    BNF(map<string, std::vector<std::vector<string>>> expressions, string start_letter);

    ~BNF();

    void remove_interminable();

    void remove_unreachable();

    void remove_direct_left_recursion();

    void remove_indirect_left_recursion();

    void replace(string const &left, int index, string const &str, std::vector<string> const &item);

    void process_first();

    set<string> get_first_single(string const &str);

    set<string> get_first(std::vector<string> const &item);

    [[nodiscard]] std::map<string, set<string>> get_first() const;

    [[nodiscard]] std::map<string, set<string>> get_follow() const;

    [[nodiscard]] std::map<std::string, std::map<std::string, std::set<std::vector<string>>>> get_LL1() const;

    void process_follow();

    static bool map_compare(map<string, set<string>> const &a, map<string, set<string>> const &b);

    void process_terminal();

    void process_LL1();

    void extract_left_common_factor_recursive();

    void get_longest_string(std::vector<std::vector<string>> const &right, std::vector<string> &ans);

    void substitution();

    void generate_syntax_tree(std::vector<MyTuple> &statement);

    [[nodiscard]] map<string, std::vector<std::vector<string>>> get_expressions() const;

    [[nodiscard]] TreeNode const &get_root() const;
};


#endif //COMPILATIONPRINCIPLE_BNF_H
