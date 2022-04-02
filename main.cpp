#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <vector>

using namespace std;

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

char* createString(char * p1, char * p2)
{
    int len = p2 - p1;
    char * ans = new char[len + 1];
    memcpy(ans, p1, len);
    ans[len] = 0;
    cout<<"[DEBUG] Created string "<<ans<<", l="<<len<<"\n";
    return ans;
}

class LexicalAnalyzer
{
    //pointer to source code to translate
    char * src = nullptr;
    char * pCrtCh = nullptr;

    //status variable
    int status = 0; //0 - not run, 1 - done, 2 - error

    //token list
    vector<Token> tokenList;
    void addTk(int code, int line = 0, char * txt = nullptr)
    {
        Token temp;
        temp.code = code;
        temp.line = line;
        if(txt == nullptr)
        {
            temp.text = new char[2];
            temp.text[0] = 0;
            temp.text[1] = 0;
        }
        tokenList.push_back(temp);
    }

    void addTk(Token t)
    {
        tokenList.push_back(t);
    }

    //dfa
    int getNextToken()
    {
        int code = 0;
        int line = 0;
        int state = 0;
        int nCh = 0;
        char ch = 0;
        char *pStartCh = pCrtCh;
        int prec_spaces = 0;
        Token temp;

        while(1)
        { // infinite loop
            ch = *pCrtCh;
            //cout<<"At "<<ch<<" "<<(int)ch<<" state = "<<state<<"\n";
            switch(state)
            {
                case 0: // transitions test for state 0
                    if(ch == 0)
                    { // the end of the input string
                        cout<<"END\n";
                        addTk(END);
                        status = 1;
                        return 1;
                    }
                    else if(isalpha(ch) || ch == '_')
                    {
                        pStartCh = pCrtCh; // memorizes the beginning of the ID
                        pCrtCh++; // consume the character
                        state = 1; // set the new state
                    }
                    //delimiters
                    else if(strchr(",;()[]{}", ch))
                    {
                        pStartCh = pCrtCh; // memorizes the beginning of the ID
                        pCrtCh++; // consume the character
                        state = 0; // set the new state

                        switch(ch)
                        {
                        case ',':
                            code = COMMA;
                            break;
                        case ';':
                            code = SEMICOLON;
                            break;
                        case '(':
                            code = LPAR;
                            break;
                        case ')':
                            code = RPAR;
                            break;
                        case '[':
                            code = LBRACKET;
                            break;
                        case ']':
                            code = RBRACKET;
                            break;
                        case '{':
                            code = LACC;
                            break;
                        case '}':
                            code = RACC;
                            break;
                        }

                        temp.code = code;
                        temp.text = createString(pStartCh, pCrtCh);
                        addTk(temp);
                        return 0;
                    }
                    //operators
                    else if(strchr("+-*.", ch))
                    {
                        pStartCh = pCrtCh; // memorizes the beginning of the ID
                        pCrtCh++; // consume the character
                        state = 0; // set the new state

                        switch(ch)
                        {
                        case '+':
                            code = ADD;
                            break;
                        case '-':
                            code = SUB;
                            break;
                        case '*':
                            code = MUL;
                            break;
                        case '.':
                            code = DOT;
                            break;
                        }

                        temp.code = code;
                        temp.text = createString(pStartCh, pCrtCh);
                        addTk(temp);
                    }

                    else if(ch == '&')
                    {
                        pCrtCh++;
                        state = 36;
                    }

                    else if(ch == '|')
                    {
                        pCrtCh++;
                        state = 37;
                    }

                    else if(ch == '!')
                    {
                        pCrtCh++;
                        state = 38;
                    }

                    //comparison
                    else if(ch == '=')
                    {
                        pCrtCh++;
                        state = 3;
                    }

                    else if(ch == '<')
                    {
                        pCrtCh++;
                        state = 31;
                    }

                    else if(ch == '>')
                    {
                        pCrtCh++;
                        state = 39;
                    }

                    else if(ch >= '1' && ch <= '9')
                    {
                        pCrtCh++;
                        state = 6;
                    }

                    else if(ch == '0')
                    {
                        pCrtCh++;
                        state = 9;
                    }

                    else if(ch == '/')
                    {
                        pCrtCh++;
                        state = 13;
                    }

                    else if(ch == ' ' || ch == '\r' || ch == '\t')
                    {
                        pCrtCh++; // consume the character and remains in state 0
                        pStartCh = pCrtCh;
                    }

                    else if(ch == '\n')
                    { // handled separately in order to update the current line
                        line++;
                        pCrtCh++;
                        pStartCh = pCrtCh;
                    }

                    else if(ch == '\'')
                    {
                        pCrtCh++;
                        state = 23; //a character
                    }
                    else if(ch == '\"')
                    {
                        pCrtCh++;
                        state = 27; //a string
                    }

                    else
                    {
                        cout<<"Found unsupported "<<ch<<" ";
                        //tkerr(addTk(END), "invalid character");
                        status = 2;
                        return 2;
                    }
                    break;

                case 1:
                    if(isalnum(ch) || ch == '_')
                        pCrtCh++;
                    else state = 2;
                    break;

                case 2:
                    nCh = pCrtCh - pStartCh; // the id length
                    // keywords tests
                    if(nCh == 5 && !memcmp(pStartCh, "break", 5))
                        code = BREAK;
                    else if(nCh == 4 && !memcmp(pStartCh, "char", 4))
                        code = CHAR;
                    else if(nCh == 6 && !memcmp(pStartCh, "double", 6))
                        code = DOUBLE;
                    else if(nCh == 4 && !memcmp(pStartCh, "else", 4))
                        code = ELSE;
                    else if(nCh == 3 && !memcmp(pStartCh, "for", 3))
                        code = FOR;
                    else if(nCh == 2 && !memcmp(pStartCh, "if", 2))
                        code = IF;
                    else if(nCh == 3 && !memcmp(pStartCh, "int", 3))
                        code = INT;
                    else if(nCh == 6 && !memcmp(pStartCh, "return", 6))
                        code = RETURN;
                    else if(nCh == 6 && !memcmp(pStartCh, "struct", 6))
                        code = STRUCT;
                    else if(nCh == 4 && !memcmp(pStartCh, "void", 4))
                        code = VOID;
                    else if(nCh == 5 && !memcmp(pStartCh, "while", 5))
                        code = WHILE;
                    else
                    { // if no keyword, then it is an ID
                        code = ID;
                        cout<<"ID ";
                    }
                    temp.code = code;
                    temp.text = createString(pStartCh, pCrtCh);
                    addTk(temp);
                    return 0;

                case 3:
                    if(ch == '=')
                    {
                        pCrtCh++;
                        state = 4;
                    }
                    else state = 5;
                    break;

                case 4:
                    temp.code = EQUAL;
                    temp.text = createString(pStartCh, pCrtCh);
                    addTk(temp);
                    cout<<"== ";
                    return 0;

                case 5:
                    temp.code = ASSIGN;
                    temp.text = createString(pStartCh, pCrtCh);
                    addTk(temp);
                    cout<<"= ";
                    return 0;

                case 6:
                    if(isdigit(ch))
                    {
                        pCrtCh++;
                        state = 6;  //why do i need 7?
                    }
                    else if (ch == '.')
                    {
                        pCrtCh++;
                        state = 17;
                    }
                    else if(ch == 'e' || ch == 'E')
                    {
                        pCrtCh++;
                        state = 19;
                    }
                    else
                    {
                        state = 8;
                    }
                    break;

                case 7:
                    if(isdigit(ch))
                    {
                        pCrtCh++;
                        state = 7;
                    }
                    else if (ch == '.')
                    {
                        pCrtCh++;
                        state = 17;
                    }
                    else if(ch == 'e' || ch == 'E')
                    {
                        pCrtCh++;
                        state = 19;
                    }
                    else
                    {
                        cout<<"invalid real const!\n";
                        status = 2;
                        return 2;
                    }
                    break;

                case 8:
                    temp.code = CT_INT;
                    temp.text = createString(pStartCh, pCrtCh);
                    temp.i = strtol(temp.text, NULL, 0);
                    //cout<<"[DEBUG] EXTRACTED CT_INT VALUE: "<<temp.i<<"\n";
                    addTk(temp);
                    return 0;

                case 9:
                    if(ch >= '0' && ch <= '7')
                    {
                        pCrtCh++;
                        state = 10;
                    }
                    else if (ch == '8' || ch == '9')
                    {
                        pCrtCh++;
                        state = 7;
                    }
                    else if(ch == 'x')
                    {
                        pCrtCh++;
                        state = 11;
                    }
                    else if (ch == '.')
                    {
                        pCrtCh++;
                        state = 17;
                    }
                    else if(ch == 'e' || ch == 'E')
                    {
                        pCrtCh++;
                        state = 19;
                    }
                    else
                    {
                        state = 8;
                    }
                    break;

                case 10:
                    if(ch >= '0' && ch <= '7')
                    {
                        pCrtCh++;
                        state = 10;
                    }
                    else if (ch == '8' || ch == '9')
                    {
                        pCrtCh++;
                        state = 7;
                    }
                    else if (ch == '.')
                    {
                        pCrtCh++;
                        state = 17;
                    }
                    else if(ch == 'e' || ch == 'E')
                    {
                        pCrtCh++;
                        state = 19;
                    }
                    else
                    {
                        state = 8;
                    }
                    break;

                case 11:
                    if(isdigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
                    {
                        pCrtCh++;
                        state = 11;
                    }
                    else
                    {
                        state = 8;  //this allows '0x' to be considered valid
                    }
                    break;

                case 13:
                    if(ch == '*')   //multi line comment
                    {
                        pCrtCh++;
                        state = 14;
                    }
                    else if (ch == '/') //single line comment
                    {
                        pCrtCh++;
                        state = 34;
                    }
                    else    //division
                    {
                        temp.code = DIV;
                        temp.text = createString(pStartCh, pCrtCh);
                        addTk(temp);
                        cout<<"/ ";
                        return 0;
                    }
                    break;

                case 14:
                    if(ch == '*')
                    {
                        pCrtCh++;
                        state = 15;
                    }
                    else
                    {
                        pCrtCh++;
                        state = 14;
                    }
                    break;

                case 15:
                    if(ch == '*')
                    {
                        pCrtCh++;
                        state = 15;
                    }
                    else if (ch == '/')
                    {
                        pCrtCh++;
                        state = 16;
                    }
                    else
                    {
                        state = 14;
                    }
                    break;

                case 16:
                    temp.code = COMMENT;
                    temp.text = createString(pStartCh, pCrtCh);
                    addTk(temp);
                    return 0;

                case 17:
                    if(isdigit(ch))
                    {
                        pCrtCh++;
                        state = 18;
                    }
                    else
                    {
                        cout<<"invalid real const!\n";
                        status = 2;
                        return 2;
                    }
                    break;

                case 18:
                    if(isdigit(ch))
                    {
                        pCrtCh++;
                        state = 18;
                    }
                    else if(ch == 'e' || ch == 'E')
                    {
                        pCrtCh++;
                        state = 19;
                    }
                    else
                    {
                        //we have CT_REAL
                        state = 22;
                    }
                    break;

                case 19:
                    if(ch == '+' || ch == '-')
                    {
                        pCrtCh++;
                        state = 20;
                    }
                    else if(isdigit(ch))
                    {
                        pCrtCh++;
                        state = 21;
                    }
                    else
                    {
                        cout<<"invalid real exponential!\n";
                        status = 2;
                        return 2;
                    }
                    break;

                case 20:
                    if(isdigit(ch))
                    {
                        pCrtCh++;
                        state = 21;
                    }
                    else
                    {
                        cout<<"invalid real const!\n";
                        status = 2;
                        return 2;
                    }
                    break;

                case 21:
                    if(isdigit(ch))
                    {
                        pCrtCh++;
                        state = 21;
                    }
                    else
                    {
                        state = 22;
                    }
                    break;

                case 22:
                    temp.code = CT_REAL;
                    temp.text = createString(pStartCh, pCrtCh);
                    temp.r = atof(temp.text);
                    //cout<<"[DEBUG] EXTRACTED CT_REAL VALUE: "<<temp.r<<"\n";
                    addTk(temp);
                    return 0;

                //CT_CHAR
                case 23:
                    if(ch == '\\')
                    {
                        //escaped character
                        pCrtCh++;
                        state = 24;
                    }

                    else if(ch != '\'')
                    {
                        pCrtCh++;
                        state = 25;
                    }

                    else
                    {
                        cout<<"empty CT_CHAR!\n";
                        status = 2;
                        return 2;
                    }
                    break;

                case 24:
                    if(strchr("abfnrtv'?\"\\0", ch))
                    {
                        pCrtCh++;
                        state = 25;
                    }
                    else
                    {
                        cout<<"invalid escaped character!\n";
                        status = 2;
                        return 2;
                    }
                    break;

                case 25:
                    if(ch == '\'')
                    {
                        pCrtCh++;
                        state = 26;
                    }
                    else
                    {
                        cout<<"CT_CHAR not properly ended!\n";
                        status = 2;
                        return 2;
                    }
                    break;

                case 26:
                    temp.code = CT_CHAR;
                    temp.text = createString(pStartCh, pCrtCh);
                    addTk(temp);
                    return 0;


                //CT_STRING
                case 27:
                    if(ch == '\\')
                    {
                        //escaped character
                        pCrtCh++;
                        state = 28;
                    }
                    else if(ch == '\"')
                    {
                        //empty string
                        pCrtCh++;
                        state = 30;
                    }

                    else
                    {
                        pCrtCh++;
                        state = 29;
                    }
                    break;

                case 28:
                    if(strchr("abfnrtv'?\"\\0", ch))
                    {
                        pCrtCh++;
                        state = 29;
                    }
                    else
                    {
                        cout<<"CT_STRING invalid escaped character!\n";
                        status = 2;
                        return 2;
                    }
                    break;

                case 29:
                    if(ch == '\"')
                    {
                        pCrtCh++;
                        state = 30;
                    }
                    else
                    {
                        pCrtCh++;
                        state = 27;
                    }
                    break;

                case 30:
                    temp.code = CT_STRING;
                    temp.text = createString(pStartCh, pCrtCh);
                    addTk(temp);
                    return 0;

                case 31:
                    if(ch == '=')
                    {
                        pCrtCh++;
                        state = 32;
                    }
                    else state = 33;
                    break;

                case 32:
                    //<=
                    temp.code = LESSEQ;
                    temp.text = createString(pStartCh, pCrtCh);
                    addTk(temp);
                    cout<<"<= ";
                    return 0;

                case 33:
                    //<
                    temp.code = LESS;
                    temp.text = createString(pStartCh, pCrtCh);
                    addTk(temp);
                    cout<<"< ";
                    return 0;

                case 34:
                    if(ch != '\n' && ch != '\r' && ch != '\0')
                    {
                        pCrtCh++;
                        state = 34;
                    }
                    else
                    {
                        state = 35;
                    }
                    break;

                case 35:
                    temp.code = LINECOMMENT;
                    temp.text = createString(pStartCh, pCrtCh);
                    addTk(temp);
                    cout<<"// ";
                    return 0;

                case 36:
                    if(ch == '&')
                    {
                        pCrtCh++;
                        temp.code = AND;
                        temp.text = createString(pStartCh, pCrtCh);
                        addTk(temp);
                        cout<<"&& ";
                        return 0;
                    }
                    else
                    {
                        cout<<"Bitwise and not done yet!\n";
                        status = 2;
                        return 2;
                    }

                case 37:
                    if(ch == '|')
                    {
                        pCrtCh++;
                        temp.code = OR;
                        temp.text = createString(pStartCh, pCrtCh);
                        addTk(temp);
                        cout<<"|| ";
                        return 0;
                    }
                    else
                    {
                        cout<<"Bitwise or not done yet!\n";
                        status = 2;
                        return 2;
                    }

                case 38:
                    if(ch == '=')
                    {
                        pCrtCh++;
                        temp.code = NOT;
                        temp.text = createString(pStartCh, pCrtCh);
                        addTk(temp);
                        cout<<"|| ";
                        return 0;
                    }
                    else
                    {
                        cout<<"Negation not done yet!\n";
                        status = 2;
                        return 2;
                    }

                case 39:
                    if(ch == '=')
                    {
                        pCrtCh++;
                        state = 40;
                    }
                    else state = 41;
                    break;

                case 40:
                    temp.code = GREATEREQ;
                    temp.text = createString(pStartCh, pCrtCh);
                    addTk(temp);
                    cout<<"<= ";
                    return 0;

                case 41:
                    //<
                    temp.code = GREATER;
                    temp.text = createString(pStartCh, pCrtCh);
                    addTk(temp);
                    cout<<"< ";
                    return 0;

            }
        }
    }

public:
    LexicalAnalyzer (char * source)
    {
        this->src = source;
    }

    int getStatus()
    {
        return status;
    }

    int run()
    {
        if(status != 0)
            return status;
        //initialize current char from the source beginning
        pCrtCh = src;

        while(status == 0)
        {
            //cout<<"Calling getNextToken()\n";
            getNextToken();
        }

        return status;
    }

    void showTokens()
    {
        cout<<"[DEBUG] Dumping tokens list\n";
        if (status == 0)
        {
            cout<<"Lexical analysis hasn't been run!\n";
            return;
        }

        for (Token & t : tokenList)
        {
            cout<<"("<<t.code<<", "<<t.text<<") ";
        }
        cout<<"\n";
        cout<<"Cursor: "<<pCrtCh - src<<"\n";
    }
};

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
    return 0;
}
