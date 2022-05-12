#ifndef SYMBOL_TABLE_H_INCLUDED
#define SYMBOL_TABLE_H_INCLUDED

/*enum {TB_INT, TB_DOUBLE, TB_CHAR, TB_STRUCT, TB_VOID};

typedef struct
{
    int typeBase; // TB_*
    Symbol *s; // struct definition for TB_STRUCT
    int nElements; // >0 array of given size, 0=array without size, <0 non array
}Type;

enum {CLS_VAR, CLS_FUNC, CLS_EXTFUNC, CLS_STRUCT};
enum {MEM_GLOBAL, MEM_ARG, MEM_LOCAL};

typedef struct _Symbol
{
    const char *name; // a reference to the name stored in a token
    int cls; // CLS_*
    int mem; // MEM_*
    Type type;
    int depth; // 0-global, 1-in function, 2... - nested blocks in function
    union
    {
        Symbols args; // used only of functions
        Symbols members; // used only for structs
    };
}Symbol;

Symbols symbols;

struct _Symbol;
typedef struct _Symbol Symbol;
typedef struct
{
    Symbol **begin; // the beginning of the symbols, or NULL
    Symbol **end; // the position after the last symbol
    Symbol **after; // the position after the allocated space
}Symbols;
void initSymbols(Symbols *symbols)
{
symbols->begin=NULL;
symbols->end=NULL;
symbols->after=NULL;

Symbol *addSymbol(Symbols *symbols,const char *name,int cls)
{
    Symbol *s;
    if(symbols->end==symbols->after)
    { // create more room
        int count=symbols->after-symbols->begin;
        int n=count*2; // double the room
        if(n==0)
            n=1; // needed for the initial case

        symbols->begin=(Symbol**)realloc(symbols->begin, n*sizeof(Symbol*));
        if(symbols->begin==NULL)err("not enough memory");
            symbols->end=symbols->begin+count;
        symbols->after=symbols->begin+n;
    }

    SAFEALLOC(s,Symbol)

    *symbols->end++=s;
    s->name=name;
    s->cls=cls;
    s->depth=crtDepth;
    return s;
}
*/

#include <string>
#include <utility>
#include <vector>

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
    int nElements; //only for arrays
    int depth; // 0-global, 1-in function, 2... - nested blocks in function

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
    }

    ~Symbol()
    {
        for (Symbol * s : args)
            delete s;
    }

};

struct TypeBase
{
    int type = TB_NONE;
    Symbol * struct_type = nullptr;
    int nElements = 0;
};


typedef std::vector<Symbol*> Symbols;

Symbol* addSymbol(Symbols & symbols, std::string name, int cls, int mem = MEM_GLOBAL, int type = TB_INT, int depth = 0, Symbol * struct_type = nullptr, int nElements = 0);
Symbol* addSymbol(Symbols & symbols, const char * name, int cls, int mem = MEM_GLOBAL, int type = TB_INT, int depth = 0, Symbol * struct_type = nullptr, int nElements = 0);

Symbol* findSymbol(Symbols & symbols, const char * name);
Symbol* findSymbol(Symbols & symbols, std::string name);

int deleteSymbolsAfter(Symbols & symbols, Symbol * s);

#endif // SYMBOL_TABLE_H_INCLUDED
