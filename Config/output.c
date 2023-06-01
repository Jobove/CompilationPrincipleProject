#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define KEYWORD_DO 1
#define KEYWORD_ELSE 2
#define KEYWORD_FLOAT 3
#define KEYWORD_IF 4
#define KEYWORD_INT 5
#define KEYWORD_RETURN 6
#define KEYWORD_VOID 7
#define KEYWORD_WHILE 8

const char *keywords[] = {
        [KEYWORD_DO] = "do",
        [KEYWORD_ELSE] = "else",
        [KEYWORD_FLOAT] = "float",
        [KEYWORD_IF] = "if",
        [KEYWORD_INT] = "int",
        [KEYWORD_RETURN] = "return",
        [KEYWORD_VOID] = "void",
        [KEYWORD_WHILE] = "while",
};

#define OPERATOR_ADD 1
#define OPERATOR_ASSIGN 2
#define OPERATOR_COMMA 3
#define OPERATOR_DIV 4
#define OPERATOR_EQ 5
#define OPERATOR_GE 6
#define OPERATOR_GT 7
#define OPERATOR_LE 8
#define OPERATOR_LT 9
#define OPERATOR_MOD 10
#define OPERATOR_MUL 11
#define OPERATOR_NE 12
#define OPERATOR_SEMICOLON 13
#define OPERATOR_SUB 14

const char *operators[] = {
        [OPERATOR_ADD] = "+",
        [OPERATOR_ASSIGN] = "=",
        [OPERATOR_COMMA] = ",",
        [OPERATOR_DIV] = "/",
        [OPERATOR_EQ] = "==",
        [OPERATOR_GE] = ">=",
        [OPERATOR_GT] = ">",
        [OPERATOR_LE] = "<=",
        [OPERATOR_LT] = "<",
        [OPERATOR_MOD] = "%%",
        [OPERATOR_MUL] = "*",
        [OPERATOR_NE] = "!=",
        [OPERATOR_SEMICOLON] = ";",
        [OPERATOR_SUB] = "-",
};

const char *token_operators[] = {
        [OPERATOR_ADD] = "ADD",
        [OPERATOR_ASSIGN] = "ASSIGN",
        [OPERATOR_COMMA] = "COMMA",
        [OPERATOR_DIV] = "DIV",
        [OPERATOR_EQ] = "EQ",
        [OPERATOR_GE] = "GE",
        [OPERATOR_GT] = "GT",
        [OPERATOR_LE] = "LE",
        [OPERATOR_LT] = "LT",
        [OPERATOR_MOD] = "MOD",
        [OPERATOR_MUL] = "MUL",
        [OPERATOR_NE] = "NE",
        [OPERATOR_SEMICOLON] = "SEMICOLON",
        [OPERATOR_SUB] = "SUB",
};

#define BRACKET_LBRACE 1
#define BRACKET_RBRACE 2
#define BRACKET_LBRACKET 3
#define BRACKET_RBRACKET 4
#define BRACKET_LPAREN 5
#define BRACKET_RPAREN 6

const char *brackets[] = {
        [BRACKET_LBRACE] = "{",
        [BRACKET_RBRACE] = "}",
        [BRACKET_LBRACKET] = "[",
        [BRACKET_RBRACKET] = "]",
        [BRACKET_LPAREN] = "(",
        [BRACKET_RPAREN] = ")",
};

const char *token_brackets[] = {
        [BRACKET_LBRACE] = "LBRACE",
        [BRACKET_RBRACE] = "RBRACE",
        [BRACKET_LBRACKET] = "LBRACKET",
        [BRACKET_RBRACKET] = "RBRACKET",
        [BRACKET_LPAREN] = "LPAREN",
        [BRACKET_RPAREN] = "RPAREN",
};

const char *comments[] = {
        [1] = "//",
};

void *read_file(const char *filepath, char *buf, int n) {
    FILE *file = fopen(filepath, "r");

    memset(buf, 0, n);
    for (int i = 0; i < n - 1 && !feof(file); ++i) {
        buf[i] = (char) fgetc(file);
    }

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
    fflush(output);
}

