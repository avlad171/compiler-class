#ifndef SYMBOL_TABLE_H_INCLUDED
#define SYMBOL_TABLE_H_INCLUDED

#include <string>
#include <utility>
#include <vector>
#include <iostream>

enum {TB_NONE = 0, TB_INT, TB_DOUBLE, TB_CHAR, TB_STRUCT, TB_VOID};
enum {CLS_VAR, CLS_FUNC, CLS_EXTFUNC, CLS_STRUCT};
enum {MEM_GLOBAL, MEM_ARG, MEM_LOCAL};

struct Symbol
{
    std::string name;
    int cls;    // CLS_*
    int mem;    // MEM_*
    int type;   // TB_*
    Symbol * struct_type;
    int nElements = -1; //only for arrays
    int depth; // 0-global, 1-in function, 2... - nested blocks in function
    int64_t addr = 0;
    int offset = 0;

    std::vector<Symbol*> args;

    Symbol(std::string name, int cls, int mem, int type, int depth=0, Symbol * struct_type=nullptr, int nElements=0)
    {
        this->name = std::move(name);
        this->cls = cls;
        this->mem = mem;
        this->type = type;
        this->depth = depth;
        this->struct_type = struct_type;
        this->nElements = nElements;

        std::cout<<"Creating symbol with name: "<<this->name<<"\n";
    }

    ~Symbol()
    {
        for (Symbol * s : args)
            delete s;
    }

};

struct Type
{
    int typeBase = TB_NONE;
    Symbol * struct_type = nullptr;
    int nElements = 0;

    Type(int typeBase, int nElements = -1)
    {
        this->typeBase = typeBase;
        this->nElements = nElements;
    }

    Type() = default;
};

typedef union
{
    int i; // int, char
    double d; // double
    const char *str; // char[]
} CtVal;

struct RetVal
{
    Type type; // type of the result
    int isLVal; // if it is a LVal
    int isCtVal; // if it is a constant value (int, real, char, char[])
    CtVal ctVal; // the constat value
    void * addr;
};

typedef std::vector<Symbol*> Symbols;

Symbol* addSymbol(Symbols & symbols, std::string name, int cls, int mem = MEM_GLOBAL, int type = TB_INT, int depth = 0, Symbol * struct_type = nullptr, int nElements = -1, int64_t addr = 0);
Symbol* addSymbol(Symbols & symbols, const char * name, int cls, int mem = MEM_GLOBAL, int type = TB_INT, int depth = 0, Symbol * struct_type = nullptr, int nElements = -1, int64_t addr = 0);

Symbol* findSymbol(Symbols & symbols, const char * name);
Symbol* findSymbol(Symbols & symbols, std::string name);

int deleteSymbolsAfter(Symbols & symbols, Symbol * s);

void printSymbolTable(Symbols & symbols);

//types analysis
void cast(const Type &, const Type &);
Type getArithType(const Type &, const Type &);

//pre-defined functions
Symbol *addExtFunc(Symbols & symbols, const char *name, int type = TB_INT, int64_t addr = 0, Symbol * struct_type = nullptr, int nElements = -1);
Symbol *addFuncArg(Symbol *func, const char *name, int type = TB_INT, Symbol * struct_type = nullptr, int nElements = -1);

#endif // SYMBOL_TABLE_H_INCLUDED
