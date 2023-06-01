//
// Created by Kasugano_Sora on 2023/5/22.
//

#ifndef COMPILATIONPRINCIPLE_UTILS_H
#define COMPILATIONPRINCIPLE_UTILS_H

#include <string>
#include <regex>
#include <filesystem>

struct MyTuple {
    std::string show_num;
    std::string num;
};

namespace Utils {
    std::string const start = "program";
}

std::string trim(const std::string &string);

std::vector<std::string> split(std::string const &, std::string const &);

std::map<std::string, std::vector<std::vector<std::string>>> parse(const std::string &string);

std::vector<MyTuple> get_tuples(const std::string &string);

#endif //COMPILATIONPRINCIPLE_UTILS_H
