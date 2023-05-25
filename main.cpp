#include <array>
#include <iostream>
#include <vector>
#include "BNF.h"
#include "Postfix.h"
#include "utils.h"
#include "nlohmann/json.hpp"
#include "MinimizedDFA.h"
#include "LexParser.h"

using std::cout;
using std::endl;
using json = nlohmann::json;

int main() {
    std::filesystem::path lex(R"(D:\Coding\Projects\CLionProjects\CompilationPrincipleProject\Config\lex.json)");
    LexParser lexParser(lex.string());
    map<string, MinimizedDFA> m;
    std::vector<string> strings;

    strings.insert(strings.end(), lexParser.get_keywords().begin(), lexParser.get_keywords().end());
    strings.insert(strings.end(), lexParser.get_operators().begin(), lexParser.get_operators().end());

    for (auto const &str: strings)
        Postfix(str).get_result();

    return 0;
}
