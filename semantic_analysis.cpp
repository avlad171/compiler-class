#include "semantic_analysis.h"
#include <iostream>
#include <set>

using namespace std;
int typeArgSize(Type & t)
{
    int size = 0;
    switch(t.typeBase)
    {
        case TB_INT:
            size = sizeof(int);
            break;

        case TB_DOUBLE:
            size = sizeof(double);
            break;

        case TB_CHAR:
            size = sizeof(char);
            break;

        case TB_STRUCT:
            cout<<"Sizeof struct not implemented!\n";
            break;

        case TB_VOID:
            size=0;
            break;

        default:
            cout<<"sizeof() undefined type: "<<t.typeBase<<"\n";
            exit(-1);
    }

    return size;
}

void getRVal(vector<Instr> & bytecode, RetVal & rv)
{
    if(rv.isLVal)
    {
        switch(rv.type.typeBase)
        {
            case TB_INT:
            case TB_DOUBLE:
            case TB_CHAR:
            case TB_STRUCT:
                addInstr(bytecode, Instr(PUSHCT_A, rv.addr));
                addInstr(bytecode, Instr(LOAD, typeArgSize(rv.type)));
                break;
            default:
                cout<<"getRVal unhandled type: "<<rv.type.typeBase<<"\n";
                exit(-1);
        }
    }

    else
        addInstr(bytecode, Instr(PUSHCT_I, rv.ctVal.i));
}

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
        lastTk = curTk;
        curTk++;
        return 1;
    }

    return 0;
}


int SemanticAnalyzer::addVar(const string& name, Type type)
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
    s->type = type.typeBase;
    s->struct_type = type.struct_type;
    s->nElements = type.nElements;

    cout<<"Mallocing for "<<name<<" space needed "<<typeArgSize(type)<<"\n";
    s->addr = (int64_t) malloc(typeArgSize(type));
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
    cout<<"DECLVAR at "<<tokenList[curTk].text<<"\n";
    int startTk = curTk;
    Type vartype = ruleTypeBase();
	if(vartype.typeBase == TB_NONE)
		return 0;

    string varname;
	if(!consume(ID))
	{
	    cout<<"Missing ID after typeBase base!\n";
		exit(0);
	}
    varname = tokenList[curTk-1].text;
	vartype.nElements = ruleArrayDecl().nElements;
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
		vartype.nElements = ruleArrayDecl().nElements;
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

Type SemanticAnalyzer::ruleTypeBase()
{
    Type ret;

    if(consume(INT))
    {
        ret.typeBase = TB_INT;
        return ret;
    }
    else if (consume(DOUBLE))
    {
        ret.typeBase = TB_DOUBLE;
        return ret;
    }
    else if(consume(CHAR))
    {
        ret.typeBase = TB_CHAR;
        return ret;
    }

    else if (consume(STRUCT))
    {
        cout<<"Struct declaration!\n";
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
            ret.typeBase = TB_STRUCT;
            ret.struct_type = s;
            return ret;
        }
    }

	return ret;
}

Type SemanticAnalyzer::ruleArrayDecl()
{
    cout<<"Array decl at "<<curTk<<": "<<tokenList[curTk].text<<"\n";
    Type ret;
    ret.nElements = 0;
	if(!consume(LBRACKET))
    {
        cout<<"No array at "<<curTk<<"\n";
        ret.nElements = -1;
        return ret;
    }

	ruleExpr();
    if (!rv.isCtVal)    //todo handle 20/4 + 5
    {
        cout<<"Array size is not constant!\n";
        exit(0);
    }
    if (!rv.type.typeBase == CT_INT)
    {
        cout<<"Array size is not an integer!\n";
        exit(0);
    }
    ret.nElements=rv.ctVal.i;

	if(!consume(RBRACKET))
    {
        cout<<"Missing right bracket after array declaration!\n";
        exit(0);
    }

    cout<<"ArrayDecl!\n";
	return ret;
}

Type SemanticAnalyzer::ruleTypeName()
{
    Type ret;
	int startTk = curTk;
    ret = ruleTypeBase();
	if(ret.typeBase == TB_NONE)
    {
		curTk = startTk;
        return ret;
    }

	ret.nElements = ruleArrayDecl().nElements;
	return ret;
}

