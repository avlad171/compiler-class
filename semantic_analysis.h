#ifndef SEMANTIC_ANALYSIS_H_INCLUDED
#define SEMANTIC_ANALYSIS_H_INCLUDED

#include "lexical_analysis.h"
#include <vector>

class SemanticAnalyzer
{
    std::vector<Token> tokenList;
    //int startTk = 0;
    int curTk = 0;

    //rules
    int consume(int code);
    int ruleWhile();
    int expr();
    int stm();

public:
    SemanticAnalyzer(std::vector<Token> t);
    int run();
};
#endif // SEMANTIC_ANALYSIS_H_INCLUDED
