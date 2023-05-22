//
// Created by Kasugano_Sora on 2023/5/22.
//

#include "LexParser.h"
#include <regex>
#include <fstream>
#include <iostream>

std::string const LexParser::normal = R"([^\\]+)";

std::string const LexParser::node = R"(\\([a-zA-Z]+) = (.+))";

LexParser::LexParser(std::string path) : filename(std::move(path)) {
    std::ifstream ifs;
    ifs.open(filename, std::ios::in);

    while (not ifs.eof()) {
        content += char (ifs.get());
    }

    std::stringstream stringstream((std::string(content)));
    while (not stringstream.eof()) {
        std::string line;
        while (not stringstream.eof() and stringstream.peek() != '\r') {
            line += char (stringstream.get());
        }

        std::cout << line << std::endl;
        std::regex node_regex(node), normal_regex(normal);
        if (std::regex_match(line, node_regex)) {

        }
    }

    std::cout << content;
}

