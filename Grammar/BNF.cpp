//
// Created by Kasugano Sora on 2023/5/23.
//

#include "BNF.h"
#include <set>
#include <string>
#include <vector>
#include <wchar.h>

BNF::BNF(map<string, std::vector<std::vector<string>>> expressions, string start_letter) : expressions(
        std::move(expressions)), starter(std::move(start_letter)), root(nullptr) {
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
    for (auto const &[left, right]: expressions) {
        bool flag = false;

        for (auto const &i: right) {
            if (i[0] != left)
                continue;
            flag = true;
            break;
        }

        if (not flag)
            return;

        string new_left = left + "'";
        std::vector<std::vector<string>> right1, right2;

        for (auto const &i: right) {
            std::vector<string> tmp;
            if (i[0] == left)
                tmp.insert(tmp.begin(), i.begin() + 1, i.end());
            else
                tmp.insert(tmp.begin(), i.begin(), i.end());
            tmp.push_back(new_left);
            (i[0] == left ? right2 : right1).push_back(tmp);
        }

        right2.push_back({empty});
        expressions[left] = right1;
        expressions[new_left] = right2;
        non_terminal.insert(new_left);
    }
}

void BNF::remove_indirect_left_recursion() {
    visited.clear();

}

void BNF::judge(const string &str) {
    if (not non_terminal.contains(str))
        return;

    visited.insert(str);
    for (int i = 0; i < expressions[str].size(); ++i) {
        auto &now = expressions[str][i];
        if (non_terminal.contains(now[0])) {
            if (str == now[0]) {
                remove_direct_left_recursion();
            }
            if (visited.contains(now[0])) {
                string target = now[0];
                now.erase(now.begin(), now.begin() + 1);
                replace(str, i, target, now);
                --i;
            } else
                judge(now[0]);
        }
    }
    visited.erase(str);
}

void BNF::replace(const string &left, int index, const string &str, const std::vector<string> &item) {
    auto const &right = expressions[str];
    std::vector<std::vector<string>> tmp(expressions[left].begin(), expressions[left].begin() + index);

    for (auto const &i: right) {
        std::vector<string> now(i);
        now.insert(now.begin(), item.begin(), item.end());
        tmp.emplace_back(std::move(now));
    }

    tmp.insert(tmp.begin(), expressions[left].begin() + index + 1, expressions[left].end());
    expressions[left] = tmp;
}

void BNF::process_first() {
    for (auto const &str: non_terminal)
        first[str] = std::move(get_first_single(str));
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
        res.merge(get_first(item));
    }

    return first[str] = res;
}

set<string> BNF::get_first(const std::vector<string> &item) {
    set<string> res;
    int i;

    for (i = 0; i < item.size(); ++i) {
        auto const &now = get_first_single(item[i]);
        res.insert(now.begin(), now.end());
        if (not now.contains(empty))
            break;
    }

    res.erase(empty);
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

void BNF::get_follow() {
    auto former_follow = follow;

    for (auto const &str: non_terminal) {
        follow[str].clear();
        if (str == starter)
            follow[str].insert("$");
    }

    while (not map_compare(former_follow, follow)) {
        former_follow = follow;

        for (auto const &[left, right]: expressions) {
            for (auto const &item: right) {
                for (int i = 0; i < item.size(); ++i) {
                    if (not non_terminal.contains(item[i]))
                        continue;

                    set<string> tmp_first, not_null;
                    std::vector<string> tmp(item.begin() + i + 1, item.end());
                    bool flag = tmp.empty();

                    tmp_first = get_first(tmp);
                    not_null = tmp_first;
                    tmp_first.erase(empty);
                    follow[item[i]].merge(not_null);

                    if (tmp_first.contains(empty) or not flag)
                        follow[item[i]].insert(follow[left].begin(), follow[left].end());
                }
            }
        }
    }
}

void BNF::get_terminal() {
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

void BNF::get_LL1() {
    for (auto const &str: non_terminal) {
        map<string, set<std::vector<string>>> tmp;

        for (auto const &s: terminal)
            tmp[s] = {};

        for (auto const &item: expressions[str]) {
            auto const &now = get_first(item);
            for (auto const &i: now) {
                tmp[i].insert(item);

                if (not now.contains(empty))
                    continue;
                for (auto const &j: follow[str])
                    tmp[j].insert(item);
            }
        }

        LL1[str] = tmp;
    }
}

void BNF::extract_left_common_factor() {
    for (auto const &str: non_terminal) {

    }
}

void BNF::get_longest_string(std::vector<std::vector<string>> &right, std::vector<string> &longest_string) {
    std::multiset<string> count;

    for (auto const &i: right)
        count.insert(i[0]);

    size_t max = 0;
    string most_common_string;
    for (auto const &i: count) {
        if (count.count(i) <= max)
            continue;

        max = count.count(i);
        most_common_string = i;
    }

    if (not max)
        return;

    longest_string.push_back(most_common_string);
    std::vector<std::vector<string>> tmp;

    for (auto &item: right) {
        if (item[0] != most_common_string)
            continue;

        std::vector<string> now(item.begin() + 1, item.end());
        if (now.empty())
            now.emplace_back(empty);

        tmp.emplace_back(std::move(now));
    }

    get_longest_string(tmp, longest_string);
}

void BNF::substitution() {
    for (auto &[left, right]: expressions) {
        auto p = right;

        for (int i = 0; i < p.size(); ++i) {
            if (not non_terminal.contains(p[i][0]))
                continue;

            auto const &target = expressions[left][i][0];

            expressions[left][i].erase(expressions[left][i].begin(), expressions[left][i].begin() + 1);
            replace(left, i, target, expressions[left][i]);
            --i;
            p = expressions[left];
        }
    }
}

