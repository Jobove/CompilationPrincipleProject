#include <stdexcept>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by Kasugano Sora on 2023/5/23.
//

#include "BNF.h"
#include "utils.h"
#include "boost/format.hpp"

#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

string const BNF::empty = "empty";

BNF::BNF(map<string, std::vector<std::vector<string>>> expressions, string start_letter) : expressions(
        std::move(expressions)), starter(std::move(start_letter)), root() {
    for (auto &[key, _]: this->expressions) {
        non_terminal.insert(key);
    }
}

BNF::~BNF() = default;

void BNF::remove_interminable() {
    set<string> terminable, not_terminal, former_terminable;
    do {
        former_terminable = terminable;
        for (auto const &[left, right]: expressions) {
            for (auto const &item: right) {
                bool flag = true;
                for (const auto &i: item) {
                    if (not terminable.contains(i) and non_terminal.contains(i)) {
                        flag = false;
                        break;
                    }
                }

                if (flag) {
                    terminable.insert(left);
                    break;
                }
            }
        }
    } while (terminable != former_terminable);

    for (auto &str: non_terminal) {
        if (terminable.contains(str))
            continue;

        not_terminal.insert(str);
    }

    for (auto &str: not_terminal) {
        expressions.erase(str);
        non_terminal.erase(str);
    }

    for (auto const &[left, right]: expressions) {
        std::vector<std::vector<string>> tmp;

        for (const auto &i: right) {
            bool flag = true;
            std::vector<string> item;

            for (const auto &j: i) {
                if (not_terminal.contains(j)) {
                    flag = false;
                    break;
                }
                item.push_back(j);
            }

            if (flag)
                tmp.push_back(item);
        }
        expressions[left] = tmp;
    }
}

void BNF::remove_unreachable() {
    map<string, bool> left;

    for (auto &[key, _]: expressions)
        left[key] = false;
    left[starter] = true;

    for (auto &[_, right]: expressions) {
        for (auto const &i: right) {
            for (auto const &j: i) {
                left[j] = true;
            }
        }
    }

    for (auto &[str, val]: left) {
        if (val)
            continue;

        non_terminal.erase(str);
        expressions.erase(str);
    }
}

void BNF::remove_direct_left_recursion() {
    using std::vector;
    for (auto it = expressions.begin(); it != expressions.end(); ++it) {
        auto right = it->second;
        bool flag = false;
        for (auto item: right) {
            if (it->first == item[0]) {
                flag = true;
                break;
            }
        }
        if (flag) {
            string new_left = it->first + "'";
            vector<vector<string>> right1, right2;
            for (auto item: right) {
                if (it->first == item[0]) {
                    vector<string> temp;
                    for (int i = 1; i < item.size(); ++i) {
                        temp.push_back(item[i]);
                    }
                    temp.push_back(new_left);
                    right2.push_back(temp);
                } else {
                    vector<string> temp;
                    for (const auto &i: item) {
                        temp.push_back(i);
                    }
                    temp.push_back(new_left);
                    right1.push_back(temp);
                }
            }
            // 添加 空字符
            right2.push_back({empty});
            expressions[it->first] = right1;
            expressions[new_left] = right2;
            non_terminal.insert(new_left);
        }
    }
}

void BNF::remove_indirect_left_recursion() {
    visited.clear();
    judge(starter);
    for (auto const &str: non_terminal)
        judge(str);
}

void BNF::judge(const string &str) {
    if (not non_terminal.contains(str))
        return;

    visited.insert(str);
    for (int i = 0; i < expressions[str].size(); ++i) {
        if (not non_terminal.contains(expressions[str][i][0]))
            continue;

        if (str == expressions[str][i][0]) {
            remove_direct_left_recursion();
        }
        if (visited.contains(expressions[str][i][0])) {
            string target = expressions[str][i][0];
            expressions[str][i].erase(expressions[str][i].begin(), expressions[str][i].begin() + 1);
            replace(str, i, target, expressions[str][i]);
            --i;
        } else
            judge(expressions[str][i][0]);
    }
    visited.erase(str);
}

