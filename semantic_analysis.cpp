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

int SemanticAnalyzer::ruleDeclStruct()
{
	int startTk = curTk;
	if(!consume(STRUCT))
		return 0;
	
	if(!consume(ID))
	{
		curTk = startTk;
		return 0;
	}
	
	if(!consume(LACC))
	{
		curTk = startTk;
		return 0;
	}
	
	while(1)
	{
		if(!ruleDeclVar())
			break;
	}
	
	if(!consume(RACC))
	{
		curTk = startTk;
		return 0;
	}
	
	if(!consume(SEMICOLON))
	{
		curTk = startTk;
		return 0;
	}
	return 1;
}

int SemanticAnalyzer::ruleDeclVar()
{
	int startTk = curTk;
	if(!ruleTypeBase())
	{
		curTk = startTk;
		return 0;
	}
	
	if(!consume(ID))
	{
		curTk = startTk;
		return 0;
	}
	
	ruleArrayDecl();
	
	while (1)
	{
		int startTk2 = curTk;
		if(!consume(COMMA))
		{
			curTk = startTk2;
			break;
		}
		
		if(!consume(ID))
		{
			curTk = startTk2;
			break;
		}
		
		ruleArrayDecl();
	}
	
	if(!consume(SEMICOLON))
	{
		curTk = startTk;
		return 0;
	}
	return 1;
}

int SemanticAnalyzer::ruleTypeBase()
{
	int startTk = curTk;
	if(!(consume(INT) || consume(DOUBLE) || consume(CHAR) || consume(STRUCT)))
	{
		curTk = startTk;
		return 0;
	}
	
	if(!consume(ID))
	{
		curTk = startTk;
		return 0;
	}
	return 1;
}

int SemanticAnalyzer::ruleArrayDecl()
{
	int startTk = curTk;
	if(!consume(LBRACKET))
    {
		curTk = startTk;
        return 0;
    }

	expr();
	
	if(!consume(RBRACKET))
    {
		curTk = startTk;
        return 0;
    }
    
	return 1;
}

int SemanticAnalyzer::ruleTypeName()
{
	int startTk = curTk;
	if(!ruleTypeBase())
    {
		curTk = startTk;
        return 0;
    }
	
	ruleArrayDecl();
	return 1;
}

int SemanticAnalyzer::ruleDeclFunc()
{
	int startTk = curTk;
	if(ruleTypeBase())
	{
		consume(MUL);
	}
	
	else if(consume(VOID))
	{
		
	}
	
	else
	{
		curTk = startTk;
        return 0;
    }
	
	if(!consume(ID))
	{
		curTk = startTk;
        return 0;
    }
	
	if(!consume(LPAR))
	{
		curTk = startTk;
        return 0;
    }
	
	//now function arguments
	ruleFuncArg();
	while(1)
	{
		if(!(consume(COMMA) && ruleFuncArg()))
			break;
	}
	
	if(!consume(RPAR))
	{
		curTk = startTk;
        return 0;
    }
	
	if(!ruleStmCompound)
	{
		curTk = startTk;
        return 0;
    }
	
	return 1;
}

int SemanticAnalyzer::ruleFuncArg()
{
	int startTk = curTk;
	if(!ruleTypeBase())
	{
		curTk = startTk;
        return 0;
    }
	
	if(!consume(ID))
	{
		curTk = startTk;
        return 0;
    }
	
	ruleArrayDecl();
	
	return 1;
}

int SemanticAnalyzer::ruleStm()
{
	int startTk = curTk;
	if(ruleStmCompound() || ruleIf() || ruleWhile() || ruleFor() || ruleBreak() || ruleReturn() || (ruleExpr() && consume(SEMICOLON)))
		return 1;
	else
	{
		curTk = startTk;
        return 0;
    }
}

