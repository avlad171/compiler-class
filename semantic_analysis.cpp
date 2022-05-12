#include "semantic_analysis.h"
#include <iostream>

using namespace std;

SemanticAnalyzer::SemanticAnalyzer(vector<Token> t)
{
    this->tokenList = t;
    //this->startTk = 0;
    this->curTk = 0;
    this->crtDepth = 0;
    this->crtStruct = nullptr;  //we are not in a struct in the beginning
    this->crtFunc = nullptr;    //we are not in a func in the beginning
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

/*int SemanticAnalyzer::expr()
{
    int startTk = curTk;
    if(consume(CT_INT))
    {
        return 1;
    }

    curTk = startTk;
    return 0;
}*/

int SemanticAnalyzer::addVar(const string& name, TypeBase type)
{
    Symbol *s;
    if(crtStruct)
    {
        if(findSymbol(crtStruct->args, name))
        {
            cout << "symbol redefinition: " << name << "\n";
            exit(0);
        }
        s = addSymbol(crtStruct->args, name, CLS_VAR);
    }
    else if(crtFunc)
    {
        s = findSymbol(symbols, name);
        if(s && s->depth == crtDepth)
            cout<<"symbol redefinition: "<<name;
        s = addSymbol(symbols, name, CLS_VAR);
        s->mem = MEM_LOCAL;
    }
    else
    {
        if(findSymbol(symbols, name))
            cout<<"symbol redefinition: "<<name;
        s = addSymbol(symbols, name, CLS_VAR);
        s->mem = MEM_GLOBAL;
    }
    s->type = type.type;
    s->struct_type = type.struct_type;
    s->nElements = type.nElements;

    return 1;
}

int SemanticAnalyzer::ruleDeclStruct()
{
    cout<<"DeclStruct\n";
	if(!consume(STRUCT))
		return 0;

	if(!consume(ID))
	{
        cout<<"Missing ID after struct!\n";
		exit(0);
	}
    string struct_name = tokenList[curTk-1].text;
    cout<<"Found struct "<<struct_name<<"\n";

    if(findSymbol(symbols, struct_name))
    {
        cout<<"Error: symbol redefinition ("<<struct_name<<")\n";
        exit(0);
    }
    crtStruct = addSymbol(symbols, struct_name, CLS_STRUCT, crtDepth);

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
    cout<<"Symbol table at the end of struct "<<struct_name<<": ";
    printSymbolTable(symbols);
    cout<<"Internal struct symbol table: ";
    printSymbolTable(crtStruct->args);
    crtStruct = nullptr;
	return 1;
}

int SemanticAnalyzer::ruleDeclVar()
{
    cout<<"Declvar!\n";
    int startTk = curTk;
    TypeBase vartype = ruleTypeBase();
	if(vartype.type == TB_NONE)
		return 0;

    string varname;
	if(!consume(ID))
	{
	    cout<<"Missing ID after type base!\n";
		exit(0);
	}
    varname = tokenList[curTk-1].text;
	vartype.nElements = ruleArrayDecl();
    addVar(varname, vartype);

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
        varname = tokenList[curTk-1].text;
		vartype.nElements = ruleArrayDecl();
        addVar(varname, vartype);
	}

	if(!consume(SEMICOLON))
	{
	    curTk = startTk;
	    return 0;
	}

	cout<<"Found variable at "<<curTk<<"\n";
	return 1;
}

TypeBase SemanticAnalyzer::ruleTypeBase()
{
    TypeBase ret;

    if(consume(INT))
    {
        ret.type = TB_INT;
        return ret;
    }
    else if (consume(DOUBLE))
    {
        ret.type = TB_DOUBLE;
        return ret;
    }
    else if(consume(CHAR))
    {
        ret.type = TB_CHAR;
        return ret;
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
            string struct_name = tokenList[curTk-1].text;
            Symbol * s = findSymbol(symbols, struct_name);
            if(s == nullptr)
            {
                cout<<"undefined struct: "<<struct_name;
                exit(0);
            }

            if(s->cls != CLS_STRUCT)
            {
                cout<<struct_name<<"is not a struct";
                exit(0);
            }
            ret.type = TB_STRUCT;
            ret.struct_type = s;
            return ret;
        }
    }

	return ret;
}