void BNF::process_first() {
    for (auto const &str: non_terminal)
        first[str] = get_first_single(str);
}

set<string> BNF::get_first_single(string const &str) {
    if (first.contains(str))
        return first[str];

    set<string> res;
    if (not non_terminal.contains(str) or str == empty) {
        res.insert(str);
        return res;
    }

    for (auto const &item: expressions[str]) {
        auto tmp = get_first(item);
        res.insert(tmp.begin(), tmp.end());
    }

    return first[str] = res;
}

set<string> BNF::get_first(const std::vector<string> &item) {
    set<string> res;
    int i;

    for (i = 0; i < item.size(); ++i) {
        auto now = get_first_single(item[i]);
        res.insert(now.begin(), now.end());
        res.erase(empty);

        if (not now.contains(empty))
            break;
    }

    if (i == item.size())
        res.insert(empty);

    return res;
}

bool BNF::map_compare(const map<string, set<string>> &a, const map<string, set<string>> &b) {
    if (a.size() != b.size())
        return false;

    return std::all_of(a.begin(), a.end(), [&b](auto const &pair) {
        return b.contains(pair.first) and pair.second == b.at(pair.first);
    });
}

void BNF::process_follow() {
    using std::vector;
    map<string, set<string> > former = follow;
    for (const auto &str: non_terminal) {
        follow[str] = set<string>();
        if (str == starter)
            follow[str].insert("$");
    }
    while (!map_compare(former, follow)) {
        former = follow;
        for (const auto &it: expressions) {
            for (auto item: it.second) {
                for (int i = 0; i < item.size(); ++i) {
                    if (not non_terminal.contains(item[i]))
                        continue;

                    set<string> temporary_first;
                    vector<string> vec;
                    bool flag = false;

                    for (int j = i + 1; j < item.size(); ++j) {
                        vec.push_back(item[j]);
                        flag = true;
                    }
                    temporary_first = get_first(vec);
                    set<string> not_null = temporary_first;
                    not_null.erase(empty);
                    follow[item[i]].insert(not_null.begin(), not_null.end());

                    if (temporary_first.contains(empty) or not flag)
                        follow[item[i]].insert(follow[it.first].begin(), follow[it.first].end());
                }
            }
        }
    }
}

void BNF::process_terminal() {
    for (auto &[left, right]: expressions) {
        for (auto const &item: right) {
            for (auto const &i: item) {
                if (non_terminal.contains(i))
                    continue;

                terminal.insert(i);
            }
        }
    }

    terminal.insert("$");
    terminal.erase(empty);
}

void BNF::process_LL1() {
    for (auto const &str: non_terminal) {
        std::map<string, std::set<std::vector<string>>> map;
        for (auto const &i: terminal)
            map.emplace(i, std::set<std::vector<string>>());

        for (auto const &i: expressions[str]) {
            auto set = get_first(i);
            for (auto const &j: set) {
                map[j].insert(i);
                if (not set.contains(empty))
                    continue;

                for (auto const &k: follow[str])
                    map[k].insert(i);
            }
        }

        LL1[str] = map;
    }
}

void BNF::extract_left_common_factor_recursive() {
    set<string> tmp;
    string suffix = "@";

    do {
        tmp = non_terminal;
        extract_left_common_factor(suffix);
        suffix += "@";
    } while (tmp != non_terminal);
}

