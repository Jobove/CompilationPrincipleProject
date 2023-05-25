#include <ctype.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <cstdio>
#include "BNF.h"
#include "LexParser.h"
#include "MinimizedDFA.h"
#include "Postfix.h"
#include "boost/format.hpp"
#include "boost/format/format_fwd.hpp"

using std::cout;
using std::endl;

string read_sample() {
    string res;
    std::filesystem::path sample_path(
            R"(C:\Coding\Projects\CLionProjects\CompilationPrincipleProject\Config\sample.c)");
    std::ifstream fin(sample_path);
    std::string line;

    while (std::getline(fin, line)) {
        res += line + '\n';
    }
    fin.close();

    return res;
}

void fill_keywords(LexParser &lex_parser, boost::format &fmt) {
    string tmp;
    int j = 0;

    for (auto i: lex_parser.get_keywords()) {
        std::transform(i.begin(), i.end(), i.begin(), toupper);
        boost::format mid("#define KEYWORD_%s %d\n");
        mid % i % ++j;

        tmp += mid.str();
    }
    fmt % tmp;

    tmp.clear();
    for (auto i: lex_parser.get_keywords()) {
        boost::format mid("        [KEYWORD_%s] = \"%s\",\n");
        std::transform(i.begin(), i.end(), i.begin(), toupper);
        mid % i;
        std::transform(i.begin(), i.end(), i.begin(), tolower);
        mid % i;

        tmp += mid.str();
    }
    fmt % tmp;
}

void fill_operators(LexParser &lex_parser, boost::format &fmt) {
    string tmp;
    int j = 0;

    for (auto const &i: lex_parser.get_operators()) {
        boost::format mid("#define OPERATOR_%s %d\n");
        mid % i.first % ++j;

        tmp += mid.str();
    }
    fmt % tmp;

    tmp.clear();
    for (auto const &i: lex_parser.get_operators()) {
        boost::format mid("        [OPERATOR_%s] = \"%s\",\n");
        mid % i.first % (i.second == "%" ? "%%" : i.second);

        tmp += mid.str();
    }
    fmt % tmp;

    tmp.clear();
    for (auto const &i: lex_parser.get_operators()) {
        boost::format mid("        [OPERATOR_%s] = \"%s\",\n");
        mid % i.first % i.first;

        tmp += mid.str();
    }
    fmt % tmp;
}

void fill_brackets(LexParser &lex_parser, boost::format &fmt) {
    string tmp;
    int j = 0;

    for (auto const &i: lex_parser.get_brackets()) {
        boost::format x("#define BRACKET_%s %d\n#define BRACKET_%s %d\n");
        x % i.first.first % (j + 1) % i.second.first % (j + 2);
        j += 2;

        tmp += x.str();
    }
    fmt % tmp;

    tmp.clear();
    for (auto const &i: lex_parser.get_brackets()) {
        boost::format mid("        [BRACKET_%s] = \"%s\",\n        [BRACKET_%s] = \"%s\",\n");
        mid % i.first.first % i.first.second % i.second.first % i.second.second;

        tmp += mid.str();
    }
    fmt % tmp;

    tmp.clear();
    for (auto const &i: lex_parser.get_brackets()) {
        boost::format mid("        [BRACKET_%s] = \"%s\",\n        [BRACKET_%s] = \"%s\",\n");
        mid % i.first.first % i.first.first % i.second.first % i.second.first;

        tmp += mid.str();
    }
    fmt % tmp;
}

void fill_comments(LexParser &lex_parser, boost::format &fmt) {
    string tmp;
    int j = 0;
    for (auto const &i: lex_parser.get_comments()) {
        boost::format mid("        [%d] = \"%s\",\n");
        mid % ++j % i;
        tmp += mid.str();
    }
    fmt % tmp;
}

using adjacent_list_t = std::map<int, std::map<char, int>>;

string const template_outer = R"(
    int total_dfa = %d, pos = 0;
    while (pos < length) {
        while (pos < length &&
               (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t' || content[pos] == '\r' ||
                content[pos] == '\v'))
            ++pos;

        if (pos >= length)
            break;

        char res = try_match(output, content, &pos);
        if (res)
            continue;

        for (int i = 0; i < total_dfa; ++i) {
            // switch between DFAs.
            switch (i) {%s
            }
        }
        next:
        {
            ++pos;
        }
    }
)";