int SemanticAnalyzer::ruleDeclFunc()
{
    cout<<"DeclFunc!\n";
    Type ret;
	int startTk = curTk;
    ret = ruleTypeBase();
    cout << ret.typeBase << "\n";
	if(ret.typeBase != TB_NONE)
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
        ret.typeBase = TB_VOID;
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
    crtFunc->type=ret.typeBase;
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
    crtFunc->addr = addInstr(bytecode, ENTER);
    cout<<"Function "<<crtFunc->name<<"is at instruction "<<crtFunc->addr<<"\n";

	if(!ruleStmCompound())
	{
		curTk = startTk;
        return 0;
    }

    if(ret.typeBase == TB_VOID)
    {
        addInstr(bytecode, Instr(RET, 0, 0));
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
    Type ret;
	int startTk = curTk;
    ret = ruleTypeBase();
	if(ret.typeBase == TB_NONE)
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
	ret.nElements = ruleArrayDecl().nElements;
    Symbol * s = addSymbol(symbols, arg_name, CLS_VAR);
    s->mem = MEM_ARG;
    s->type = ret.typeBase;
    s->struct_type = ret.struct_type;
    s->nElements = ret.nElements;
    s = addSymbol(crtFunc->args, arg_name, CLS_VAR);
    s->mem = MEM_ARG;
    s->type = ret.typeBase;
    s->struct_type = ret.struct_type;
    s->nElements = ret.nElements;
	return 1;
}

int SemanticAnalyzer::ruleStm()
{
    cout<<"STATEMENT at "<<tokenList[curTk].text<<"\n";
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

    //cout<<"if1\n";
	if(!consume(LPAR))
	{
		cout<<"missing ( after if\n";
        exit(0);
    }

    //cout<<"if2 at "<<curTk<<"\n";
	if(!ruleExpr())
	{
		cout<<"invalid expression after if(\n";
        exit(0);
    }

    if (rv.type.typeBase == TB_STRUCT)
    {
        cout<<"A structure can't be logically tested(if)!\n";
        exit(0);
    }
    //cout<<"if3 at "<<curTk<<"\n";
	if(!consume(RPAR))
	{
		cout<<"missing ) after if\n";
        exit(0);
    }

    int b1 = addInstr(bytecode, Instr(JF_I, 0));

    cout<<"if4\n";
	if(!ruleStm())
	{
		cout<<"missing if statement\n";
        exit(0);
    }

    cout<<"if5\n";
	//see whether we have an else
	if(consume(ELSE))
    {
        int b2 = addInstr(bytecode, Instr(JMP, 0));
        bytecode[b1].args[0].i = b2 + 1;
        if (!ruleStm())
        {
            cout << "missing else statement\n";
            exit(0);
        }
        bytecode[b2].args[0].i = bytecode.size();
    }
    else
    {
        bytecode[b1].args[0].i = bytecode.size();
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

    if (rv.type.typeBase == TB_STRUCT)
    {
        cout<<"A structure can't be logically tested (while)!\n";
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

    if (rv.type.typeBase == TB_STRUCT)
    {
        cout<<"A structure can't be logically tested (for)!\n";
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
    if (rv.type.typeBase == TB_VOID)
    {
        cout<<"A void function can't return a value!\n";
        exit(0);
    }

	if(!consume(SEMICOLON))
	{
        cout<<"missing ; after return\n";
        exit(0);
    }
    addInstr(bytecode, Instr(RET, 0, 0));
    return 1;
}

int SemanticAnalyzer::ruleExpr()
{
    cout<<"Expr\n";
	return ruleExprAssign();
}

int SemanticAnalyzer::ruleExprAssign()
{
    cout<<"Expr assign "<<curTk<<"\n";
    int startTk = curTk;

    if(ruleExprUnary())
    {
        auto rv1 = rv;

        if(!consume(ASSIGN))
        {
            curTk = startTk;
            cout<<"= not found\n";
        }

        else
        {
            if(!rv1.isLVal)
            {
                cout<<rv1.type.typeBase;
                cout<<"Can't assign to a non lval (expr assign)!\n";
                exit(0);
            }

            if(!ruleExprAssign())
            {
                cout<<"Missing expression after =\n";
                exit(0);
            }
            auto rv2 = rv;

            if(rv1.type.nElements > -1 || rv2.type.nElements > -1)
            {
                cout<<"Arrays can't be assigned (expr assign)!\n";
                exit(0);
            }
            cast(rv1.type, rv2.type);

            //addInstr(bytecode, NOP);
            addInstr(bytecode, Instr(PUSHCT_A, rv1.addr));
            if(rv2.isLVal)
            {
                addInstr(bytecode, Instr(PUSHCT_A, rv2.addr));
                addInstr(bytecode, Instr(LOAD, typeArgSize(rv2.type)));
            }
            else
                addInstr(bytecode, Instr(PUSHCT_I, rv2.ctVal.i));

            addInstr(bytecode, Instr(STORE, typeArgSize(rv1.type)));
            //addInstr(bytecode, NOP);

            rv1.isCtVal = 0;
            rv1.isLVal = 0;
            rv = rv1;
            return 1;
        }
    }

    if(ruleExprOr())
        return 1;
    cout<<"a\n";

    return 0;
}

int SemanticAnalyzer::ruleExprOr()
{
    cout<<"Expr or "<<curTk<<"\n";
    if(!ruleExprAnd())
        return 0;

    ruleExprOr1();
	return 1;
}

int SemanticAnalyzer::ruleExprOr1()
{
    cout<<"Expr or1 "<<curTk<<"\n";
    auto rv1 = rv;
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

        if(rv1.type.typeBase == TB_STRUCT || rv.type.typeBase == TB_STRUCT)
        {
            cout << "A structure cannot be logically tested\n";
            exit(0);
        }
        rv.type = Type{TB_INT,-1};
        rv.isCtVal = 0;
        rv.isLVal = 0;
    }

    return 1;
}

int SemanticAnalyzer::ruleExprAnd()
{
    cout<<"Expr and "<<curTk<<"\n";
    if(!ruleExprEq())
        return 0;

    ruleExprAnd1();
    return 1;
}

int SemanticAnalyzer::ruleExprAnd1()
{
    auto rv1 = rv;
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

        if(rv1.type.typeBase == TB_STRUCT || rv.type.typeBase == TB_STRUCT)
        {
            cout << "A structure cannot be logically tested\n";
            exit(0);
        }
        rv.type = Type{TB_INT,-1};
        rv.isCtVal = 0;
        rv.isLVal = 0;
    }

    return 1;
}

int SemanticAnalyzer::ruleExprEq()
{
    cout<<"Expr eq "<<curTk<<"\n";
    if(!ruleExprRel())
        return 0;

    ruleExprEq1();
    return 1;
}

int SemanticAnalyzer::ruleExprEq1()
{
    auto rv1 = rv;
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

        if(rv1.type.typeBase == TB_STRUCT || rv.type.typeBase == TB_STRUCT)
        {
            cout << "A structure cannot be compared\n";
            exit(0);
        }
        rv.type = Type{TB_INT,-1};
        rv.isCtVal = 0;
        rv.isLVal = 0;
    }

    return 1;
}

int SemanticAnalyzer::ruleExprRel()
{
    cout<<"Expr rel "<<curTk<<"\n";
    if(!ruleExprAdd())
        return 0;

    ruleExprRel1();
	return 1;
}

int SemanticAnalyzer::ruleExprRel1()
{
    auto rv1 = rv;
    int comparison = 0;
    if(consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ))
    {
        comparison = tokenList[lastTk].code;
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

        if(rv1.type.nElements > -1 || rv.type.nElements > -1)
        {
            cout<<"An array cannot be compared\n";
            exit(0);
        }

        if(rv1.type.typeBase == TB_STRUCT || rv.type.typeBase == TB_STRUCT)
        {
            cout << "A structure cannot be compared\n";
            exit(0);
        }

        cout<<"COMPARISON: "<<comparison<<"\n";
        if(comparison == GREATER)
        {
            getRVal(bytecode, rv);
            getRVal(bytecode, rv1);
            addInstr(bytecode, Instr(GREATER_I));
        }

        rv.type = Type{TB_INT,-1};
        rv.isCtVal = 0;
        rv.isLVal = 0;
    }

    return 1;
}

