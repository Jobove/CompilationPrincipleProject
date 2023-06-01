#include <malloc.h>
#include <stdio.h>
#include <string.h>

%
s
const char *keywords[] = {
        %s};

%
s
const char *operators[] = {
        %s};

const char *token_operators[] = {
        %s};

%
s
const char *brackets[] = {
        %s};

const char *token_brackets[] = {
        %s};

const char *comments[] = {
        %s};

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
    fprintf(output, "%%s\n", token);
    fflush(output);
}

void output_comment(FILE *output, const char *content, int l, int r) {
    fprintf(output, "//");
    for (int i = l; i <= r; ++i) {
        fprintf(output, "%%c", content[i]);
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

    int total_dfa = %d, pos = 0, l;
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
                %s
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