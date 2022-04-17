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
    int startTk = curTk;
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
			cout<<"Missing identifier after comma in variable declaration!\n";
			exit(0);
		}

		ruleArrayDecl();
	}

	if(!consume(SEMICOLON))
	{
	    curTk = startTk;
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

	ruleExpr();

	if(!consume(RBRACKET))
    {
        cout<<"Missing right bracket after array declaration!\n";
        exit(0);
    }

    cout<<"ArrayDecl!\n";
	return 1;
}

int SemanticAnalyzer::ruleTypeName()
{
	if(!ruleTypeBase())
    {
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
	if(!ruleTypeBase())
	{
        return 0;
    }

	if(!consume(ID))
	{
		cout<<"Missing identifier of function argument!\n";
		exit(0);
    }

	ruleArrayDecl();

	return 1;
}

int SemanticAnalyzer::ruleStm()
{
    cout<<"Statement at "<<curTk<<"\n";
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
    cout<<"Matching statement compound at "<<curTk<<"\n";
	if(!consume(LACC))
	{
	    cout<<"Statement compound failed!\n";
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
    cout<<"Matching IF at "<<curTk<<"\n";
	if(!consume(IF))
    {
        cout<<"IF failed!\n";
        return 0;
    }

    cout<<"if1\n";
	if(!consume(LPAR))
	{
		cout<<"missing ( after if\n";
        exit(0);
    }

    cout<<"if2 at "<<curTk<<"\n";
	if(!ruleExpr())
	{
		cout<<"invalid expression after if(\n";
        exit(0);
    }

    cout<<"if3 at "<<curTk<<"\n";
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
    cout<<"Matching WHILE at "<<curTk<<"\n";
	if(!consume(WHILE))
    {
        cout<<"WHILE failed!\n";
        return 0;
    }

    if(!consume(LPAR))
    {
        cout<<"missing ( after while\n";
        exit(0);
    }

    if(!ruleExpr())
    {
        cout<<"invalid expression after while(\n";
        exit(0);
    }

    if(!consume(RPAR))
    {
        cout<<"missing ) after while\n";
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
    cout<<"Matching FOR at "<<curTk<<"\n";
	if(!consume(FOR))
    {
        cout<<"FOR failed!\n";
        return 0;
    }

    if(!consume(LPAR))
    {
        cout<<"missing ( after for\n";
        exit(0);
    }

    ruleExpr();
	if(!consume(SEMICOLON))
	{
		cout<<"missing ; after first expression in for\n";
        exit(0);
	}

	ruleExpr();
	if(!consume(SEMICOLON))
	{
		cout<<"missing ; after second expression in for\n";
        exit(0);
	}

	ruleExpr();
    if(!consume(RPAR))
    {
        cout<<"missing ) after for\n";
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
	cout<<"Matching BREAK at "<<curTk<<"\n";
	if(!consume(BREAK))
    {
        cout<<"BERAK failed!\n";
        return 0;
    }

	if(!consume(SEMICOLON))
	{
        cout<<"missing ; after break\n";
        exit(0);
    }

	return 1;
}

int SemanticAnalyzer::ruleReturn()
{
	cout<<"Matching RETURN at "<<curTk<<"\n";
	if(!consume(RETURN))
    {
        cout<<"RETURN failed!\n";
        return 0;
    }

	ruleExpr();

	if(!consume(SEMICOLON))
	{
        cout<<"missing ; after return\n";
        exit(0);
    }

    return 1;
}

int SemanticAnalyzer::ruleExpr()
{
	return ruleExprAssign();
}

int SemanticAnalyzer::ruleExprAssign()
{
    cout<<"Expr assign\n";
    int startTk = curTk;
    if(ruleExprUnary())
    {
        if(!consume(ASSIGN))
        {
            curTk = startTk;
        }

        else
        {
            if(!ruleExprAssign())
            {
                cout<<"Missing expression after =\n";
                exit(0);
            }

            return 1;
        }
    }

	if(ruleExprOr())
		return 1;

    else
        return 0;
}

int SemanticAnalyzer::ruleExprOr()
{
    cout<<"Expr or\n";
    if(!ruleExprAnd())
        return 0;

    ruleExprOr1();
	return 1;
}

int SemanticAnalyzer::ruleExprOr1()
{
    if(consume(OR))
    {
        if(!ruleExprAnd())
        {
            cout<<"Missing exception after OR\n";
            exit(0);
        }

        if(!ruleExprOr1())
        {
            cout<<"Missing exception after OR\n";
            exit(0);
        }
    }

    return 1;
}

int SemanticAnalyzer::ruleExprAnd()
{
    cout<<"Expr and\n";
    if(!ruleExprEq())
        return 0;

    ruleExprAnd1();
    return 1;
}

int SemanticAnalyzer::ruleExprAnd1()
{
    if(consume(AND))
    {
        if(!ruleExprEq())
        {
            cout<<"Missing expression after AND\n";
            exit(0);
        }

        if(!ruleExprAnd1())
        {
            cout<<"Missing expression after AND\n";
            exit(0);
        }
    }

    return 1;
}

int SemanticAnalyzer::ruleExprEq()
{
    cout<<"Expr eq\n";
    if(!ruleExprRel())
        return 0;

    ruleExprEq1();
    return 1;
}

int SemanticAnalyzer::ruleExprEq1()
{
    if(consume(EQUAL) || consume(NOTEQ))
    {
        if(!ruleExprRel())
        {
            cout<<"Missing expression after ==\n";
            exit(0);
        }
        if(!ruleExprEq1())
        {
            cout<<"Missing expression after ==\n";
            exit(0);
        }
    }

    return 1;
}

int SemanticAnalyzer::ruleExprRel()
{
    cout<<"Expr rel\n";
    if(!ruleExprAdd())
        return 0;

    ruleExprRel1();
	return 1;
}

int SemanticAnalyzer::ruleExprRel1()
{
    if(consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
    {
        if(!ruleExprAdd())
        {
            cout<<"Missing expression after comparison\n";
            exit(0);
        }

        if(!ruleExprRel1())
        {
            cout<<"Missing expression after comparison\n";
            exit(0);
        }
    }

    return 1;
}

int SemanticAnalyzer::ruleExprAdd()
{
    cout<<"Expr add\n";
    if(!ruleExprMul())
        return 0;

    ruleExprAdd1();
    return 1;
}

int SemanticAnalyzer::ruleExprAdd1()
{
	if(consume(ADD) || consume(SUB))
    {
        if(!ruleExprMul())
        {
            cout<<"Missing expression after addition\n";
            exit(0);
        }

        if(!ruleExprAdd1())
        {
            cout<<"Missing expression after addition\n";
            exit(0);
        }
    }

    return 1;
}

int SemanticAnalyzer::ruleExprMul()
{
    cout<<"Expr mul\n";
    if(!ruleExprCast())
        return 0;

    ruleExprMul1();
    return 1;
}

int SemanticAnalyzer::ruleExprMul1()
{
    if(consume(MUL) || consume(DIV))
    {
        if(!ruleExprCast())
        {
            cout<<"Missing expression after addition\n";
            exit(0);
        }

        if(!ruleExprMul1())
        {
            cout<<"Missing expression after addition\n";
            exit(0);
        }
    }

    return 1;
}
int SemanticAnalyzer::ruleExprCast()
{
    cout<<"Expr cast\n";
    if(ruleExprUnary())
        return 1;

    if(!consume(LPAR))
        return 0;

    if(!ruleTypeName())
    {
        cout<<"Missing type in cast\n";
        exit(0);
    }

    if(!consume(RPAR))
    {
        cout<<"Missing ) after cast\n";
        exit(0);
    }

    if(!ruleExprCast())
    {
        cout<<"Missing expression after cast\n";
        exit(0);
    }

    return 1;
}

int SemanticAnalyzer::ruleExprUnary()
{
    cout<<"Expr unary\n";
    if(ruleExprPostfix())
        return 1;

    if(!consume(SUB) || !consume(NOT))
        return 0;

    if(!ruleExprUnary())
    {
        cout<<"Missing unary expression!\n";
        exit(0);
    }

    return 1;
}

int SemanticAnalyzer::ruleExprPostfix()
{
    cout<<"Expr postfix\n";
    if(!ruleExprPrimary())
        return 0;

    ruleExprPostfix1();
    return 1;
}

int SemanticAnalyzer::ruleExprPostfix1()
{
    if(consume(LBRACKET))
    {
        if(!ruleExpr())
        {
            cout<<"Missing expression after {\n";
            exit(0);
        }

        if(!consume(RBRACKET))
        {
            cout<<"Missing } after expression\n";
            exit(0);
        }

        ruleExprPostfix1();
        return 1;
    }

    if(consume(DOT))
    {
        if(!consume(ID))
        {
            cout<<"Missing identifier after .\n";
            exit(0);
        }

        ruleExprPostfix1();
        return 1;
    }

    return 1;
}

int SemanticAnalyzer::ruleExprPrimary()
{
    cout<<"Expr primary\n";
    if(consume(CT_INT) || consume(CT_REAL) || consume(CT_CHAR) || consume(CT_STRING))
        return 1;

    if(consume(LPAR))
    {
        if(!ruleExpr())
        {
            cout<<"Missing expression after (\n";
            exit(0);
        }

        if(!consume(RPAR))
        {
            cout<<"Missing ) after expression\n";
            exit(0);
        }

        return 1;
    }

    if(consume(ID))
    {
        ruleExprPrimaryInner1();
        cout<<"found id\n";
        return 1;
    }

    return 0;
}

int SemanticAnalyzer::ruleExprPrimaryInner1()
{
    cout<<"Expr primary1\n";
    if(!consume(LPAR))
        return 0;

    ruleExprPrimaryInner2();

    if(!consume(RPAR))
    {
        cout<<"Missing ) after expression\n";
        exit(0);
    }

    return 1;
}

int SemanticAnalyzer::ruleExprPrimaryInner2()
{
    if(!ruleExpr())
        return 0;

    while (1)
    {
        if(!consume(COMMA))
            break;

        if(!ruleExpr())
        {
            cout<<"Missing expression after comma\n";
            exit(0);
        }
    }

    return 1;
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
        {
            cout<<"Found END at token "<<curTk<<"\n";
			return 1;
        }
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