int SemanticAnalyzer::ruleExprAdd()
{
    cout<<"Expr add "<<curTk<<"\n";
    if(!ruleExprMul())
        return 0;

    ruleExprAdd1();
    return 1;
}

int SemanticAnalyzer::ruleExprAdd1()
{
    auto rv1 = rv;
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

        if(rv1.type.nElements > -1 || rv.type.nElements > -1)
        {
            cout<<"An array cannot be added/subtracted\n";
            exit(0);
        }

        if(rv1.type.typeBase == TB_STRUCT || rv.type.typeBase == TB_STRUCT)
        {
            cout << "A structure cannot be added/subtracted\n";
            exit(0);
        }
        rv.type = getArithType(rv1.type, rv.type);
        rv.isCtVal = 0;
        rv.isLVal = 0;
    }

    return 1;
}

int SemanticAnalyzer::ruleExprMul()
{
    cout<<"Expr mul "<<curTk<<"\n";
    if(!ruleExprCast())
        return 0;

    ruleExprMul1();
    return 1;
}

int SemanticAnalyzer::ruleExprMul1()
{
    auto rv1 = rv;
    if(consume(MUL) || consume(DIV))
    {
        if(!ruleExprCast())
        {
            cout<<"Missing expression after multiplication\n";
            exit(0);
        }

        if(!ruleExprMul1())
        {
            cout<<"Missing expression after multiplication\n";
            exit(0);
        }

        if(rv1.type.nElements > -1 || rv.type.nElements > -1)
        {
            cout<<"An array cannot be multiplied/divided\n";
            exit(0);
        }

        if(rv1.type.typeBase == TB_STRUCT || rv.type.typeBase == TB_STRUCT)
        {
            cout << "A structure cannot be multiplied/divided\n";
            exit(0);
        }
        rv.type = getArithType(rv1.type, rv.type);
        rv.isCtVal = 0;
        rv.isLVal = 0;
    }

    return 1;
}

