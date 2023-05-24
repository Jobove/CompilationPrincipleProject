#include <iostream>
#include "utils.h"
#include "nlohmann/json.hpp"

using std::cout;
using std::endl;
using json = nlohmann::json;

int main() {
//    std::string content = read(R"(C:\Coding\Projects\CLionProjects\CompilationPrincipleProject\Config\grammar)");
    json j = {
            {"pi",      3.141},
            {"happy",   true},
            {"name",    "Niels"},
            {"nothing", nullptr},
            {"answer",  {
                                {"everything", 42}
                        }
            },
            {"list",    {1, 0, 2}},
            {"object",  {
                                {"currency",   "USD"},
                                   {"value", 42.99}
                        }
            }
    };
    cout << j["list"][1] << endl;
}
