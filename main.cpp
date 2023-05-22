#include <iostream>
#include "functional"
#include "Lex/Postfix.h"
#include "Lex/NFA.h"
#include "Lex/DFA.h"
#include "Lex/MinimizedDFA.h"

using std::cout;
using std::endl;

int main() {
    Postfix *postfix;

    postfix = new Postfix(
            "l(l|d)?"
            );

    cout << postfix->get_result() << endl;

    auto *dfa = new MinimizedDFA(postfix->get_result());

    dfa->output();

    cout << dfa->match("lll") << endl;

    return 0;
}
