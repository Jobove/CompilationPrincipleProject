//
// Created by Kasugano_Sora on 2023/5/22.
//

#include "LexParser.h"
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

LexParser::LexParser(std::string path) : filename(std::move(path)) {
    std::ifstream ifs(filename);
    config = json::parse(ifs);

    keywords = config["keywords"].get<std::set<std::string>>();
    operators = config["operators"].get<std::set<std::pair<std::string, std::string>>>();
    brackets = config["brackets"].get<std::set<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>>();
    comments = config["comment"].get<std::set<std::string>>();
    tokens = config["tokens"].get<std::map<std::string, std::string>>();
}

const std::set<std::string> &LexParser::get_keywords() const {
    return keywords;
}

const std::set<std::pair<std::string, std::string>> &LexParser::get_operators() const {
    return operators;
}

const std::set<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> &
LexParser::get_brackets() const {
    return brackets;
}

const std::set<std::string> &LexParser::get_comments() const {
    return comments;
}

const std::map<std::string, std::string> &LexParser::get_tokens() const {
    return tokens;
}
