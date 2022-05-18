#ifndef SEMANTIC_ANALYSIS_H_INCLUDED
#define SEMANTIC_ANALYSIS_H_INCLUDED

#include "lexical_analysis.h"
#include "symbol_table.h"
#include <vector>

class SemanticAnalyzer
{
    std::vector<Token> tokenList;
    //int startTk = 0;
    int curTk = 0;
    int lastTk = 0;

    //rules
    int consume(int code);
    int ruleUnit();
    int ruleDeclStruct();
    int ruleDeclVar();
    Type ruleTypeBase();
    Type ruleArrayDecl();
    Type ruleTypeName();
    int ruleDeclFunc();
    int ruleFuncArg();
    int ruleStm();
    int ruleStmCompound();
    int ruleIf();
    int ruleWhile();
    int ruleFor();
    int ruleBreak();
    int ruleReturn();
    int ruleExpr();
    int ruleExprAssign();
    int ruleExprOr();
    int ruleExprOr1();
    int ruleExprAnd();
    int ruleExprAnd1();
    int ruleExprEq();
    int ruleExprEq1();
    int ruleExprRel();
    int ruleExprRel1();
    int ruleExprAdd();
    int ruleExprAdd1();
    int ruleExprMul();
    int ruleExprMul1();
    int ruleExprCast();
    int ruleExprUnary();
    int ruleExprPostfix();
    int ruleExprPostfix1();
    int ruleExprPrimary();
    int ruleExprPrimaryInner1();
    int ruleExprPrimaryInner2();
    int addVar(const std::string&, Type);

    //for symbol table
    Symbols symbols;// tabela de simboluri
    int crtDepth;// "adancimea" contextului curent, initial 0
    Symbol* crtFunc;// (pointer la simbolul functiei daca in functie, altfel NULL)
    Symbol* crtStruct;// (pointer la simbolul structurii daca in structura, altfel NULL)
    RetVal rv;
    RetVal rve;

public:
    SemanticAnalyzer(std::vector<Token> t);
    int run();

};
#endif // SEMANTIC_ANALYSIS_H_INCLUDED
