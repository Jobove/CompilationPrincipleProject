//
// Created by Kasugano_Sora on 2023/5/22.
//

#include "utils.h"
#include <regex>

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
        size_t pos = str.find(delim);

        result.emplace_back(str.substr(head, pos - head));
        head = pos + delim.size();
    }


    return result;
}