string const template_inner = R"(
                    int const end_len = %d, end[] = {%s};
                    int state = 1;
                    char const *token = "%s";
                    while (pos < length) {
                        char ch = content[pos];
                        switch (ch) {
                            case ' ':   // delimiter ch
                            case '\n':
                            case '\t':
                            case '\r':
                            case '\v': {
                                if (check_end(end, end_len, state)) {
                                    int r = pos - 1;
                                    char *tmp = (char *) malloc(sizeof(char) * (r - l + 1 + sizeof(token)) + 1);
                                    memset(tmp, 0, sizeof(char) * (r - l + 1 + sizeof(token)) + 1);
                                    memmove(tmp, content + l, sizeof(char) * (r - l + 1));
                                    strcat(tmp, " ");
                                    strcat(tmp, token);
                                    output_token(output, tmp);
                                    free(tmp);
                                    goto next;
                                } else {
                                    fprintf(output, "Error: Unexpected token '%%c'.\n", ch);
                                    goto unmatched_%d;
                                }
                            }
                        }
                        // switch between states.

                        switch (state) {
%s
                            default: {
                                goto unmatched_%d;
                            }
                        }
                        ++pos;
                        continue;
                        unmatched_%d:
                        {
                            fprintf(output, "Error: Unmatched token '%%c'.\n", ch);
                            while (pos < length && content[pos] != ' ' && content[pos] != '\n' &&
                                   content[pos] != '\t' && content[pos] != '\r' && content[pos] != '\v')
                                ++pos;
                        }
                    }
)";

string fill_token(const string &token, MinimizedDFA &dfa, int count) {
    adjacent_list_t adjacent_list = std::move(dfa.get_adjacent_list());
    size_t total_state = adjacent_list.size(), total_char = 0, total_end = dfa.count_end();
    boost::format fmt(template_inner);

    // 统计字符数
    for (auto &[u, next]: adjacent_list)
        total_char += next.size();

    fmt % total_end;
    string ends;
    for (auto i: dfa.get_end())
        ends += std::to_string(i) + ", ";
    ends.pop_back();
    ends.pop_back();
    fmt % ends % token % count;

    string cases;
    string const state_template = R"(                            case %d: {
                                switch (ch) {%s
                                }
                                break;
                            }
)";
    string const case_template = R"(
                                    case '%c': {
                                        state = %d;
                                        break;
                                    })";
    for (auto &[u, next]: adjacent_list) {
        string tmp;
        for (auto &[ch, v]: next) {
            boost::format mid(case_template);
            mid % ch % v;
            tmp += mid.str();
        }
        boost::format mid(state_template);
        mid % u % tmp;
        cases += mid.str();
    }
    fmt % cases % count % count;
    return fmt.str();
}

void fill_regex(std::vector<std::pair<string, MinimizedDFA>> &arr, boost::format &fmt) {
    size_t total_dfa = arr.size();
    int dfa_count = 0;

    static string const regex_template = R"(
                case %d: {%s
                    break;
                })";

    string tmp;
    for (auto &[token, dfa]: arr) {
        boost::format mid(regex_template);
        mid % dfa_count % fill_token(token, dfa, dfa_count);
        ++dfa_count;
        tmp += mid.str();
    }
    fmt % total_dfa % tmp;
}

int main() {
    std::filesystem::path lex(R"(C:\Coding\Projects\CLionProjects\CompilationPrincipleProject\Config\lex.json)");
    LexParser lex_parser(lex.string());
    string sample = std::move(read_sample());
    boost::format fmt(sample);

    read_sample();
    fill_keywords(lex_parser, fmt);
    fill_operators(lex_parser, fmt);
    fill_brackets(lex_parser, fmt);
    fill_comments(lex_parser, fmt);

    std::vector<std::pair<string, MinimizedDFA>> arr;
    for (auto const &[name, regex]: lex_parser.get_tokens()) {
        arr.emplace_back(name, Postfix(regex).get_result());
    }

    fill_regex(arr, fmt);

    std::ofstream out(R"(C:\Coding\Projects\CLionProjects\CompilationPrincipleProject\Config\output.c)",
                      std::ios::out | std::ios::trunc);
    out << fmt.str() << endl;
    out.close();

    return 0;
}
