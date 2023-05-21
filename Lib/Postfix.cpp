//
// Created by Kasugano_Sora on 2023/5/16.
//

#include "Postfix.h"
#include <stack>
#include <set>

map<char,int> Postfix::priority{
        {'+', 3},
        {'*', 3},
        {'?', 3},
        {'.', 2},
        {'|', 1},
        {'(', 0}
};

std::set<char> const unary_operator{
    '+', '*', '?'
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

    auto check_unary = [&](char ch) {
        return unary_operator.find(ch) != unary_operator.end();
    };

    auto check_operator = [&](char ch) {
        return priority.find(ch) != priority.end();
    };

    for (int i = 0; i < expression.size(); ++i) {
        char ch = expression[i];
        bool now = check_unary(ch) or not check_operator(ch) or ch == ')';

        process += ch;
        if (not now)
            continue;

        bool next = i < expression.size() - 1 and
                (not check_operator(expression[i + 1]) or expression[i + 1] == '(') and
                expression[i + 1] != ')';
        if (not next)
            continue;

        process += '.';
    }

//    for (char ch : expression) {
//        if (priority.find(ch) != priority.end() and ch != '(') {
//            process += ch;
//            is_last_operand = true;
//        } else {
//            if (ch != ')' and not is_last_operand)
//                process += '.';
//
//            process += ch;
//            is_last_operand = (ch == '(');
//        }
//    }

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