void output_comment(FILE *output, const char *content, int l, int r) {
    fprintf(output, "//");
    for (int i = l; i <= r; ++i) {
        fprintf(output, "%c", content[i]);
    }
    fflush(output);
}

char try_match(FILE *output, const char *content, int *p) {
    // try comments.
    for (int i = 1; i < sizeof(comments) / sizeof(char *); ++i) {
        size_t len = strlen(comments[i]);
        if (strncmp(content + *p, comments[i], len) != 0)
            continue;

        int l = *p + 2, r;
        while (content[*p] != '\n' && content[*p] != '\0')
            ++*p;
        r = *p;
//        output_comment(output, content, l, r);
        return 1;
    }
    // try keywords.
    for (int i = 1; i < sizeof(keywords) / sizeof(char *); ++i) {
        size_t len = strlen(keywords[i]);
        if (strncmp(content + *p, keywords[i], len) != 0)
            continue;

        output_token(output, keywords[i]);
        *p += (int) len;
        return 1;
    }
    // try operators.
    for (int i = 1; i < sizeof(operators) / sizeof(char *); ++i) {
        size_t len = strlen(operators[i]);
        if (strncmp(content + *p, operators[i], len) != 0)
            continue;

        output_token(output, operators[i]);
        *p += (int) len;
        return 1;
    }
    // try brackets.
    for (int i = 1; i < sizeof(brackets) / sizeof(char *); ++i) {
        size_t len = strlen(brackets[i]);
        if (strncmp(content + *p, brackets[i], len) != 0)
            continue;

        output_token(output, brackets[i]);
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

    int total_dfa = 2, pos = 0, l;
    while (pos < length) {
        while (pos < length &&
               (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t' || content[pos] == '\r' ||
                content[pos] == '\v'))
            l = ++pos;

        if (pos >= length)
            break;

        char res = try_match(output, content, &pos);
        if (res)
            continue;

        for (int i = 0; i < total_dfa; ++i) {
            // switch between DFAs.
            switch (i) {

                case 0: {
                    int const end_len = 1, end[] = {2};
                    int state = 1;
                    char const *token = "ID";
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
                                    fprintf(output, "Error: Unexpected token '%c'.\n", ch);
                                    goto unmatched_0;
                                }
                            }
                        }
                        // switch between states.

                        switch (state) {
                            case 1: {
                                switch (ch) {
                                    case 'A': {
                                        state = 2;
                                        break;
                                    }
                                    case 'B': {
                                        state = 2;
                                        break;
                                    }
                                    case 'C': {
                                        state = 2;
                                        break;
                                    }
                                    case 'D': {
                                        state = 2;
                                        break;
                                    }
                                    case 'E': {
                                        state = 2;
                                        break;
                                    }
                                    case 'F': {
                                        state = 2;
                                        break;
                                    }
                                    case 'G': {
                                        state = 2;
                                        break;
                                    }
                                    case 'H': {
                                        state = 2;
                                        break;
                                    }
                                    case 'I': {
                                        state = 2;
                                        break;
                                    }
                                    case 'J': {
                                        state = 2;
                                        break;
                                    }
                                    case 'K': {
                                        state = 2;
                                        break;
                                    }
                                    case 'L': {
                                        state = 2;
                                        break;
                                    }
                                    case 'M': {
                                        state = 2;
                                        break;
                                    }
                                    case 'N': {
                                        state = 2;
                                        break;
                                    }
                                    case 'O': {
                                        state = 2;
                                        break;
                                    }
                                    case 'P': {
                                        state = 2;
                                        break;
                                    }
                                    case 'Q': {
                                        state = 2;
                                        break;
                                    }
                                    case 'R': {
                                        state = 2;
                                        break;
                                    }
                                    case 'S': {
                                        state = 2;
                                        break;
                                    }
                                    case 'T': {
                                        state = 2;
                                        break;
                                    }
                                    case 'U': {
                                        state = 2;
                                        break;
                                    }
                                    case 'V': {
                                        state = 2;
                                        break;
                                    }
                                    case 'W': {
                                        state = 2;
                                        break;
                                    }
                                    case 'X': {
                                        state = 2;
                                        break;
                                    }
                                    case 'Y': {
                                        state = 2;
                                        break;
                                    }
                                    case 'Z': {
                                        state = 2;
                                        break;
                                    }
                                    case '_': {
                                        state = 2;
                                        break;
                                    }
                                    case 'a': {
                                        state = 2;
                                        break;
                                    }
                                    case 'b': {
                                        state = 2;
                                        break;
                                    }
                                    case 'c': {
                                        state = 2;
                                        break;
                                    }
                                    case 'd': {
                                        state = 2;
                                        break;
                                    }
                                    case 'e': {
                                        state = 2;
                                        break;
                                    }
                                    case 'f': {
                                        state = 2;
                                        break;
                                    }
                                    case 'g': {
                                        state = 2;
                                        break;
                                    }
                                    case 'h': {
                                        state = 2;
                                        break;
                                    }
                                    case 'i': {
                                        state = 2;
                                        break;
                                    }
                                    case 'j': {
                                        state = 2;
                                        break;
                                    }
                                    case 'k': {
                                        state = 2;
                                        break;
                                    }
                                    case 'l': {
                                        state = 2;
                                        break;
                                    }
                                    case 'm': {
                                        state = 2;
                                        break;
                                    }
                                    case 'n': {
                                        state = 2;
                                        break;
                                    }
                                    case 'o': {
                                        state = 2;
                                        break;
                                    }
                                    case 'p': {
                                        state = 2;
                                        break;
                                    }
                                    case 'q': {
                                        state = 2;
                                        break;
                                    }
                                    case 'r': {
                                        state = 2;
                                        break;
                                    }
                                    case 's': {
                                        state = 2;
                                        break;
                                    }
                                    case 't': {
                                        state = 2;
                                        break;
                                    }
                                    case 'u': {
                                        state = 2;
                                        break;
                                    }
                                    case 'v': {
                                        state = 2;
                                        break;
                                    }
                                    case 'w': {
                                        state = 2;
                                        break;
                                    }
                                    case 'x': {
                                        state = 2;
                                        break;
                                    }
                                    case 'y': {
                                        state = 2;
                                        break;
                                    }
                                    case 'z': {
                                        state = 2;
                                        break;
                                    }
                                }
                                break;
                            }
                            case 2: {
                                switch (ch) {
                                    case '0': {
                                        state = 2;
                                        break;
                                    }
                                    case '1': {
                                        state = 2;
                                        break;
                                    }
                                    case '2': {
                                        state = 2;
                                        break;
                                    }
                                    case '3': {
                                        state = 2;
                                        break;
                                    }
                                    case '4': {
                                        state = 2;
                                        break;
                                    }
                                    case '5': {
                                        state = 2;
                                        break;
                                    }
                                    case '6': {
                                        state = 2;
                                        break;
                                    }
                                    case '7': {
                                        state = 2;
                                        break;
                                    }
                                    case '8': {
                                        state = 2;
                                        break;
                                    }
                                    case '9': {
                                        state = 2;
                                        break;
                                    }
                                    case 'A': {
                                        state = 2;
                                        break;
                                    }
                                    case 'B': {
                                        state = 2;
                                        break;
                                    }
                                    case 'C': {
                                        state = 2;
                                        break;
                                    }
                                    case 'D': {
                                        state = 2;
                                        break;
                                    }
                                    case 'E': {
                                        state = 2;
                                        break;
                                    }
                                    case 'F': {
                                        state = 2;
                                        break;
                                    }
                                    case 'G': {
                                        state = 2;
                                        break;
                                    }
                                    case 'H': {
                                        state = 2;
                                        break;
                                    }
                                    case 'I': {
                                        state = 2;
                                        break;
                                    }
                                    case 'J': {
                                        state = 2;
                                        break;
                                    }
                                    case 'K': {
                                        state = 2;
                                        break;
                                    }
                                    case 'L': {
                                        state = 2;
                                        break;
                                    }
                                    case 'M': {
                                        state = 2;
                                        break;
                                    }
                                    case 'N': {
                                        state = 2;
                                        break;
                                    }
                                    case 'O': {
                                        state = 2;
                                        break;
                                    }
                                    case 'P': {
                                        state = 2;
                                        break;
                                    }
                                    case 'Q': {
                                        state = 2;
                                        break;
                                    }
                                    case 'R': {
                                        state = 2;
                                        break;
                                    }
                                    case 'S': {
                                        state = 2;
                                        break;
                                    }
                                    case 'T': {
                                        state = 2;
                                        break;
                                    }
                                    case 'U': {
                                        state = 2;
                                        break;
                                    }
                                    case 'V': {
                                        state = 2;
                                        break;
                                    }
                                    case 'W': {
                                        state = 2;
                                        break;
                                    }
                                    case 'X': {
                                        state = 2;
                                        break;
                                    }
                                    case 'Y': {
                                        state = 2;
                                        break;
                                    }
                                    case 'Z': {
                                        state = 2;
                                        break;
                                    }
                                    case '_': {
                                        state = 2;
                                        break;
                                    }
                                    case 'a': {
                                        state = 2;
                                        break;
                                    }
                                    case 'b': {
                                        state = 2;
                                        break;
                                    }
                                    case 'c': {
                                        state = 2;
                                        break;
                                    }
                                    case 'd': {
                                        state = 2;
                                        break;
                                    }
                                    case 'e': {
                                        state = 2;
                                        break;
                                    }
                                    case 'f': {
                                        state = 2;
                                        break;
                                    }
                                    case 'g': {
                                        state = 2;
                                        break;
                                    }
                                    case 'h': {
                                        state = 2;
                                        break;
                                    }
                                    case 'i': {
                                        state = 2;
                                        break;
                                    }
                                    case 'j': {
                                        state = 2;
                                        break;
                                    }
                                    case 'k': {
                                        state = 2;
                                        break;
                                    }
                                    case 'l': {
                                        state = 2;
                                        break;
                                    }
                                    case 'm': {
                                        state = 2;
                                        break;
                                    }
                                    case 'n': {
                                        state = 2;
                                        break;
                                    }
                                    case 'o': {
                                        state = 2;
                                        break;
                                    }
                                    case 'p': {
                                        state = 2;
                                        break;
                                    }
                                    case 'q': {
                                        state = 2;
                                        break;
                                    }
                                    case 'r': {
                                        state = 2;
                                        break;
                                    }
                                    case 's': {
                                        state = 2;
                                        break;
                                    }
                                    case 't': {
                                        state = 2;
                                        break;
                                    }
                                    case 'u': {
                                        state = 2;
                                        break;
                                    }
                                    case 'v': {
                                        state = 2;
                                        break;
                                    }
                                    case 'w': {
                                        state = 2;
                                        break;
                                    }
                                    case 'x': {
                                        state = 2;
                                        break;
                                    }
                                    case 'y': {
                                        state = 2;
                                        break;
                                    }
                                    case 'z': {
                                        state = 2;
                                        break;
                                    }
                                }
                                break;
                            }

                            default: {
                                goto unmatched_0;
                            }
                        }
                        ++pos;
                        continue;
                        unmatched_0:
                        {
                            fprintf(output, "Error: Unmatched token '%c'.\n", ch);
                            while (pos < length && content[pos] != ' ' && content[pos] != '\n' &&
                                   content[pos] != '\t' && content[pos] != '\r' && content[pos] != '\v')
                                ++pos;
                        }
                    }

                    break;
                }
                case 1: {
                    int const end_len = 2, end[] = {2, 4};
                    int state = 1;
                    char const *token = "NUM";
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
                                    fprintf(output, "Error: Unexpected token '%c'.\n", ch);
                                    goto unmatched_1;
                                }
                            }
                        }
                        // switch between states.

                        switch (state) {
                            case 1: {
                                switch (ch) {
                                    case '0': {
                                        state = 2;
                                        break;
                                    }
                                    case '1': {
                                        state = 2;
                                        break;
                                    }
                                    case '2': {
                                        state = 2;
                                        break;
                                    }
                                    case '3': {
                                        state = 2;
                                        break;
                                    }
                                    case '4': {
                                        state = 2;
                                        break;
                                    }
                                    case '5': {
                                        state = 2;
                                        break;
                                    }
                                    case '6': {
                                        state = 2;
                                        break;
                                    }
                                    case '7': {
                                        state = 2;
                                        break;
                                    }
                                    case '8': {
                                        state = 2;
                                        break;
                                    }
                                    case '9': {
                                        state = 2;
                                        break;
                                    }
                                }
                                break;
                            }
                            case 2: {
                                switch (ch) {
                                    case '.': {
                                        state = 3;
                                        break;
                                    }
                                    case '0': {
                                        state = 2;
                                        break;
                                    }
                                    case '1': {
                                        state = 2;
                                        break;
                                    }
                                    case '2': {
                                        state = 2;
                                        break;
                                    }
                                    case '3': {
                                        state = 2;
                                        break;
                                    }
                                    case '4': {
                                        state = 2;
                                        break;
                                    }
                                    case '5': {
                                        state = 2;
                                        break;
                                    }
                                    case '6': {
                                        state = 2;
                                        break;
                                    }
                                    case '7': {
                                        state = 2;
                                        break;
                                    }
                                    case '8': {
                                        state = 2;
                                        break;
                                    }
                                    case '9': {
                                        state = 2;
                                        break;
                                    }
                                }
                                break;
                            }
                            case 3: {
                                switch (ch) {
                                    case '0': {
                                        state = 4;
                                        break;
                                    }
                                    case '1': {
                                        state = 4;
                                        break;
                                    }
                                    case '2': {
                                        state = 4;
                                        break;
                                    }
                                    case '3': {
                                        state = 4;
                                        break;
                                    }
                                    case '4': {
                                        state = 4;
                                        break;
                                    }
                                    case '5': {
                                        state = 4;
                                        break;
                                    }
                                    case '6': {
                                        state = 4;
                                        break;
                                    }
                                    case '7': {
                                        state = 4;
                                        break;
                                    }
                                    case '8': {
                                        state = 4;
                                        break;
                                    }
                                    case '9': {
                                        state = 4;
                                        break;
                                    }
                                }
                                break;
                            }
                            case 4: {
                                switch (ch) {
                                    case '0': {
                                        state = 4;
                                        break;
                                    }
                                    case '1': {
                                        state = 4;
                                        break;
                                    }
                                    case '2': {
                                        state = 4;
                                        break;
                                    }
                                    case '3': {
                                        state = 4;
                                        break;
                                    }
                                    case '4': {
                                        state = 4;
                                        break;
                                    }
                                    case '5': {
                                        state = 4;
                                        break;
                                    }
                                    case '6': {
                                        state = 4;
                                        break;
                                    }
                                    case '7': {
                                        state = 4;
                                        break;
                                    }
                                    case '8': {
                                        state = 4;
                                        break;
                                    }
                                    case '9': {
                                        state = 4;
                                        break;
                                    }
                                }
                                break;
                            }

                            default: {
                                goto unmatched_1;
                            }
                        }
                        ++pos;
                        continue;
                        unmatched_1:
                        {
                            fprintf(output, "Error: Unmatched token '%c'.\n", ch);
                            while (pos < length && content[pos] != ' ' && content[pos] != '\n' &&
                                   content[pos] != '\t' && content[pos] != '\r' && content[pos] != '\v')
                                ++pos;
                        }
                    }

                    break;
                }
            }
        }
        next:
        {
            ++pos;
        }
    }
    fclose(output);
    return 0;
    eof:
    {
        fprintf(output, "Error: Unexpected EOF.\n");
        fclose(output);
        return 0;
    }
}

