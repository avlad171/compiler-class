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
	if(!consume(STRUCT))
		return 0;
    //cout<<"STRUCT!\n";
	if(!consume(ID))
	{
        cout<<"Missing ID after struct!\n";
		exit(0);
	}

	if(!consume(LACC))
	{
	    cout<<"Missing { after struct!\n";
		exit(0);
	}

	while(1)
	{
		if(!ruleDeclVar())
			break;
	}

	if(!consume(RACC))
	{
	    cout<<"Missing } after struct!\n";
		exit(0);
	}

	if(!consume(SEMICOLON))
	{
	    cout<<"Missing ; after struct!\n";
		exit(0);
	}

	cout<<"Found struct at token "<<curTk<<"\n";
	return 1;
}

int SemanticAnalyzer::ruleDeclVar()
{
    //cout<<"Declvar!\n";
	if(!ruleTypeBase())
		return 0;


	if(!consume(ID))
	{
	    cout<<"Missing ID after type base!\n";
		exit(0);
	}

	ruleArrayDecl();

	while (1)
	{
		if(!consume(COMMA))
		{
			break;
		}

		if(!consume(ID))
		{
			break;
		}

		ruleArrayDecl();
	}

	if(!consume(SEMICOLON))
	{
		return 0;
	}
	cout<<"Found variable at "<<curTk<<"\n";
	return 1;
}

int SemanticAnalyzer::ruleTypeBase()
{
	if(consume(INT) || consume(DOUBLE) || consume(CHAR))
    {
        cout<<"Declvar!\n";
        return 1;
    }

    else if (consume(STRUCT))
    {
        if(!consume(ID))
        {
            cout<<"Missing ID after struct!\n";
            exit(0);
        }
        else
        {
            cout<<"Decl struct\n";
            return 1;
        }
    }

	return 0;
}

int SemanticAnalyzer::ruleArrayDecl()
{
	if(!consume(LBRACKET))
    {
        cout<<"No array at "<<curTk<<"\n";
        return 0;
    }

	expr();

	if(!consume(RBRACKET))
    {
        return 0;
    }

    cout<<"ArrayDecl!\n";
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
    cout<<"DeclFunc!\n";
	int startTk = curTk;
	if(ruleTypeBase())
	{
	    cout<<"Function returning non-void!\n";
		consume(MUL);
	}

	else if(consume(VOID))
	{
        cout<<"Void \n";
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

	if(!ruleStmCompound())
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
    cout<<"Statement!\n";
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
    cout<<"STATEMENT COMPOUND!\n";
	if(!consume(LACC))
	{
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
		cout<<"No } after stm compound!\n";
		exit(0);
    }
    return 1;
}

int SemanticAnalyzer::ruleIf()
{
    cout<<"if0\n";
	if(!consume(IF))
		return 0;

    cout<<"if1\n";
	if(!consume(LPAR))
	{
		cout<<"missing ( after if\n";
        exit(0);
    }

    cout<<"if2\n";
	if(!ruleExpr())
	{
		cout<<"invalid expression after (\n";
        exit(0);
    }

    cout<<"if3\n";
	if(!consume(RPAR))
	{
		cout<<"missing ) after if\n";
        exit(0);
    }

    cout<<"if4\n";
	if(!ruleStm())
	{
		cout<<"missing if statement\n";
        exit(0);
    }

    cout<<"if5\n";
	//see whether we have an else
	if(consume(ELSE))
		if(!ruleStm())
		{
			cout<<"missing else statement\n";
			exit(0);
		}

    cout<<"if6\n";
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
    if(!ruleStm())
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
	return ruleExprAssign();
}

int SemanticAnalyzer::ruleExprAssign()
{
	if(ruleExprOr())
		return 1;

	//if(!ruleExprUnary())
	if(!ruleExprCast())
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
	ruleExprAnd();

    if(consume(OR))
    {
        if(!ruleExprOr())
        {
            cout<<"Missing expression after OR\n";
            exit(0);
        }
    }
	return 1;
}

int SemanticAnalyzer::ruleExprAnd()
{
	ruleExprEq();

	if(consume(AND))
    {
        if(!ruleExprAnd())
        {
            cout<<"Missing expression after AND\n";
            exit(0);
        }
    }

    return 1;
}

int SemanticAnalyzer::ruleExprEq()
{
    ruleExprRel();

	if(consume(EQUAL) || consume(NOTEQ))
    {
        if(!ruleExprEq())
        {
            cout<<"Missing expression after ==\n";
            exit(0);
        }
    }

    return 1;
}

int SemanticAnalyzer::ruleExprRel()
{
    ruleExprAdd();

	if(consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
    {
        if(!ruleExprRel())
        {
            cout<<"Missing expression after comparison\n";
            exit(0);
        }
    }

    return 1;
}

int SemanticAnalyzer::ruleExprAdd()
{
    ruleExprMul();

	if(consume(ADD) || consume(SUB))
    {
        if(!ruleExprAdd())
        {
            cout<<"Missing expression after addition\n";
            exit(0);
        }
    }

    return 1;
}

int SemanticAnalyzer::ruleExprMul()
{
    ruleExprCast();

	if(consume(MUL) || consume(DIV))
    {
        if(!ruleExprMul())
        {
            cout<<"Missing expression after addition\n";
            exit(0);
        }
    }

    return 1;
}

int SemanticAnalyzer::ruleExprCast()
{
     int startTk = curTk;
    if(consume(CT_INT))
    {
        return 1;
    }

    curTk = startTk;
    return 0;
    //TODO
}

int SemanticAnalyzer::ruleUnit()
{
	bool ok = true;
	while(ok)
	{
		ok = false;
		ok |= ruleDeclStruct();
		ok |= ruleDeclVar();
		ok |= ruleDeclFunc();


		if(consume(END))
			return 1;
	}
	return 0;
}

int SemanticAnalyzer::run()
{
    curTk = 0;
	if(!ruleUnit())
		return 0;
    return 1;
}
