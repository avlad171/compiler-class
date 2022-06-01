//
// Created by Vlad on 26-May-22.
//

#ifndef CT_INSTRUCTION_H
#define CT_INSTRUCTION_H

#include <vector>
#include <iostream>
#include "vm.h"

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

std::ostream& operator<<(std::ostream& os, const Instr & ins);
#endif //CT_INSTRUCTION_H