int SemanticAnalyzer::ruleExprCast()
{
    cout<<"Expr cast "<<curTk<<"\n";
    if(ruleExprUnary())
        return 1;

    if(!consume(LPAR))
        return 0;
    Type to = ruleTypeName();
    if(to.typeBase == TB_NONE)
    {
        cout<<"Missing typeBase in cast\n";
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
    cast(to, rv.type);
    rv.type = to;
    rv.isCtVal = 0;
    rv.isLVal = 0;
    return 1;
}

int SemanticAnalyzer::ruleExprUnary()
{
    int op = -1;
    cout<<"Expr unary "<<curTk<<"\n";
    if(ruleExprPostfix())
        return 1;

    if(consume(SUB))
        op = SUB;
    else if (consume(NOT))
        op = NOT;
    else
        return 0;

    if(!ruleExprUnary())
    {
        cout<<"Missing unary expression after addition!\n";
        exit(0);
    }

    if (op == SUB)
    {
        if (rv.type.nElements > -1)
        {
            cout<<"Unary '-' cannot be applied to an array!\n";
            exit(0);
        }
        if (rv.type.typeBase == TB_STRUCT)
        {
            cout<<"Unary '-' cannot be applied to a struct!\n";
            exit(0);
        }
    }
    else if (op == NOT)
    {
        if (rv.type.typeBase == TB_STRUCT)
        {
            cout<<"Unary '!' cannot be applied to a struct!\n";
            exit(0);
        }
        rv.type.typeBase = TB_INT;
        rv.type.nElements = -1;
    }
    else
    {
        //error
        cout<<"Invalid unary operator!\n";
        exit(0);
    }
    rv.isCtVal = 0;
    rv.isLVal = 0;
    return 1;
}

int SemanticAnalyzer::ruleExprPostfix()
{
    cout<<"Expr postfix "<<curTk<<"\n";
    if(!ruleExprPrimary())
        return 0;

    ruleExprPostfix1();
    return 1;
}

int SemanticAnalyzer::ruleExprPostfix1()
{
    cout<<"Expr postfix1 "<<curTk<<"\n";
    auto rv1 = rv;

    if(consume(LBRACKET))
    {
        cout<<"Expr postfix2"<<endl;
        if(!ruleExpr())
        {
            cout<<"Missing expression after ()\n";
            exit(0);
        }

        auto rv2 = rv;
        if(rv1.type.nElements < 0)
        {
            printSymbolTable(symbols);
            cout << "only an array can be indexed\n";
            exit(0);
        }

        Type typeInt = Type{TB_INT,-1};
        cast(typeInt, rv2.type);

        rv1.type = rv2.type;
        rv1.type.nElements = -1;
        rv1.isLVal = 1;
        rv1.isCtVal = 0;

        rv = rv1;

        if(!consume(RBRACKET))
        {
            cout<<"Missing ) after expression\n";
            exit(0);
        }

        ruleExprPostfix1();
        //return 1;
    }

    if(consume(DOT))
    {
        cout<<"Expr postfix3\n";
        if(!consume(ID))
        {
            cout<<"Missing identifier after .\n";
            exit(0);
        }

        string field = tokenList[lastTk].text;
        cout<<"Atribut "<<field<<endl;
        Symbol *sStruct = rv_struct.type.struct_type;
        cout<<"Struct at "<<sStruct<<endl;
        Symbol *sMember = findSymbol(sStruct->args, field);
        if(!sMember)
        {
            cout<<"Struct "<<field<<" does not have a member "<<sStruct->name<<"\n";
            exit(0);
        }

        cout<<"AA\n";
        rv.type.typeBase = sMember->type;
        rv.isLVal = 1;
        rv.isCtVal = 0;

        ruleExprPostfix1();
        return 1;
    }

    return 0;
}

int SemanticAnalyzer::ruleExprPrimary()
{
    cout<<"Expr primary "<<curTk<<"\n";
    if (consume(CT_INT))
    {
        cout<<"Found CT_INT\n";
        rv.type = Type{TB_INT};
        rv.ctVal.i = tokenList[lastTk].i;
        rv.isCtVal = 1;
        rv.isLVal = 0;
        //addInstr(bytecode, Instr(PUSHCT_I, tokenList[lastTk].i));
        return 1;
    }
    else if (consume(CT_REAL))
    {
        rv.type = Type{TB_DOUBLE};
        rv.ctVal.i = tokenList[lastTk].r;
        rv.isCtVal = 1;
        rv.isLVal = 0;
        //addInstr(bytecode, Instr(PUSHCT_D, tokenList[lastTk].r));
        return 1;
    }
    else if (consume(CT_CHAR))
    {
        rv.type = Type{TB_CHAR};
        rv.ctVal.i = tokenList[lastTk].i;
        rv.isCtVal = 1;
        rv.isLVal = 0;
        //addInstr(bytecode, Instr(PUSHCT_C, tokenList[lastTk].i));
        return 1;
    }
    else if (consume(CT_STRING))
    {
        rv.type = Type{TB_CHAR};
        rv.ctVal.str = tokenList[lastTk].text;
        rv.isCtVal = 1;
        rv.isLVal = 0;
        //addInstr(bytecode, Instr(PUSHCT_A, tokenList[lastTk].text));    //won't work IF the string is not put in .data
        return 1;
    }

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
        string id_name = tokenList[lastTk].text;
        cout<<"found id "<<id_name<<"\n";
        printSymbolTable(symbols);
        Symbol *s = findSymbol(symbols, id_name);
        if(!s)
        {
            cout<<id_name<<" undefined symbol \n";
            exit(0);
        }

        rv.type.typeBase = s->type;
        rv.type.nElements = s->nElements;
        rv.type.struct_type = s->struct_type;
        rv.isCtVal = 0;
        rv.isLVal = 1;
        rv.addr = reinterpret_cast<void *>(s->addr);
        cout<<"Type: "<<rv.type.typeBase<<"\n";
        cout<<"Is lval: "<<rv.isLVal<<"\n";
        cout<<"Struct: "<<rv.type.struct_type<<"\n";
        cout<<"N elements: "<<rv.type.nElements<<"\n";

        if(rv.type.struct_type != nullptr)
            rv_struct = rv;

        if(!ruleExprPrimaryInner1(s))
        {
            if (s->cls == CLS_FUNC || s->cls == CLS_EXTFUNC)
            {
                cout << "Missing call for function " << id_name << "\n";
                exit(0);
            }

            //variable
            /*if(s->depth)
            {
                addInstr(bytecode, Instr(PUSHFPADDR, s->offset));
            }

            else
            {
                addInstr(bytecode, Instr(PUSHCT_A, s->addr));
            }*/
        }

        return 1;
    }

    return 0;
}