void BNF::extract_left_common_factor(string const &suffix) {
    using std::vector;
    set<string> temp;
    for (const auto &left: non_terminal) {
        vector<string> ans;
        get_longest_string(expressions[left], ans);
        if (ans.empty())
            continue;
        string str = left + suffix;
        vector<vector<string> > new_right;
        vector<vector<string> > old_right;

        for (auto item: expressions[left]) {
            if (item.size() >= ans.size()) {
                bool flag = true;
                for (int i = 0; i < ans.size(); ++i) {
                    if (ans[i] != item[i]) {
                        flag = false;
                        break;
                    }
                }

                if (flag) {
                    item.erase(item.begin(), item.begin() + ans.size());
                    if (item.empty())
                        item.push_back(empty);
                    new_right.push_back(item);
                } else {
                    old_right.push_back(item);
                }

            } else {
                old_right.push_back(item);
            }
        }
        ans.push_back(str);
        old_right.push_back(ans);

        expressions[left] = old_right;
        expressions[str] = new_right;
    }

    for (const auto &it: expressions)
        non_terminal.insert(it.first);
}

void BNF::get_longest_string(std::vector<std::vector<string>> const &right, std::vector<string> &ans) {
    std::multiset<string> count;


    for (auto const &i: right)
        count.insert(i[0]);

    size_t max = 1;
    string most_common_string;
    for (auto const &i: count) {
        if (count.count(i) <= max)
            continue;

        max = count.count(i);
        most_common_string = i;
    }

    if (max == 1 || most_common_string == empty)
        return;

    ans.push_back(most_common_string);
    std::vector<std::vector<string>> tmp;

    for (auto &item: right) {
        if (item[0] != most_common_string)
            continue;

        std::vector<string> now(item.begin() + 1, item.end());
        if (now.empty())
            now.emplace_back(empty);

        tmp.emplace_back(now);
    }

    get_longest_string(tmp, ans);
}

void BNF::replace(const string &left, int index, const string &str, const std::vector<string> &item) {
    auto const right = expressions[str];
    std::vector<std::vector<string>> tmp(expressions[left].begin(), expressions[left].begin() + index);

    for (auto const &i: right) {
        tmp.push_back(i);
        for (auto const &j: item) {
            tmp.back().push_back(j);
        }
    }

    tmp.insert(tmp.begin() + (int) tmp.size(), expressions[left].begin() + index + 1, expressions[left].end());
    expressions[left] = tmp;
}

void BNF::substitution() {
    for (auto &[left, right]: expressions) {
        auto tmp = right;
        for (int i = 0; i < tmp.size(); ++i) {
            if (not non_terminal.contains(tmp[i][0]))
                continue;

            auto target = expressions[left][i][0];

            expressions[left][i].erase(expressions[left][i].begin(), expressions[left][i].begin() + 1);
            replace(left, i, target, expressions[left][i]);
            --i;
            tmp = expressions[left];
        }
    }
}

map<string, std::vector<std::vector<string>>> BNF::get_expressions() const {
    return expressions;
}

