#include "symbol_table.h"
using namespace std;

Symbol * addSymbol(Symbols & symbols, string name, int cls, int mem, int type, int depth, Symbol * struct_type, int nElements)
{
    Symbol * s = new Symbol(name, cls, mem, type, depth, struct_type, nElements);
    return s;
}

Symbol * addSymbol(Symbols & symbols, const char * name, int cls, int mem, int type, int depth, Symbol * struct_type, int nElements)
{
    string n = name;
    Symbol * s = new Symbol(n, cls, mem, type, depth, struct_type, nElements);
    return s;
}

Symbol * findSymbol(Symbols & symbols, const char * name)
{
    string n = name;
    return findSymbol(symbols, n);
}

Symbol * findSymbol(Symbols & symbols, string name)
{
    for (auto it = symbols.rbegin(); it != symbols.rend(); it++)
    {
        if ((*it)->name == name)
            return *it;
    }

    return nullptr;
}

int deleteSymbolsAfter(Symbols & symbols, Symbol * s)
{
    for (auto it = symbols.begin(); it != symbols.end(); ++it)
    {
        if (*it == s)
        {
            //it is now the end of the symbol table
            for (auto it2 = it++; it2 != symbols.end(); ++it2)
            {
                delete (*it2);
            }

            symbols.erase(it++, symbols.end());
        }
    }

    return 1;
}