//
// Created by Vlad on 25-May-22.
//

#include <iostream>
#include <vector>
#include <map>
#include <stdlib.h>

#include "vm.h"
#include "instruction.h"

using namespace std;

//context
#define STACK_SIZE (8 * 1024 * 1024)
char stack[STACK_SIZE];
char * SP;
char * stackAfter;
char * IP;
unsigned int VIP;
char * BP;

#define GLOBAL_SIZE (32 * 1024 * 1024)
char globals[GLOBAL_SIZE];
int nGlobals = 0;

//helpers
void * allocGlobal(int sz)
{
    void *p = globals + nGlobals;
    if(nGlobals + sz > GLOBAL_SIZE)
    {
        cout<<"Fatal error, global variables memory full, can't allocate anymore!\n";
        exit(-1);
    }
    nGlobals += sz;
    return p;
}

void pushd(double d)
{
    if(SP + sizeof(double) > stackAfter)
    {
        cout<<"Fatal error, stack overflow!"<<endl;
        exit(-1);
    }

    *(double *)SP = d;
    SP += sizeof (double);
}

void pusha(void * a)
{
    if(SP + sizeof(void *) > stackAfter)
    {
        cout<<"Fatal error, stack overflow!"<<endl;
        exit(-1);
    }

    *(void**)SP = a;
    SP += sizeof (void *);
}

void pushi(int i)
{
    if(SP + sizeof(int) > stackAfter)
    {
        cout<<"Fatal error, stack overflow!"<<endl;
        exit(-1);
    }

    *(int *)SP = i;
    SP += sizeof (int);
}

void pushc(char c)
{
    if(SP + sizeof(char) > stackAfter)
    {
        cout<<"Fatal error, stack overflow!"<<endl;
        exit(-1);
    }

    *(char *)SP = c;
    SP += sizeof (char);
}

double popd()
{
    SP -= sizeof (double);
    if (SP < stack)
    {
        cout<<"Fatal error, popping from empty stack!"<<endl;
        exit(-1);
    }

    return *(double *) SP;
}

void * popa()
{
    SP -= sizeof (void *);
    if (SP < stack)
    {
        cout<<"Fatal error, popping from empty stack!"<<endl;
        exit(-1);
    }

    return *(void **) SP;
}

int popi()
{
    SP -= sizeof (int);
    if (SP < stack)
    {
        cout<<"Fatal error, popping from empty stack!"<<endl;
        exit(-1);
    }

    return *(int *) SP;
}

char popc()
{
    SP -= sizeof (char);
    if (SP < stack)
    {
        cout<<"Fatal error, popping from empty stack!"<<endl;
        exit(-1);
    }

    return *(char *) SP;
}

void add_sp(int n)
{
    SP += n;
    if (SP > stackAfter)
    {
        cout<<"Fatal error, stack overflow (DROP)!"<<endl;
        exit(-1);
    }
}

void sub_sp(int n)
{
    SP -= n;
    if (SP < stack)
    {
        cout<<"Fatal error, popping from empty stack (DROP)!"<<endl;
        exit(-1);
    }
}

void f()
{
    cout<<"Vand Golf 4\n";
    return;
}

void put_i()
{
    cout<<"PUT I:\t";
    int i = popi();
    cout<<i<<"\n";
}

map <uint64_t, uint64_t> funcs;