void BNF::generate_syntax_tree(std::vector<MyTuple> &statement) {
    statement.push_back({"$", ""});

    std::stack<TreeNode> valueStack;
    //node start = { "",vector<node>() };
    //valueStack.push(start);


    std::stack<MyTuple> executionStack;


    executionStack.push({"$", ""});

    executionStack.push({starter, ""});
    MyTuple temp = {"", ""};
    int index = 0;
    while (!executionStack.empty()) {
        temp = executionStack.top();
        MyTuple a = statement[index];
        // 特殊符号  进行建树操作
        if (temp.show_num.size() >= 3 && temp.show_num[0] == '<' && temp.show_num[temp.show_num.size() - 1] == '>') {
            std::vector<TreeNode> children;
            executionStack.pop();
            temp.show_num.erase(0, 1);
            temp.show_num.erase(temp.show_num.size() - 1, 1);
            // 字符串转数字
            int num = std::stoi(temp.show_num);
            // 取出值栈前num个
            for (int i = 0; i < num; ++i) {
                children.push_back(valueStack.top());
                valueStack.pop();
            }
            // 再取出一个作为父节点
            TreeNode parent = valueStack.top();
            valueStack.pop();
            parent.child = children;
            valueStack.push(parent);
        }
            // empty 推入值栈
        else if (temp.show_num == "empty") {
            TreeNode treeNode = {temp, std::vector<TreeNode>()};
            valueStack.push(treeNode);
            executionStack.pop();
        }
            // 执行栈顶部和当前读入的串相同时，将其推入值栈，指针指向下一个串
        else if (temp.show_num == a.show_num) {
            if (temp.show_num != "$") {
                TreeNode treeNode = {a, std::vector<TreeNode>()};
                valueStack.push(treeNode);
                executionStack.pop();
            } else {
                executionStack.pop();
            }
            ++index;
        }
            // LL(1)表中对应位置没有产生式，报错
        else if (LL1[temp.show_num][a.show_num].empty()) {
            throw std::runtime_error("");
//            std::cout << "error" << std::endl;
//            break;
        }
            // LL(1)表中对应位置有产生式，先将执行栈顶部的节点包装好，push到值栈中，
            // 然后先往执行栈push特殊符号<num>，再逆序将产生式的每一项推入执行栈
        else if (!LL1[temp.show_num][a.show_num].empty()) {
            TreeNode treeNode = {temp, std::vector<TreeNode>()};
            valueStack.push(treeNode);
            executionStack.pop();

            for (auto it: LL1[temp.show_num][a.show_num]) {
                string num = std::to_string(it.size());
                executionStack.push({"<" + num + ">", ""});
                for (int i = it.size() - 1; i >= 0; --i) {
                    executionStack.push({it[i], ""});
                }
            }


        }
    }

    // 值栈只剩下一个节点，就是根节点
    root = valueStack.top();
//    statement.emplace_back("$", "");
//    std::stack<TreeNode> value_stack;
//    std::stack<MyTuple> execution_stack;
//
//    execution_stack.emplace("$", "");
//    execution_stack.emplace(starter, "");
//
//    int index = 0;
//    MyTuple tmp;
//    while (not execution_stack.empty()) {
//        tmp = execution_stack.top();
//        execution_stack.pop();
//        MyTuple a = statement[index];
//
//        if (tmp.show_num.size() >= 3 and tmp.show_num.starts_with('<') and tmp.show_num.ends_with('>')) {
//            std::vector<TreeNode> child;
//
//            tmp.show_num.erase(tmp.show_num.begin());
//            tmp.show_num.erase(tmp.show_num.back());
//
//            int num = std::stoi(tmp.show_num);
//            for (int i = 0; i < num; ++i) {
//                child.emplace_back(value_stack.top());
//                value_stack.pop();
//            }
//
//            TreeNode parent = value_stack.top();
//            value_stack.pop();
//            parent.child = child;
//            value_stack.push(parent);
//            continue;
//        }
//        if (tmp.show_num == "empty") {
//            TreeNode node{.data = tmp};
//            value_stack.emplace(node);
//            continue;
//        }
//        if (tmp.show_num == a.show_num) {
//            if (tmp.show_num != "$") {
//                TreeNode node{.data = a};
//                value_stack.emplace(node);
//                execution_stack.pop();
//            }
//            ++index;
//            continue;
//        }
//        if (LL1[tmp.show_num][a.show_num].empty())
//            throw std::runtime_error("syntax error");
//
//        TreeNode node{tmp};
//        value_stack.emplace(node);
//
//        for (auto const &i: LL1[tmp.show_num][a.show_num]) {
//            execution_stack.push({(boost::format("<%1%>") % i.size()).str(), ""});
//            std::for_each(i.rbegin(), i.rend(), [&](string const &s) {
//                execution_stack.push({s, ""});
//            });
//        }
//    }
//
//    root = value_stack.top();
}

std::map<string, set<string>> BNF::get_first() const {
    return first;
}

std::map<string, set<string>> BNF::get_follow() const {
    return follow;
}

TreeNode const &BNF::get_root() const {
    return root;
}

set<string> BNF::get_terminal() const {
    return terminal;
}

std::map<std::string, std::map<std::string, std::set<std::vector<string>>>> BNF::get_LL1() const {
    return LL1;
}

set<string> BNF::get_non_terminal() const {
    return non_terminal;
}

#pragma clang diagnostic pop