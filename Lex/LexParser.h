//
// Created by Kasugano_Sora on 2023/5/22.
//

#ifndef COMPILATIONPRINCIPLE_LEXPARSER_H
#define COMPILATIONPRINCIPLE_LEXPARSER_H

#include <string>
#include <set>

class LexParser {
private:
    static std::string const normal;

    static std::string const node;

    std::string filename;

    std::string content;

    std::set<std::string> node_name;
public:
    explicit LexParser(std::string path);
};


#endif //COMPILATIONPRINCIPLE_LEXPARSER_H
