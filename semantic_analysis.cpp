#include "semantic_analysis.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

SemanticAnalyzer::SemanticAnalyzer(vector<Token> t)
{
    this->tokenList = t;
    //this->startTk = 0;
    this->curTk = 0;
}

int SemanticAnalyzer::consume(int code)
{
    if(tokenList[curTk].code == code)
    {
        curTk++;
        return 1;
    }

    return 0;
}

int SemanticAnalyzer::ruleWhile()
{
    //int startTk = curTk;
    if(!consume(WHILE))
        return 0;
    if(!consume(LPAR))
    {
        cout<<"missing ( after while\n";
        exit(0);
    }
    if(!expr())
    {
        cout<<"invalid expression after (\n";
        exit(0);
    }
    if(!consume(RPAR))
    {
        cout<<"missing )\n";
        exit(0);
    }
    if(!stm())
    {
        cout<<"missing while statement\n";
        exit(0);
    }

    return 1;
}

int SemanticAnalyzer::expr()
{
    int startTk = curTk;
    if(consume(CT_INT))
    {
        return 1;
    }

    curTk = startTk;
    return 0;
}

int SemanticAnalyzer::stm()
{
    int startTk = curTk;
    if(!consume(RETURN))
        return 0;
    expr();
    if(consume(SEMICOLON))
        return 1;

    curTk = startTk;
    return 0;
}
int SemanticAnalyzer::run()
{
    for(int i = 0; i < tokenList.size(); ++i)
    {
        cout<<"[+] Current token: "<<curTk<<"\n";
        if(ruleWhile())
        {
            cout<<"Found while at token "<<curTk<<"\n";
        }

        else
        {
            curTk++;
        }
    }

    return 1;
}
