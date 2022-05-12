#ifndef LEXICAL_ANALYSIS_H_INCLUDED
#define LEXICAL_ANALYSIS_H_INCLUDED

#include <vector>
#include <inttypes.h>
#include <string.h>

enum{ID, END, BREAK, CHAR, DOUBLE, ELSE, FOR, INT, IF, RETURN, STRUCT, VOID, WHILE, EQUAL, CT_INT, ASSIGN, SEMICOLON,
COMMENT, CT_REAL, CT_CHAR, CT_STRING, COMMA, LPAR, RPAR, LBRACKET, RBRACKET, LACC, RACC, ADD, SUB, MUL, DIV, NOT, AND, OR, DOT,
NOTEQ, LESS, LESSEQ, GREATER, GREATEREQ, LINECOMMENT}; // tokens codes TBA

struct Token
{
    int code; // code (name)
    char *text = nullptr; // used for ID, CT_STRING (dynamically allocated), represents the text
    union
    {
        int64_t i; // used for CT_INT, CT_CHAR
        double r; // used for CT_REAL
    };

    int line = 0; // the input file line, can be 0
    Token *next = nullptr; // link to the next token, can be null if not used as a list

    Token()
    {
    }

    Token(const Token &t1)
    {
        code = t1.code;
        line = t1.line;
        i = t1.i;

        if (t1.text != nullptr) //todo maybe replace with c++ string
        {
            int l = strlen(t1.text);
            text = new char [l + 1];
            memcpy(text, t1.text, l);
            text[l] = 0;
        }
    }

    ~Token()
    {
        if(text != nullptr)
            delete text;
    }
};

class LexicalAnalyzer
{
    //pointer to source code to translate
    char * src = nullptr;
    char * pCrtCh = nullptr;

    //status variable
    int status = 0; //0 - not run, 1 - done, 2 - error

    //token list
    std::vector<Token> tokenList;
    void addTk(int code, int line = 0, char * txt = nullptr);
    void addTk(Token t);

    //dfa
    int getNextToken();

public:
    LexicalAnalyzer (char * source)
    {
        this->src = source;
    }

    int getStatus()
    {
        return status;
    }

    int run();
    void showTokens();
    void showTokens2();
    std::vector<Token> getTokenList();
};

#endif // LEXICAL_ANALYSIS_H_INCLUDED
