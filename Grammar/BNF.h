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

    std::vector<TreeNode *> child;
};

class BNF {
private:
    std::map<string, std::vector<std::vector<string>>> expressions;

    set<string> visited;

    set<string> terminal;

    set<string> non_terminal;

    string starter;

    map<string, set<string>> first;

    map<string, set<string>> follow;

    map<string, map<string, set<std::vector<string>>>> LL1;

    static string const empty;

    TreeNode *root;

public:
    BNF(map<string, std::vector<std::vector<string>>> expressions, string start_letter);

    ~BNF();

    void remove_interminable();

    void remove_unreachable();

    void remove_direct_left_recursion();

    void remove_indirect_left_recursion();

    void judge(string const &str);

    void replace(string const &left, int index, string const &str, std::vector<string> const &item);

    void process_first();

    set<string> get_first_single(string const &str);

    set<string> get_first(std::vector<string> const &item);

    void get_follow();

    static bool map_compare(map<string, set<string>> const &a, map<string, set<string>> const &b);

    void get_terminal();

    void get_LL1();

    void extract_left_common_factor();

    void get_longest_string(std::vector<std::vector<string>> &right, std::vector<string> &longest_string);

    void substitution();
};


#endif //COMPILATIONPRINCIPLE_BNF_H