set <int> visited;
int SemanticAnalyzer::ruleExprPrimaryInner1(Symbol * s)
{
    cout<<"Expr primary1 at "<<curTk<<"\n";
    int skip = 0;
    if (visited.find(curTk) != visited.end())
    {
        cout<<"Skipping already tested expression primary at "<<curTk<<"\n";
        skip = 1;
        //return 1;
    }
    visited.insert(curTk);

    if(!consume(LPAR))
        return 0;

    if(s->cls != CLS_FUNC && s->cls != CLS_EXTFUNC)
    {
        cout<<"Call of the non-function "<<s->name<<"\n";
        exit(0);
    }

    int i = 0;
    if(ruleExpr())
    {
        while (1)
        {
            if(i >= s->args.size())
            {
                cout<<"Too many arguments in call!\n";
                exit(0);
            }

            if(s->args[i]->nElements < 0)
            {  //only arrays are passed by addr
                if(!skip)
                {
                    if (rv.isLVal) {
                        addInstr(bytecode, Instr(PUSHCT_A, rv.addr));
                        addInstr(bytecode, Instr(LOAD, typeArgSize(rv.type)));
                    } else
                        addInstr(bytecode, Instr(PUSHCT_I, rv.ctVal.i));
                }
            }
            else
            {
                //Instr in = bytecode[bytecode.size() - 1];
                //addInstr(bytecode, )
            }
            //addCastInstr(i,&arg.type,&(*crtDefArg)->type);

            Type temp;
            temp.typeBase = s->args[i]->type;
            temp.nElements = s->args[i]->nElements;
            temp.struct_type = s->args[i]->struct_type;

            cast(temp, rv.type);
            i++;

            if(!consume(COMMA))
                break;

            if(!ruleExpr())
            {
                cout<<"Missing expression after comma\n";
                exit(0);
            }
        }
    }

    if(!consume(RPAR))
    {
        cout<<"Missing ) after expression\n";
        exit(0);
    }

    if(!skip)
    {
        if (s->cls == CLS_FUNC)
            addInstr(bytecode, Instr(CALL, s->addr));
        else
            addInstr(bytecode, Instr(CALLEXT, (void *) (s->addr)));
    }

    if (i + 1 < s->args.size())
    {
        cout<<"Too few arguments in call!\n";
        exit(0);
    }
    rv.type = s->type;
    rv.isCtVal = 0;
    rv.isLVal = 0;

    return 1;
}

