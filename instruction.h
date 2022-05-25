//
// Created by Vlad on 26-May-22.
//

#ifndef CT_INSTRUCTION_H
#define CT_INSTRUCTION_H

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

};
#endif //CT_INSTRUCTION_H
