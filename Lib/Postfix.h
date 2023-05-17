//
// Created by Kasugano_Sora on 2023/5/16.
//

#ifndef COMPILATIONPRINCIPLE_POSTFIX_H
#define COMPILATIONPRINCIPLE_POSTFIX_H

#include <map>
#include <string>

using std::map;
using std::string;

class Postfix {
private:
    static map<char, int> priority;

    string expression;

    string result;

public:
    explicit Postfix(string expr);

    string get_result();
};


#endif //COMPILATIONPRINCIPLE_POSTFIX_H
