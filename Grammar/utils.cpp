//
// Created by Kasugano_Sora on 2023/5/22.
//

#include "utils.h"
#include <regex>
#include <iostream>
#include <vector>

std::string trim(const std::string &string) {
    return std::regex_replace(string, std::regex("(^ +)|( +$)"), "");
}

std::vector<std::string> split(const std::string &string, const std::string &delim) {
    std::vector<std::string> result;
    if (string.empty())
        return result;

    size_t head = 0;
    std::string str = string + delim;
    size_t const tail = str.size() - 1;
    while (head < tail) {
        auto const &now = str.substr(head);
        size_t pos = head + now.find(delim);

        result.emplace_back(trim(str.substr(head, pos - head)));
        head = pos + delim.size();
    }

    return result;
}

std::map<std::string, std::vector<std::vector<std::string>>> parse(const std::string &string) {
    std::map<std::string, std::vector<std::string>> mid;
    std::regex pattern(R"(^([^:]+)\s*:=\s*([^\n]+)$)", std::regex::optimize), sub_pattern(R"(([^|]+)(?: *\| *(.+))*)",
                                                                                          std::regex::optimize);
    std::smatch match_result, sub_match_result;
    auto lines = split(string, "\n");

    for (auto const &line: lines) {
        if (not std::regex_match(line, match_result, pattern))
            throw std::runtime_error("match");

        auto const &key = match_result[1].str();
        auto &entry = mid[key];
        std::string statements = match_result[2];

        while (std::regex_match(statements, sub_match_result, sub_pattern)) {
            entry.emplace_back(sub_match_result[1]);
            statements = sub_match_result[2];
        }
    }

    std::map<std::string, std::vector<std::vector<std::string>>> result;
    for (auto &[key, value]: mid) {
        for (auto const &item: value) {
            result[key].emplace_back(split(item, " "));
        }
    }

    return result;
}

std::vector<MyTuple> get_tuples(const std::string &string) {
    auto lines = split(string, "\n");
    std::vector<MyTuple> result;

    for (auto const &line: lines) {
        if (line.empty())
            continue;

        auto tmp = split(line, " ");
        if (tmp.size() == 2)
            result.emplace_back(MyTuple{tmp[1], tmp[0]});
        if (tmp.size() == 1)
            result.emplace_back(MyTuple{tmp[0], ""});
    }

    return std::move(result);
}