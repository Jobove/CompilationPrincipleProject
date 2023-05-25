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
    const std::set<std::string> &get_keywords() const;

    const std::set<std::string> &get_operators() const;

    const std::set<std::pair<char, char>> &get_delimiters() const;

    const std::set<std::string> &get_comment() const;

    const std::map<std::string, std::string> &get_tokens() const;

private:

    std::set<std::string> operators;

    std::set<std::pair<char, char>> delimiters;

    std::set<std::string> comment;

    std::map<std::string, std::string> tokens;

    json config;
public:
    explicit LexParser(std::string path);
};


#endif //COMPILATIONPRINCIPLE_LEXPARSER_H
