//
// Created by Kasugano_Sora on 2023/5/22.
//

#ifndef COMPILATIONPRINCIPLE_LEXPARSER_H
#define COMPILATIONPRINCIPLE_LEXPARSER_H

#include "nlohmann/json.hpp"
#include <string>
#include <set>
#include <utility>
#include <filesystem>

using json = nlohmann::json;

class LexParser {
private:
    std::string filename;

    std::set<std::string> keywords;
public:
    [[nodiscard]] const std::set<std::string> &get_keywords() const;

    [[nodiscard]] const std::set<std::pair<std::string, std::string>> &get_operators() const;

    [[nodiscard]] const std::set<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> &
    get_brackets() const;

    [[nodiscard]] const std::set<std::string> &get_comments() const;

    [[nodiscard]] const std::map<std::string, std::string> &get_tokens() const;

private:

    std::set<std::pair<std::string, std::string>> operators;

    std::set<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> brackets;

    std::set<std::string> comments;

    std::map<std::string, std::string> tokens;

    json config;
public:
    explicit LexParser(std::string path);
};


#endif //COMPILATIONPRINCIPLE_LEXPARSER_H
