#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <vector>

#include "lexical_analysis.h"
#include "semantic_analysis.h"
using namespace std;

int run(vector<Instr> & bytecode);

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        cout<<"Invalid argument! Please provide a source file!\n";
        return 0;
    }

    FILE * src = fopen(argv[1], "rb");
    if(!src)
    {
        cout<<"Unable to open file!\n";
        return 0;
    }

    fseek(src, 0, SEEK_END);
    int filesize = ftell(src);

    cout<<"[DEBUG] File size is "<<filesize<<"\n";

    fseek(src, 0, SEEK_SET);

    char * filedata = new char [filesize + 2];
    fread(filedata, 1, filesize, src);
    filedata[filesize] = 0;
    filedata[filesize + 1] = 0;

    LexicalAnalyzer L(filedata);
    L.run();
    L.showTokens();
    cout<<"[+] Lexical analysis over!\n";
    cout<<"Filesize: "<<filesize<<"\n";

    SemanticAnalyzer S(L.getTokenList());
    S.run();
    cout<<"EMULATING PROGRAM!\n\n";
    auto spl = S.getBytecode();

    run(spl);
    return 0;
}