int SemanticAnalyzer::ruleStmCompound()
{
	int startTk = curTk;
	if(!consume(LACC))
	{
		curTk = startTk;
        return 0;
    }
	
	while(1)
	{
		if(ruleDeclVar())
			continue;
		else if (ruleStm())
			continue;
		else
			break;
	}
	
	if(!consume(RACC))
	{
		curTk = startTk;
        return 0;
    }
}

int SemanticAnalyzer::ruleIf()
{
	if(!consume(IF))
		return 0;
	
	if(!consume(LPAR))
	{
		cout<<"missing ( after if\n";
        exit(0);
    }
	
	if(!expr())
	{
		cout<<"invalid expression after (\n";
        exit(0);
    }
	
	if(!consume(RPAR))
	{
		cout<<"missing ) after if\n";
        exit(0);
    }
	
	if(!ruleStm())
	{
		cout<<"missing if statement\n";
        exit(0);
    }
	
	//see whether we have an else
	if(consume(ELSE))
		if(!ruleStm())
		{
			cout<<"missing else statement\n";
			exit(0);
		}
	
	return 1;
}

int SemanticAnalyzer::ruleWhile()
{
    int startTk = curTk;
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
    if(!ruleStm())
    {
        cout<<"missing while statement\n";
        exit(0);
    }

    return 1;
}

int SemanticAnalyzer::ruleFor()
{
    int startTk = curTk;
    if(!consume(FOR))
        return 0;
    if(!consume(LPAR))
    {
        cout<<"missing ( after for\n";
        exit(0);
    }
	
    expr();
	if(!consume(SEMICOLON))
	{
		cout<<"missing ; after first expression in for\n";
        exit(0);
	}
	
	expr();
	if(!consume(SEMICOLON))
	{
		cout<<"missing ; after second expression in for\n";
        exit(0);
	}
	
	expr();
    if(!consume(RPAR))
    {
        cout<<"missing ) in for\n";
        exit(0);
    }
    if(!stm())
    {
        cout<<"missing for statement\n";
        exit(0);
    }

    return 1;
}

int SemanticAnalyzer::ruleBreak()
{
	if(!consume(BREAK))
		return 0;
	
	if(!consume(SEMICOLON))
	{
        cout<<"missing ; after break\n";
        exit(0);
    }
	
	return 1;
}

int SemanticAnalyzer::ruleReturn()
{
	if(!consume(RETURN))
		return 0;
	
	expr();
	
	if(!consume(SEMICOLON))
	{
        cout<<"missing ; after return\n";
        exit(0);
    }
}

int SemanticAnalyzer::ruleExpr()
{
	return exprAssign();
}

int SemanticAnalyzer::ruleExprAssign()
{
	if(ruleExprOr())
		return 1;
	
	if(!ruleExprUnary())
		return 0;
	
	if(!consume(ASSIGN))
	{
        cout<<"missing = in assignment\n";
        exit(0);
    }
	
	if(ruleExprAssign())
		return 1;
	
	else
	{
        cout<<"missing expression after assignment\n";
        exit(0);
    }
}

int SemanticAnalyzer::ruleExprOr()
{
	if(ruleExprAnd())
		return 1;
	
	
	if(exprOr())
		return 1;
	
	if(!consume(OR))
	{
        cout<<"missing OR in expression\n";
        exit(0);
    }
	
	if(ruleExprOr())
		return 1;
	else
	{
		cout<<"missing expression after or\n";
        exit(0);
	}
}

int SemanticAnalyzer::ruleExprAnd()
{
	if(ruleExprEq())
		return 1;
	
	
}

int SemanticAnalyzer::ruleUnit()
{
	bool ok = true;
	while(ok)
	{
		ok = false;
		ok |= ruleDeclStruct();
		ok |= ruleDeclFunc();
		ok |= ruleDeclVar();
		
		if(consume(END))
			return 1;
	}
	return 0;
}

int SemanticAnalyzer::run()
{

	if(!ruleUnit())
		return 0;
    return 1;
}
