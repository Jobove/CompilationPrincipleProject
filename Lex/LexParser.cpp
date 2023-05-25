//
// Created by Kasugano_Sora on 2023/5/22.
//

#include "LexParser.h"
#include <fstream>
#include <iostream>
#include <map>

LexParser::LexParser(std::string path) : filename(std::move(path)) {
    std::ifstream ifs(filename);
    config = json::parse(ifs);

    keywords = config["keywords"].get<std::set<std::string>>();
    operators = config["operators"].get<std::set<std::string>>();
    for (auto const &i: config["delimiters"].get<std::vector<std::vector<std::string>>>())
        delimiters.insert(std::make_pair(i[0][0], i[1][0]));
    comment = config["comment"].get<std::set<std::string>>();
    tokens = config["tokens"].get<std::map<std::string, std::string>>();
}

const std::set<std::string> &LexParser::get_keywords() const {
    return keywords;
}

const std::set<std::string> &LexParser::get_operators() const {
    return operators;
}

const std::set<std::pair<char, char>> &LexParser::get_delimiters() const {
    return delimiters;
}

const std::set<std::string> &LexParser::get_comment() const {
    return comment;
}

const std::map<std::string, std::string> &LexParser::get_tokens() const {
    return tokens;
}
