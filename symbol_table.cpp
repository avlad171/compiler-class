#include "symbol_table.h"
using namespace std;

Symbol * addSymbol(Symbols & symbols, string name, int cls, int mem, int type, int depth, Symbol * struct_type, int nElements)
{
    Symbol * s = new Symbol(name, cls, mem, type, depth, struct_type, nElements);
    symbols.push_back(s);
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
            it++;   //skip current elem
            //it is now the end of the symbol table
            for (auto it2 = it; it2 != symbols.end(); ++it2)
            {
                cout<<"Deleting "<<(*it2)->name<<"\n";
                delete (*it2);
            }

            symbols.erase(it, symbols.end());
            break;
        }
    }

    return 1;
}

void printSymbolTable(Symbols & symbols)
{
    cout<<symbols.size()<<": ";
    for (auto it = symbols.begin(); it != symbols.end(); ++it)
    {
        cout<<"("<<(*it)->name<<", "<<(*it)->type<<", "<<(*it)->struct_type<<") ";
    }
    cout<<endl;
}