//
// Created by Kasugano_Sora on 2023/5/16.
//

#include "Postfix.h"
#include <stack>

map<char,int> Postfix::priority{
        {'+', 3},
        {'*', 3},
        {'?', 3},
        {'.', 2},
        {'|', 1},
        {'(', 0}
};

Postfix::Postfix(string expr) : expression(std::move(expr)){
    result.clear();
}

string Postfix::get_result() {
    if (not result.empty())
        return result;

    std::stack<char> operators;
    string process;
    bool is_last_operand = true;

    for (char ch : expression) {
        if (priority.find(ch) != priority.end() and ch != '(') {
            process += ch;
            is_last_operand = true;
        } else {
            if (ch != ')' and not is_last_operand)
                process += '.';

            process += ch;
            is_last_operand = (ch == '(');
        }
    }

    for (char ch : process) {
        if (priority.find(ch) != priority.end() and ch != '(') {
            char op;
            while (not operators.empty() and (op = operators.top()) and priority[op] >= priority[ch]) {
                result += op;
                operators.pop();
            }
            operators.push(ch);
        } else if (ch == '(') {
            operators.push(ch);
        } else if (ch == ')') {
            while (operators.top() != '(') {
                result += operators.top();
                operators.pop();
            }
            operators.pop();
        } else {
            result += ch;
        }
    }

    while (not operators.empty()) {
        result += operators.top();
        operators.pop();
    }

    return result;
}
