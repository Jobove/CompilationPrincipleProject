#include <iostream>
#include "functional"
#include "Lib/Postfix.h"
#include "Lib/NFA.h"
#include "Lib/DFA.h"
#include "MinimizedDFA.h"

using std::cout;
using std::endl;

int main() {
    Postfix *postfix;

    postfix = new Postfix(
            "l(l|d)*"
            );

    cout << postfix->get_result() << endl;

    auto *dfa = new MinimizedDFA(postfix->get_result());

    dfa->output();

    return 0;
}