void initGlobalSymbols(Symbols & symbol_table)
{
    Symbol * put_s = addExtFunc(symbol_table, "put_s", TB_VOID, 0x10000);
    addFuncArg(put_s, "s", TB_CHAR, nullptr, 0);

    printSymbolTable(symbol_table);
    Symbol * get_s = addExtFunc(symbol_table, "get_s", TB_VOID, 0x20000);
    addFuncArg(get_s, "s", TB_CHAR, nullptr, 0);

    Symbol * put_i = addExtFunc(symbol_table, "put_i", TB_VOID, 0x30000);
    addFuncArg(put_i, "i", TB_INT);

    Symbol * get_i = addExtFunc(symbol_table, "get_i", TB_INT, 0x400000);

    Symbol * put_d = addExtFunc(symbol_table, "put_d", TB_VOID, 0x50000);
    addFuncArg(put_d, "d", TB_DOUBLE);

    Symbol * get_d = addExtFunc(symbol_table, "get_d", TB_DOUBLE, 0x60000);

    Symbol * put_c = addExtFunc(symbol_table, "put_c", TB_VOID, 0x70000);
    addFuncArg(put_c, "c", TB_CHAR);

    Symbol * get_c = addExtFunc(symbol_table, "get_c", TB_CHAR, 0x80000);
    printSymbolTable(symbol_table);
}

int SemanticAnalyzer::ruleUnit()
{
	bool ok = true;
    initGlobalSymbols(symbols);
    addInstr(bytecode, Instr(CALL, 0));
    addInstr(bytecode, Instr(HALT));

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

            //set first call to main
            bytecode[0].args[0].i = findSymbol(symbols, "main")->addr;
            cout<<"Nr instructions: "<<bytecode.size()<<"\n";
            for (int i = 0; i < bytecode.size(); ++i)
                cout<<i<<": "<<bytecode[i];
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
