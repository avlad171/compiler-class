//
// Created by Vlad on 26-May-22.
//

#ifndef CT_INSTRUCTION_H
#define CT_INSTRUCTION_H

#include <vector>

class Instr
{
public:
    int opcode;
    union
    {
        int i; // int, char
        double d;
        void *addr;
    } args[2];

    //ctors
    Instr(int op, void * a);
    Instr(int op, int i1 = 0, int i2 = 0);
};

int addInstr(std::vector<Instr> &v, Instr i);

#endif //CT_INSTRUCTION_H