int SemanticAnalyzer::ruleArrayDecl()
{
	if(!consume(LBRACKET))
    {
        cout<<"No array at "<<curTk<<"\n";
        return -1;
    }

	ruleExpr();

	if(!consume(RBRACKET))
    {
        cout<<"Missing right bracket after array declaration!\n";
        exit(0);
    }

    cout<<"ArrayDecl!\n";
	return 0;  //no arrays ATM
}

TypeBase SemanticAnalyzer::ruleTypeName()
{
    TypeBase ret;
	int startTk = curTk;
    ret = ruleTypeBase();
	if(ret.type == TB_NONE)
    {
		curTk = startTk;
        return ret;
    }

	ret.nElements = ruleArrayDecl();
	return ret;
}

int SemanticAnalyzer::ruleDeclFunc()
{
    cout<<"DeclFunc!\n";
    TypeBase ret;
	int startTk = curTk;
    ret = ruleTypeBase();
    cout<<ret.type<<"\n";
	if(ret.type != TB_NONE)
	{
	    cout<<"Function returning non-void!\n";
		if(consume(MUL))
        {
            ret.nElements = 0;
        }
        else
        {
            ret.nElements = -1;
        }
	}

	else if(consume(VOID))
	{
        cout<<"Void function\n";
        ret.type = TB_VOID;
	}

	else
	{
        cout<<"func failed!\n";
		curTk = startTk;
        return 0;
    }

	if(!consume(ID))
	{
		curTk = startTk;
        return 0;
    }
    string func_name = tokenList[curTk-1].text;
    cout<<func_name<<"\n";

	if(!consume(LPAR))
	{
		curTk = startTk;
        return 0;
    }

    if(findSymbol(symbols, func_name))
    {
        cout << "function name redefinition: " << func_name;
        exit(0);
    }

    crtFunc = addSymbol(symbols, func_name, CLS_FUNC);
    crtFunc->type=ret.type;
    crtFunc->nElements = ret.nElements;
    crtFunc->struct_type = ret.struct_type;
    crtDepth++;
    cout<<"Increasing depth: "<<crtDepth<<"\n";

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
    crtDepth--;
    cout<<"Decreasing depth (func): "<<crtDepth<<"\n";
	if(!ruleStmCompound())
	{
		curTk = startTk;
        return 0;
    }
    cout<<"Symbol table at the end of function "<<func_name<<": ";
    printSymbolTable(symbols);
    cout<<"Internal function symbol table: ";
    printSymbolTable(crtFunc->args);

    deleteSymbolsAfter(symbols, crtFunc);
    crtFunc = nullptr;

	return 1;
}

int SemanticAnalyzer::ruleFuncArg()
{
    TypeBase ret;
	int startTk = curTk;
    ret = ruleTypeBase();
	if(ret.type == TB_NONE)
	{
		curTk = startTk;
        return 0;
    }

	if(!consume(ID))
	{
		cout<<"Missing identifier of function argument!\n";
		exit(0);
    }
    string arg_name = tokenList[curTk-1].text;
	ret.nElements = ruleArrayDecl();
    Symbol * s = addSymbol(symbols, arg_name, CLS_VAR);
    s->mem = MEM_ARG;
    s->type = ret.type;
    s->struct_type = ret.struct_type;
    s->nElements = ret.nElements;
    s = addSymbol(crtFunc->args, arg_name, CLS_VAR);
    s->mem = MEM_ARG;
    s->type = ret.type;
    s->struct_type = ret.struct_type;
    s->nElements = ret.nElements;
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
    Symbol * start = nullptr;
    for (auto it = symbols.begin(); it != symbols.end(); ++it)
        start = *it;

    cout<<"Found last symbol in table!\n";

	if(!consume(LACC))
	{
	    cout<<"Statement compound failed!\n";
        return 0;
    }
    crtDepth++;
    cout<<"Increasing depth: "<<crtDepth<<"\n";

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
    crtDepth--;
    cout<<"Decreasing depth (stm compound): "<<crtDepth<<"\n";
    cout<<"Deleting symbols after "<<start->name<<"\n";
    deleteSymbolsAfter(symbols, start);
    cout<<"Symbols deleted\n";
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

    if(ruleTypeName().type == TB_NONE)
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

            cout<<"Final symbol table: ";
            printSymbolTable(symbols);
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
