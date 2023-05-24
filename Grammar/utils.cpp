//
// Created by Kasugano_Sora on 2023/5/22.
//

#include "utils.h"
#include <regex>
#include <fstream>
#include <iostream>

void trim(std::string &string) {
    string = std::regex_replace(string, std::regex("(^ +)|( +$)"), "");
}

std::vector<std::string> split(const std::string &string, const std::string &delim) {
    std::vector<std::string> result;
    if (string.empty())
        return result;

    size_t head = 0;
    size_t const tail = string.size() - 1;
    std::string str = string + delim;
    while (head < tail) {
        auto const &now = str.substr(head);
        size_t pos = head + now.find(delim);

        result.emplace_back(str.substr(head, pos - head));
        head = pos + delim.size();
    }


    return result;
}

std::map<std::string, std::vector<std::string>> parse(const std::string &string) {
    std::map<std::string, std::vector<std::string>> result;
    std::regex pattern(R"(^([^:]+)\s*:=\s*([^\n]+)$)", std::regex::optimize),
    sub_pattern(R"(([^|]+)(?: *\| *(.+))*)", std::regex::optimize);
    std::smatch match_result, sub_match_result;
    auto lines = split(string, "\n");

    for (auto const &line : lines) {
        if (not std::regex_match(line, match_result, pattern))
            throw std::runtime_error("match");

        auto &entry = result[match_result[1].str()];
        std::string statements = match_result[2];

        while (std::regex_match(statements, sub_match_result, sub_pattern)) {
            entry.emplace_back(sub_match_result[1]);
            statements = sub_match_result[2];
        }
    }

    return result;
}

std::string read(const std::filesystem::path& path) {
    std::ifstream input;
    std::string result;

    input.open(path);
    while (not input.eof()) {
        result += (char) input.get();
    }

    return result;
}
