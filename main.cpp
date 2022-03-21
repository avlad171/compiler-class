#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>

using namespace std;

enum{ID, END, BREAK, CHAR, DOUBLE, ELSE, FOR, IF, INT, RETURN, STRUCT, VOID, WHILE, EQUAL, CT_INT, ASSIGN, SEMICOLON, COMMENT}; // tokens codes TBA

void err(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    //exit(-1);
}

#define SAFEALLOC(var, Type) if((var = (Type*)malloc(sizeof(Type))) == NULL) err("not enough memory");

struct Token
{
    int code; // code (name)
    union
    {
        char *text; // used for ID, CT_STRING (dynamically allocated)
        long int i; // used for CT_INT, CT_CHAR
        double r; // used for CT_REAL
    };

    int line; // the input file line
    Token *next; // link to the next token
};

Token * lastToken = nullptr;
Token * head = nullptr;
Token * addTk(int code, int line = 0)
{
    Token *tk;
    SAFEALLOC(tk,Token)
    tk->code = code;
    tk->line = line;
    tk->next = NULL;

    if(lastToken)
    {
        lastToken->next = tk;
    }
    else
    {
        head = tk;
    }

    lastToken = tk;
    return tk;
}

void tkerr(const Token *tk,const char *fmt,...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error in line %d: ", tk->line);
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    //exit(-1);
}

char* createString(char * p1, char * p2)
{
    int len = p2 - p1;
    char * ans = (char *)malloc(len + 1);
    memcpy(ans, p1, len);
    ans[len] = 0;
    cout<<"Created string "<<ans<<"\n";
    return ans;
}


int getNextToken(char * str)
{
    int line = 0;
    int state = 0;
    int nCh = 0;
    char ch = 0;
    char *pStartCh = str;
    char * pCrtCh = str;
    int prec_spaces = 0;
    Token *tk;

    while(1)
    { // infinite loop
        ch = *pCrtCh;
        cout<<"At "<<ch<<" "<<(int)ch<<" state = "<<state<<"\n";
        switch(state)
        {
            case 0: // transitions test for state 0
                if(isalpha(ch) || ch == '_')
                {
                    pStartCh = pCrtCh; // memorizes the beginning of the ID
                    pCrtCh++; // consume the character
                    state = 1; // set the new state
                }

                else if(ch == '=')
                {
                    pCrtCh++;
                    state = 3;
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
                    prec_spaces++;
                }

                else if(ch == '\n')
                { // handled separately in order to update the current line
                    line++;
                    pCrtCh++;
                }

                else if(ch == 0)
                { // the end of the input string
                    cout<<"END ";
                    addTk(END);
                    return 0;
                }

                else
                {
                    cout<<"Found unsupported "<<ch<<" ";
                    tkerr(addTk(END), "invalid character");
                    return 0;
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
                    tk = addTk(BREAK);
                else if(nCh == 4 && !memcmp(pStartCh, "char", 4))
                    tk = addTk(CHAR);
                else if(nCh == 6 && !memcmp(pStartCh, "double", 6))
                    tk = addTk(DOUBLE);
                else if(nCh == 4 && !memcmp(pStartCh, "else", 4))
                    tk = addTk(ELSE);
                else if(nCh == 3 && !memcmp(pStartCh, "for", 3))
                    tk = addTk(FOR);
                else if(nCh == 2 && !memcmp(pStartCh, "if", 2))
                    tk = addTk(IF);
                else if(nCh == 3 && !memcmp(pStartCh, "int", 3))
                    tk = addTk(INT);
                else if(nCh == 6 && !memcmp(pStartCh, "return", 6))
                    tk = addTk(RETURN);
                else if(nCh == 6 && !memcmp(pStartCh, "struct", 6))
                    tk = addTk(STRUCT);
                else if(nCh == 4 && !memcmp(pStartCh, "void", 4))
                    tk = addTk(VOID);
                else if(nCh == 5 && !memcmp(pStartCh, "while", 5))
                    tk = addTk(WHILE);
                else
                { // if no keyword, then it is an ID
                    tk = addTk(ID);
                    cout<<"ID ";
                }
                tk->text = createString(pStartCh, pCrtCh);
                return nCh + prec_spaces;

            case 3:
                if(ch == '=')
                {
                    pCrtCh++;
                    state = 4;
                }
                else state = 5;
                break;

            case 4:
                tk = addTk(EQUAL);
                tk->text = createString(pStartCh, pCrtCh);
                cout<<"== ";
                return pCrtCh - pStartCh + prec_spaces;

            case 5:
                tk = addTk(ASSIGN);
                tk->text = createString(pStartCh, pCrtCh);
                cout<<"= ";
                return pCrtCh - pStartCh + prec_spaces;

            case 6:
                if(isdigit(ch))
                {
                    pCrtCh++;
                    state = 7;
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
                else
                {
                    state = 8;
                }
                break;

            case 8:
                tk = addTk(CT_INT);
                tk->text = createString(pStartCh, pCrtCh);
                return pCrtCh - pStartCh + prec_spaces;

            case 9:
                if(ch >= '0' && ch <= '7')
                {
                    pCrtCh++;
                    state = 10;
                }
                else if(ch == 'x')
                {
                    pCrtCh++;
                    state = 11;
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
                if(ch == '*')
                {
                    pCrtCh++;
                    state = 14;
                }
                else
                {
                    cout<<"Found / without succeding *\n";
                    tkerr(addTk(END), "invalid character");
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
                tk = addTk(COMMENT);
                tk->text = createString(pStartCh, pCrtCh);
                return pCrtCh - pStartCh + prec_spaces;
        }
    }
}

void print_tokens()
{
    if (head == NULL)
    {
        cout<<"List empty!\n";
    }

    else
    {
        for(Token * c = head; c != NULL; c = c->next)
        {
            cout<<"("<<c->code<<", "<<c->text<<") ";
        }
    }
}

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

    char * filedata = (char *)malloc(filesize + 1);
    fread(filedata, 1, filesize, src);
    filedata[filesize] = 0;

    int start = 0;
    while (start < filesize)
    {
        cout<<"CALL\n";
        int r = getNextToken(filedata + start);
        if (r == 0)
            break;
        start += r;
    }

    print_tokens();
    return 0;
}
