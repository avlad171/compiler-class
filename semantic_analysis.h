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
    int ruleUnit();
    int ruleDeclStruct();
    int ruleDeclVar();
    int ruleTypeBase();
    int ruleArrayDecl();
    int ruleTypeName();
    int ruleDeclFunc();
    int ruleFuncArg();
    int ruleStm();
    int ruleStmCompound();
    int ruleIf();
    int ruleFor();
    int ruleBreak();
    int ruleReturn();
    int ruleExpr();
    int ruleExprAssign();
    int ruleExprOr();
    int ruleExprAnd();
    int ruleExprEq();
    int ruleExprRel();
    int ruleExprAdd();
    int ruleExprMul();
    int ruleExprCast();
};
#endif // SEMANTIC_ANALYSIS_H_INCLUDED
