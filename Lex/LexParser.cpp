//
// Created by Kasugano_Sora on 2023/5/22.
//

#include "LexParser.h"
#include "Postfix.h"
#include "boost/format.hpp"
#include <fstream>
#include <map>
#include <vector>

LexParser::LexParser(std::string path) : filename(std::move(path)) {
    std::ifstream ifs(filename);
    config = json::parse(ifs);

    keywords = config["keywords"].get<std::set<std::string>>();
    operators = config["operators"].get<std::set<std::pair<std::string, std::string>>>();
    brackets = config["brackets"].get<std::set<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>>();
    comments = config["comment"].get<std::set<std::string>>();
    tokens = config["tokens"].get<std::map<std::string, std::string>>();
}

const std::set<std::string> &LexParser::get_keywords() const {
    return keywords;
}

const std::set<std::pair<std::string, std::string>> &LexParser::get_operators() const {
    return operators;
}

const std::set<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> &
LexParser::get_brackets() const {
    return brackets;
}

const std::set<std::string> &LexParser::get_comments() const {
    return comments;
}

const std::map<std::string, std::string> &LexParser::get_tokens() const {
    return tokens;
}

std::string read_sample(const std::filesystem::path &sample_path) {
    std::string res;
    std::ifstream fin(sample_path);
    std::string line;

    while (std::getline(fin, line)) {
        res += line + '\n';
    }
    fin.close();

    return res;
}

void fill_keywords(LexParser &lex_parser, boost::format &fmt) {
    std::string tmp;
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
    std::string tmp;
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
    std::string tmp;
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
    std::string tmp;
    int j = 0;
    for (auto const &i: lex_parser.get_comments()) {
        boost::format mid("        [%d] = \"%s\",\n");
        mid % ++j % i;
        tmp += mid.str();
    }
    fmt % tmp;
}

using adjacent_list_t = std::map<int, std::map<char, int>>;

std::string const template_inner = R"(
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
                                    char *tmp = (char *) malloc(sizeof(char) * (r - l + 1 + strlen(token)) + 1);
                                    memset(tmp, 0, sizeof(char) * (r - l + 1 + strlen(token)) + 1);
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

std::string fill_token(const string &token, MinimizedDFA &dfa, int count) {
    adjacent_list_t adjacent_list = std::move(dfa.get_list());
    size_t total_state = adjacent_list.size(), total_char = 0, total_end = dfa.count_end();
    boost::format fmt(template_inner);

    // 统计字符数
    for (auto &[u, next]: adjacent_list)
        total_char += next.size();

    fmt % total_end;
    std::string ends;
    for (auto i: dfa.get_end())
        ends += std::to_string(i) + ", ";
    ends.pop_back();
    ends.pop_back();
    fmt % ends % token % count;

    std::string cases;
    std::string const state_template = R"(                            case %d: {
                                switch (ch) {%s
                                }
                                break;
                            }
)";
    std::string const case_template = R"(
                                    case '%c': {
                                        state = %d;
                                        break;
                                    })";
    for (auto &[u, next]: adjacent_list) {
        std::string tmp;
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

void fill_regex(std::vector<std::pair<std::string, MinimizedDFA>> &arr, boost::format &fmt) {
    size_t total_dfa = arr.size();
    int dfa_count = 0;

    static std::string const regex_template = R"(
                case %d: {%s
                    break;
                })";

    std::string tmp;
    for (auto &[token, dfa]: arr) {
        boost::format mid(regex_template);
        mid % dfa_count % fill_token(token, dfa, dfa_count);
        ++dfa_count;
        tmp += mid.str();
    }
    fmt % total_dfa % tmp;
}

std::vector<MinimizedDFA> get_lexer(const std::filesystem::path &config_path, std::string &lexer) {
    std::filesystem::path lex(config_path);
    LexParser lex_parser(lex.string());
    std::string sample = std::move(
            read_sample(R"(C:\Coding\Projects\CLionProjects\CompilationPrincipleProject\Config\sample.c)"));
    boost::format fmt(sample);

    fill_keywords(lex_parser, fmt);
    fill_operators(lex_parser, fmt);
    fill_brackets(lex_parser, fmt);
    fill_comments(lex_parser, fmt);

    std::vector<std::pair<std::string, MinimizedDFA>> arr;
    std::vector<MinimizedDFA> result;
    for (auto const &[name, regex]: lex_parser.get_tokens()) {
        arr.emplace_back(name, Postfix(regex).get_result());
        result.emplace_back(arr.back().second);
    }

    fill_regex(arr, fmt);

    lexer = fmt.str();
    return result;
}
