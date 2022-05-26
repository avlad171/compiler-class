//
// Created by Vlad on 26-May-22.
//

#include "instruction.h"
#include "vm.h"

using namespace std;

Instr::Instr(int op, void * a)
{
    this->opcode = op;
    this->args[0].addr = a;
    this->args[1].addr = nullptr;
}

Instr::Instr(int op, int i1, int i2)
{
    {
        this->opcode = op;
        this->args[0].i = i1;
        this->args[1].i = i2;
    }
}

int addInstr(vector<Instr> & v, Instr i)
{
    int pos = v.size();
    v.push_back(i);
    return pos;
}