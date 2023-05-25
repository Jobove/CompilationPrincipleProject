#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define KEYWORD_ELSE 1
#define KEYWORD_IF 2
#define KEYWORD_INT 3
#define KEYWORD_FLOAT 4
#define KEYWORD_RETURN 5
#define KEYWORD_VOID 6
#define KEYWORD_DO 7
#define KEYWORD_WHILE 8

const char *keywords[] = {
//        [KEYWORD_ELSE] = "else",
//        [KEYWORD_IF] = "if",
//        [KEYWORD_INT] = "int",
//        [KEYWORD_FLOAT] = "float",
//        [KEYWORD_RETURN] = "return",
//        [KEYWORD_VOID] = "void",
//        [KEYWORD_DO] = "do",
//        [KEYWORD_WHILE] = "while"
};

const char *token_keywords[] = {
//        [KEYWORD_ELSE] = "ELSE",
//        [KEYWORD_IF] = "IF",
//        [KEYWORD_INT] = "INT",
//        [KEYWORD_FLOAT] = "FLOAT",
//        [KEYWORD_RETURN] = "RETURN",
//        [KEYWORD_VOID] = "VOID",
//        [KEYWORD_DO] = "DO",
//        [KEYWORD_WHILE] = "WHILE"
};

#define OPERATOR_ADD 1
#define OPERATOR_SUB 2
#define OPERATOR_MUL 3
#define OPERATOR_DIV 4
#define OPERATOR_MOD 5
#define OPERATOR_LT 6
#define OPERATOR_LE 7
#define OPERATOR_GT 8
#define OPERATOR_GE 9
#define OPERATOR_EQ 10
#define OPERATOR_NE 11
#define OPERATOR_ASSIGN 12
#define OPERATOR_SEMICOLON 13
#define OPERATOR_COMMA 14

const char *operators[] = {
        [OPERATOR_ADD] = "+",
        [OPERATOR_SUB] = "-",
        [OPERATOR_MUL] = "*",
        [OPERATOR_DIV] = "/",
        [OPERATOR_MOD] = "%",
        [OPERATOR_LT] = "<",
        [OPERATOR_LE] = "<=",
        [OPERATOR_GT] = ">",
        [OPERATOR_GE] = ">=",
        [OPERATOR_EQ] = "==",
        [OPERATOR_NE] = "!=",
        [OPERATOR_ASSIGN] = "=",
        [OPERATOR_SEMICOLON] = ";",
        [OPERATOR_COMMA] = ","
};

const char *token_operators[] = {
        [OPERATOR_ADD] = "ADD",
        [OPERATOR_SUB] = "SUB",
        [OPERATOR_MUL] = "MUL",
        [OPERATOR_DIV] = "DIV",
        [OPERATOR_MOD] = "MOD",
        [OPERATOR_LT] = "LT",
        [OPERATOR_LE] = "LE",
        [OPERATOR_GT] = "GT",
        [OPERATOR_GE] = "GE",
        [OPERATOR_EQ] = "EQ",
        [OPERATOR_NE] = "NE",
        [OPERATOR_ASSIGN] = "ASSIGN",
        [OPERATOR_SEMICOLON] = "SEMICOLON",
        [OPERATOR_COMMA] = "COMMA"
};

#define BRACKET_LPAREN 1
#define BRACKET_RPAREN 2
#define BRACKET_LBRACK 3
#define BRACKET_RBRACK 4
#define BRACKET_LBRACE 5
#define BRACKET_RBRACE 6

const char *brackets[] = {
        [BRACKET_LPAREN] = "(",
        [BRACKET_RPAREN] = ")",
        [BRACKET_LBRACK] = "[",
        [BRACKET_RBRACK] = "]",
        [BRACKET_LBRACE] = "{",
        [BRACKET_RBRACE] = "}"
};

const char *token_brackets[] = {
        [BRACKET_LPAREN] = "LPAREN",
        [BRACKET_RPAREN] = "RPAREN",
        [BRACKET_LBRACK] = "LBRACK",
        [BRACKET_RBRACK] = "RBRACK",
        [BRACKET_LBRACE] = "LBRACE",
        [BRACKET_RBRACE] = "RBRACE"
};

void *read_file(const char *filepath, char *buf, int n) {
    FILE *file = fopen(filepath, "r");
    fgets(buf, n, file);

    fclose(file);
    return buf;
}

char check_end(const int *end, int end_len, int state) {
    for (int i = 0; i < end_len; ++i) {
        if (end[i] == state)
            return 1;
    }
    return 0;
}

void output_token(FILE *output, const char *token) {
    fprintf(output, "%s\n", token);
}

char try_match(FILE *output, const char *content, int *p) {
    // try keywords.
    for (int i = 0; i < sizeof(keywords) / sizeof(char *); ++i) {
        size_t len = strlen(keywords[i]);
        if (strncmp(content + *p, keywords[i], len) != 0)
            continue;

        output_token(output, token_keywords[i]);
        *p += (int) len;
        return 1;
    }
    // try operators.
    for (int i = 0; i < sizeof(operators) / sizeof(char *); ++i) {
        size_t len = strlen(operators[i]);
        if (strncmp(content + *p, operators[i], len) != 0)
            continue;

        output_token(output, token_operators[i]);
        *p += (int) len;
        return 1;
    }
    // try brackets.
    for (int i = 0; i < sizeof(brackets) / sizeof(char *); ++i) {
        size_t len = strlen(brackets[i]);
        if (strncmp(content + *p, brackets[i], len) != 0)
            continue;

        output_token(output, token_brackets[i]);
        *p += (int) len;
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Too few arguments! Usage: Lex.exe [Input file path] [Output file path]\n");
        exit(-1);
    }

    int size = 10000;
    char *input = argv[1], *buf = malloc(size), *content = read_file(input, buf, size);
    size_t length = strlen(content);
    FILE *output = fopen(argv[2], "w+");

    int total_dfa = %d, pos = 0;
    while (pos < length) {
        while (pos < length &&
               (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t' || content[pos] == '\r' ||
                content[pos] == '\v'))
            ++pos;

        char res = try_match(output, content, &pos);
        if (res)
            continue;

        for (int i = 0; i < total_dfa; ++i) {
            // switch between DFAs.
            switch (i) {
                case %d:
                {
                    int const end_len = %d, end[] = {%s};
                    int state = 0;
                    char const *token = "%s";
                    while (pos < length) {
                        char ch = content[pos];
                        // switch between states.
                        switch (ch) {
                            case '%c': {
                            }
                                //...

                            case ' ':   // delimiter ch
                            case '\n':
                            case '\t':
                            case '\r':
                            case '\v': {
                                if (check_end(end, end_len, state)) {
                                    output_token(output, token);
                                    goto next;
                                } else {
                                    fprintf(output, "Error: Unexpected token '%c'.\n", ch);
                                    goto unmatched;
                                }
                            }
                            default: {
                                goto unmatched;
                            }
                        }
                        unmatched:
                        {
                            fprintf(output, "Error: Unmatched token '%c'.\n", ch);
                            while (pos < length && content[pos] != ' ' && content[pos] != '\n' &&
                                   content[pos] != '\t' && content[pos] != '\r' && content[pos] != '\v')
                                ++pos;
                        }
                    }
                }
                    // ...
            }
        }
        next:
        {
            ++pos;
        }
    }
    eof:
    {
        fprintf(output, "Error: Unexpected EOF.\n");
    }
}