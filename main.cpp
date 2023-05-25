#include <iostream>
#include "LexParser.h"

using std::cout;
using std::endl;

int main() {
    cout << get_lexer(R"(C:\Coding\Projects\CLionProjects\CompilationPrincipleProject\Config\lex.json)") << endl;

    return 0;
}