//todo somehow remove global variables
int run(vector<Instr> & bytecode)
{
    //temp variables
    //int i1, i2;
    //double d1, d2;
    //char * addr;

    //init context
    SP = stack;
    VIP = 0;
    stackAfter = stack + STACK_SIZE;

    //VM main loop
    while(true)
    {
        cout << "[VM] " << to_hex_string(VIP) << "\t";
        switch(bytecode[VIP].opcode)
        {
            case ADD_I:
            {
                int i1, i2;
                i1 = popi();
                i2 = popi();
                cout << "ADD_I\t" << i1 << "+" << i2 << " -> " << (i1 + i2) << "\n";
                pushi(i1 + i2);
                VIP++;
            }
            break;

            case ADD_D:
            {
                double d1, d2;
                d1 = popd();
                d2 = popd();
                cout << "ADD_D\t" << d1 << "+" << d2 << " -> " << (d1 + d2) << "\n";
                pushd(d1 + d2);
                VIP++;
            }
            break;

            case ADD_C:
            {
                char c1, c2;
                c1 = popc();
                c2 = popc();
                cout << "ADD_C\t" << c1 << "+" << c2 << " -> " << (c1 + c2) << "\n";
                pushc(c1 + c2);
                VIP++;
            }
            break;

            case AND_A:
            {
                void * a1;
                void * a2;
                a1 = popa();
                a2 = popa();
                cout << "AND_A\t" << a1 << "-" << a2 << " -> " << (a1 && a2) << "\n";
                pushi(a1 && a2);
                VIP++;
            }
            break;

            case AND_C:
            {
                char c1, c2;
                c1 = popc();
                c2 = popc();
                cout << "AND_C\t" << c1 << "-" << c2 << " -> " << (c1 && c2) << "\n";
                pushi(c1 && c2);
                VIP++;
            }
            break;

            case AND_D:
            {
                double d1, d2;
                d1 = popd();
                d2 = popd();
                cout << "AND_D\t" << d1 << "-" << d2 << " -> " << (d1 && d2) << "\n";
                pushi(d1 && d2);
                VIP++;
            }
            break;

            case AND_I:
            {
                int i1, i2;
                i1 = popi();
                i2 = popi();
                cout << "AND_I\t" << i1 << "-" << i2 << " -> " << (i1 && i2) << "\n";
                pushi(i1 && i2);
                VIP++;
            }
            break;

            case CALL:
            {
                int callAddr = bytecode[VIP].args[0].i;
                int retAddr = VIP + 1;
                cout<<"CALL\t"<<callAddr<<"\n";
                pushi(retAddr);
                VIP = callAddr;
            }
            break;

            case CALLEXT:
            {
                cout<<"CALLEXT\t";
                uint64_t func_addr = reinterpret_cast<uint64_t>(bytecode[VIP].args[0].addr);
                void (*fun_ptr)() = reinterpret_cast<void (*)()>(funcs[func_addr]);
                (*fun_ptr)();

                VIP++;
            }
            break;

            case CAST_C_D:
            {
                cout<<"CAST char to double\n";
                char c = popc();
                double d = (double)c;
                pushd(d);
                VIP++;
            }
            break;

            case CAST_C_I:
            {
                cout<<"CAST char to int\n";
                char c = popc();
                int i = (int)c;
                pushi(i);
                VIP++;
            }
            break;

            case CAST_D_C:
            {
                cout<<"CAST double to char\n";
                double d = popd();
                char c = (char)d;
                pushc(c);
                VIP++;
            }
            break;

            case CAST_D_I:
            {
                cout<<"CAST double to int\n";
                double d = popd();
                int i = (int)d;
                pushi(i);
                VIP++;
            }
            break;

            case CAST_I_C:
            {
                cout<<"CAST int to char\n";
                int i = popi();
                char c = (char)i;
                pushc(c);
                VIP++;
            }
            break;

            case CAST_I_D:
            {
                cout<<"CAST int to double\n";
                int i = popi();
                double d = (double)i;
                pushd(d);
                VIP++;
            }
            break;

            case DIV_I:
            {
                int i1, i2;
                i2 = popi();
                i1 = popi();
                cout << "DIV_I\t" << i1 << "/" << i2 << " -> " << (i1 / i2) << "\n";
                pushi(i1 / i2);
                VIP++;
            }
            break;

            case DIV_D:
            {
                double d1, d2;
                d2 = popd();
                d1 = popd();
                cout << "DIV_D\t" << d1 << "/" << d2 << " -> " << (d1 / d2) << "\n";
                pushd(d1 / d2);
                VIP++;
            }
            break;

            case DIV_C:
            {
                char c1, c2;
                c2 = popc();
                c1 = popc();
                cout << "DIV_C\t" << c1 << "/" << c2 << " -> " << (c1 / c2) << "\n";
                pushc(c1 / c2);
                VIP++;
            }
            break;

            case DROP:
            {
                int n = bytecode[VIP].args[0].i;
                cout<<"DROP "<<n<<"\n";
                sub_sp(n);
                VIP++;
            }
            break;

            case ENTER:
            {
                int n = bytecode[VIP].args[0].i;
                cout<<"ENTER "<<n<<"\n";
                pusha(BP);
                BP = SP;
                add_sp(n);
                VIP++;
            }
            break;

            case EQ_I:
            {
                int i1, i2;
                i1 = popi();
                i2 = popi();
                cout << "EQ_I\t" << i1 << "-" << i2 << " -> " << (i1 == i2) << "\n";
                pushi(i1 == i2);
                VIP++;
            }
            break;

            case EQ_D:
            {
                double d1, d2;
                d1 = popd();
                d2 = popd();
                cout << "EQ_D\t" << d1 << "-" << d2 << " -> " << (d1 == d2) << "\n";
                pushi(d1 == d2);
                VIP++;
            }
            break;

            case EQ_C:
            {
                char c1, c2;
                c1 = popc();
                c2 = popc();
                cout << "EQ_C\t" << c1 << "-" << c2 << " -> " << (c1 == c2) << "\n";
                pushi(c1 == c2);
                VIP++;
            }
            break;

            case EQ_A:
            {
                void * a1;
                void * a2;
                a1 = popa();
                a2 = popa();
                cout << "EQ_A\t" << a1 << "-" << a2 << " -> " << (a1 == a2) << "\n";
                pushi(a1 == a2);
                VIP++;
            }
            break;

            case GREATER_I:
            {
                int i1, i2;
                i1 = popi();
                i2 = popi();
                cout << "GREATER_I\t" << i1 << "-" << i2 << " -> " << (i1 > i2) << "\n";
                pushi(i1 > i2);
                VIP++;
            }
            break;

            case GREATER_D:
            {
                double d1, d2;
                d1 = popd();
                d2 = popd();
                cout << "GREATER_D\t" << d1 << "-" << d2 << " -> " << (d1 > d2) << "\n";
                pushi(d1 > d2);
                VIP++;
            }
            break;

            case GREATER_C:
            {
                char c1, c2;
                c1 = popc();
                c2 = popc();
                cout << "GREATEREQ_C\t" << c1 << "-" << c2 << " -> " << (c1 > c2) << "\n";
                pushi(c1 > c2);
                VIP++;
            }
            break;

            case GREATEREQ_I:
            {
                int i1, i2;
                i1 = popi();
                i2 = popi();
                cout << "GREATEREQ_I\t" << i1 << "-" << i2 << " -> " << (i1 >= i2) << "\n";
                pushi(i1 >= i2);
                VIP++;
            }
            break;

            case GREATEREQ_D:
            {
                double d1, d2;
                d1 = popd();
                d2 = popd();
                cout << "GREATEREQ_D\t" << d1 << "-" << d2 << " -> " << (d1 >= d2) << "\n";
                pushi(d1 >= d2);
                VIP++;
            }
            break;

            case GREATEREQ_C:
            {
                char c1, c2;
                c1 = popc();
                c2 = popc();
                cout << "GREATEREQ_C\t" << c1 << "-" << c2 << " -> " << (c1 >= c2) << "\n";
                pushi(c1 >= c2);
                VIP++;
            }
            break;

            case HALT:
            {
                cout<<"HALT\n";
                return 0;
            }
            break;

            case INSERT:
            {
                int i1 = bytecode[VIP].args[0].i; // iDst
                int i2 = bytecode[VIP].args[1].i; // nBytes
                cout<<"INSERT\t"<<i1<<", "<<i2<<"\n";

                if(SP + i2 > stackAfter)
                {
                    cout<<"Fatal error, stack overflow at insert\n";
                    exit(-1);
                }
                memmove(SP- i1 + i2,SP - i1,i1); //make room
                memmove(SP - i1,SP + i2,i2); //dup
                SP += i2;

                VIP++;
            }
            break;

            case JF_A:
            {
                int i = bytecode[VIP].args[0].i;
                cout<<"JF_A "<<i<<"\n";
                void * a = popa();
                if (!a)
                {
                    VIP = i;
                }
                else
                {
                    VIP++;
                }
            }
            break;

            case JF_C:
            {
                int i = bytecode[VIP].args[0].i;
                cout<<"JF_C "<<i<<"\n";
                char c = popc();
                if (!c)
                {
                    VIP = i;
                }
                else
                {
                    VIP++;
                }
            }
            break;

            case JF_D:
            {
                int i = bytecode[VIP].args[0].i;
                cout<<"JF_D "<<i<<"\n";
                double d = popd();
                if (!d)
                {
                    VIP = i;
                }
                else
                {
                    VIP++;
                }
            }
            break;

            case JF_I:
            {
                int i = bytecode[VIP].args[0].i;
                cout<<"JF_I "<<i<<"\n";
                char i2 = popi();
                if (!i2)
                {
                    VIP = i;
                }
                else
                {
                    VIP++;
                }
            }
            break;

            case JMP:
            {
                int i = bytecode[VIP].args[0].i;
                cout<<"JMP "<<i<<"\n";
                VIP = i;
            }
            break;

            case JT_A:
            {
                int i = bytecode[VIP].args[0].i;
                cout<<"JF_A "<<i<<"\n";
                void * a = popa();
                if (a)
                {
                    VIP = i;
                }
                else
                {
                    VIP++;
                }
            }
                break;

            case JT_C:
            {
                int i = bytecode[VIP].args[0].i;
                cout<<"JF_C "<<i<<"\n";
                char c = popc();
                if (c)
                {
                    VIP = i;
                }
                else
                {
                    VIP++;
                }
            }
                break;

            case JT_D:
            {
                int i = bytecode[VIP].args[0].i;
                cout<<"JF_D "<<i<<"\n";
                double d = popd();
                if (d)
                {
                    VIP = i;
                }
                else
                {
                    VIP++;
                }
            }
                break;

            case JT_I:
            {
                int i = bytecode[VIP].args[0].i;
                cout<<"JF_I "<<i<<"\n";
                char i2 = popi();
                if (i2)
                {
                    VIP = i;
                }
                else
                {
                    VIP++;
                }
            }
            break;

            case LESS_I:
            {
                int i1, i2;
                i1 = popi();
                i2 = popi();
                cout << "LESS_I\t" << i1 << "-" << i2 << " -> " << (i1 < i2) << "\n";
                pushi(i1 < i2);
                VIP++;
            }
            break;

            case LESS_D:
            {
                double d1, d2;
                d1 = popd();
                d2 = popd();
                cout << "LESS_D\t" << d1 << "-" << d2 << " -> " << (d1 < d2) << "\n";
                pushi(d1 < d2);
                VIP++;
            }
            break;

            case LESS_C:
            {
                char c1, c2;
                c1 = popc();
                c2 = popc();
                cout << "GREATEREQ_C\t" << c1 << "-" << c2 << " -> " << (c1 < c2) << "\n";
                pushi(c1 < c2);
                VIP++;
            }
            break;

            case LESSEQ_I:
            {
                int i1, i2;
                i1 = popi();
                i2 = popi();
                cout << "LESSEQ_I\t" << i1 << "-" << i2 << " -> " << (i1 <= i2) << "\n";
                pushi(i1 <= i2);
                VIP++;
            }
            break;

            case LESSEQ_D:
            {
                double d1, d2;
                d1 = popd();
                d2 = popd();
                cout << "LESSEQ_D\t" << d1 << "-" << d2 << " -> " << (d1 <= d2) << "\n";
                pushi(d1 <= d2);
                VIP++;
            }
            break;

            case LESSEQ_C:
            {
                char c1, c2;
                c1 = popc();
                c2 = popc();
                cout << "GREATEREQ_C\t" << c1 << "-" << c2 << " -> " << (c1 <= c2) << "\n";
                pushi(c1 <= c2);
                VIP++;
            }
            break;

            case MUL_I:
            {
                int i1, i2;
                i1 = popi();
                i2 = popi();
                cout << "MUL_I\t" << i1 << "*" << i2 << " -> " << (i1 * i2) << "\n";
                pushi(i1 * i2);
                VIP++;
            }
            break;

            case MUL_D:
            {
                double d1, d2;
                d1 = popd();
                d2 = popd();
                cout << "MUL_D\t" << d1 << "*" << d2 << " -> " << (d1 * d2) << "\n";
                pushd(d1 * d2);
                VIP++;
            }
            break;

            case MUL_C:
            {
                char c1, c2;
                c1 = popc();
                c2 = popc();
                cout << "MUL_C\t" << c1 << "*" << c2 << " -> " << (c1 * c2) << "\n";
                pushc(c1 * c2);
                VIP++;
            }
            break;

            case NEG_C:
            {
                char c1 = popc();
                char c2 = -c1;
                cout<<"NEG_C\t"<<c1<<" -> "<<c2<<"\n";
                pushc(c2);
                VIP++;
            }
            break;

            case NEG_D:
            {
                double d1 = popd();
                double d2 = -d1;
                cout<<"NEG_D\t"<<d1<<" -> "<<d2<<"\n";
                pushd(d2);
                VIP++;
            }
            break;

            case NEG_I:
            {
                int i1 = popc();
                int i2 = -i1;
                cout<<"NEG_I\t"<<i1<<" -> "<<i2<<"\n";
                pushi(i2);
                VIP++;
            }
            break;

            case NOP:
            {
                cout<<"NOP\n";
                VIP++;
            }
            break;

            case NOT_C:
            {
                char c1 = popc();
                int c2 = !c1;
                cout<<"NOT_C\t"<<c1<<" -> "<<c2<<"\n";
                pushi(c2);
                VIP++;
            }
            break;

            case NOT_D:
            {
                double d1 = popd();
                int d2 = !d1;
                cout<<"NOT_D\t"<<d1<<" -> "<<d2<<"\n";
                pushi(d2);
                VIP++;
            }
            break;

            case NOT_I:
            {
                int i1 = popc();
                int i2 = !i1;
                cout<<"NEG_I\t"<<i1<<" -> "<<i2<<"\n";
                pushi(i2);
                VIP++;
            }
            break;

            case NOT_A:
            {
                void *a1 = popa();
                int a2 = !a1;
                cout<<"NEG_A\t"<<a1<<" -> "<<a2<<"\n";
                pushi(a2);
                VIP++;
            }
            break;

            case NOTEQ_I:
            {
                int i1, i2;
                i1 = popi();
                i2 = popi();
                cout << "NOTEQ_I\t" << i1 << "-" << i2 << " -> " << (i1 != i2) << "\n";
                pushi(i1 != i2);
                VIP++;
            }
            break;

            case NOTEQ_D:
            {
                double d1, d2;
                d1 = popd();
                d2 = popd();
                cout << "EQ_D\t" << d1 << "-" << d2 << " -> " << (d1 != d2) << "\n";
                pushi(d1 != d2);
                VIP++;
            }
            break;

            case NOTEQ_C:
            {
                char c1, c2;
                c1 = popc();
                c2 = popc();
                cout << "NOTEQ_C\t" << c1 << "-" << c2 << " -> " << (c1 != c2) << "\n";
                pushi(c1 != c2);
                VIP++;
            }
            break;

            case NOTEQ_A:
            {
                void * a1;
                void * a2;
                a1 = popa();
                a2 = popa();
                cout << "NOTEQ_A\t" << a1 << "-" << a2 << " -> " << (a1 != a2) << "\n";
                pushi(a1 != a2);
                VIP++;
            }
            break;

            case OFFSET:
            {
                int offset = bytecode[VIP].args[0].i;
                void * a = popa();
                int64_t a_temp = (int64_t)a;
                a_temp += offset;
                a = (void *) a_temp;
                cout << "OFFSET\t" << a << " + " << offset << " -> " << (to_hex_string((uint64_t)a)) << "\n";
                pusha(a);
                VIP++;
            }
            break;

            case OR_A:
            {
                void * a1;
                void * a2;
                a1 = popa();
                a2 = popa();
                cout << "OR_A\t" << a1 << "-" << a2 << " -> " << (a1 || a2) << "\n";
                pushi(a1 || a2);
                VIP++;
            }
            break;

            case OR_C:
            {
                char c1, c2;
                c1 = popc();
                c2 = popc();
                cout << "OR_C\t" << c1 << "-" << c2 << " -> " << (c1 || c2) << "\n";
                pushi(c1 || c2);
                VIP++;
            }
            break;

            case OR_D:
            {
                double d1, d2;
                d1 = popd();
                d2 = popd();
                cout << "OR_D\t" << d1 << "-" << d2 << " -> " << (d1 || d2) << "\n";
                pushi(d1 || d2);
                VIP++;
            }
            break;

            case OR_I:
            {
                int i1, i2;
                i1 = popi();
                i2 = popi();
                cout << "AND_I\t" << i1 << "-" << i2 << " -> " << (i1 || i2) << "\n";
                pushi(i1 || i2);
                VIP++;
            }
            break;

            case PUSHFPADDR:
            {
                int offset = bytecode[VIP].args[0].i;
                cout << "PUSHFPADDR\t" << to_hex_string(reinterpret_cast<uint64_t>(BP)) << " - " << offset << " -> " << (BP + offset) << "\n";
                pusha(BP + offset);
                VIP++;
            }
            break;

            case PUSHCT_A:
            {
                void * a = bytecode[VIP].args[0].addr;
                cout<<"PUSHCT_A\t"<<to_hex_string((uint64_t)a)<<"\n";
                pusha(a);
                VIP++;
            }
            break;

            case PUSHCT_C:
            {
                char c = bytecode[VIP].args[0].i & 0xFF;
                cout<<"PUSHCT_C\t"<<c<<"\n";
                pushc(c);
                VIP++;
            }
            break;

            case PUSHCT_I:
            {
                int i = bytecode[VIP].args[0].i;
                cout<<"PUSHCT_I\t"<<i<<"\n";
                pushi(i);
                VIP++;
            }
            break;

            case PUSHCT_D:
            {
                double d = bytecode[VIP].args[0].d;
                cout<<"PUSHCT_D\t"<<d<<"\n";
                pushd(d);
                VIP++;
            }
            break;

            case RET:
            {
                int sizeArgs = bytecode[VIP].args[0].i;
                int sizeofRetType = bytecode[VIP].args[1].i;
                cout<<"RET\t"<<sizeArgs<<", "<<sizeofRetType<<"\n";
                char * oldSP = SP;
                SP=BP;
                BP = (char *)popa();
                VIP = popi();
                if(SP - sizeArgs < stack)
                {
                    cout<<"Fatal error, emptied stack before returning!\n";
                    exit(-1);
                }

                SP -= sizeArgs;
                memmove(SP,oldSP - sizeofRetType,sizeofRetType);
                SP += sizeofRetType;
            }
            break;

            case LOAD:
            {
                int i = bytecode[VIP].args[0].i;
                void * a = popa();
                cout<<"LOAD\t"<<i<<"\t"<<to_hex_string((uint64_t)a)<<"\n";
                if(SP + i > stackAfter)
                {
                    cout<<"Fatal error, stack overflow on LOAD\n";
                    exit(-1);
                }
                memcpy(SP,a,i);
                SP += i;

                VIP++;
            }
            break;

            case STORE:
            {
                int i = bytecode[VIP].args[0].i;
                if(SP - (sizeof(void*) + i) < stack)
                {
                    cout<<"Fatal error, not enough stack bytes for SET!\n";
                    exit(-1);
                }

                void * a = *(void**)(SP - ((sizeof(void*) + i)));
                cout<<"STORE\t"<<i<<"\t"<<to_hex_string((uint64_t)a)<<"\n";
                memcpy(a,SP - i,i);
                SP -= sizeof(void*) + i;
                VIP++;
            }
            break;

            case SUB_I:
            {
                int i1, i2;
                i2 = popi();
                i1 = popi();
                cout << "SUB_I\t" << i1 << "-" << i2 << " -> " << (i1 - i2) << "\n";
                pushi(i1 - i2);
                VIP++;
            }
            break;

            case SUB_D:
            {
                double d1, d2;
                d2 = popd();
                d1 = popd();
                cout << "SUB_D\t" << d1 << "-" << d2 << " -> " << (d1 - d2) << "\n";
                pushd(d1 - d2);
                VIP++;
            }
            break;

            case SUB_C:
            {
                char c1, c2;
                c2 = popc();
                c1 = popc();
                cout << "SUB_C\t" << c1 << "-" << c2 << " -> " << (c1 - c2) << "\n";
                pushc(c1 - c2);
                VIP++;
            }
            break;
        }
    }
    return 0;
}


