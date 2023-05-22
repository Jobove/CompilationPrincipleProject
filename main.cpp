#include <iostream>
#include "LexParser.h"
#include "utils.h"

using std::cout;
using std::endl;

int main() {
//    LexParser lexParser(R"(D:\Coding\Projects\CLionProjects\CompilationPrincipleProject\Config\lex)");
    std::string string = "   123   ";
    trim(string);
    cout << string << endl;
}