int main()
{
    cout<<"Hello from the VM!\n";

    funcs[0x10000] = (uint64_t)f;
    funcs[0x20000] = (uint64_t)put_i;

    cout << "Address of put_i is " << to_hex_string(reinterpret_cast<uint64_t>(put_i)) << "\n";
    vector<Instr> bytecode;

    int *v = (int*)allocGlobal(sizeof(int));

    addInstr(bytecode, Instr (PUSHCT_A, v));
    addInstr(bytecode, Instr (PUSHCT_I, 3));
    addInstr(bytecode, Instr (STORE, sizeof(int)));
    int L1 = addInstr(bytecode, Instr (PUSHCT_A, v));
    addInstr(bytecode, Instr (LOAD, sizeof(int)));
    addInstr(bytecode, Instr(CALLEXT, (void *)0x20000));
    addInstr(bytecode, Instr (PUSHCT_A, v));
    addInstr(bytecode, Instr (PUSHCT_A, v));
    addInstr(bytecode, Instr (LOAD, sizeof(int)));
    addInstr(bytecode, Instr (PUSHCT_I, 1));
    addInstr(bytecode, Instr (SUB_I));
    addInstr(bytecode, Instr (STORE, sizeof(int)));
    addInstr(bytecode, Instr (PUSHCT_A, v));
    addInstr(bytecode, Instr (LOAD, sizeof(int)));
    addInstr(bytecode, Instr (JT_I, L1));
    addInstr(bytecode, Instr (HALT));
    run(bytecode);
    return 0;
}