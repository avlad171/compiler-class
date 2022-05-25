//
// Created by Vlad on 25-May-22.
//

#ifndef CT_VM_H
#define CT_VM_H

#include <string>

//the vm operations
enum
{
    ADD_C, ADD_D, ADD_I,
    AND_A, AND_C, AND_D, AND_I,
    CALL, CALLEXT,
    CAST_C_D, CAST_C_I, CAST_D_C, CAST_D_I, CAST_I_C, CAST_I_D,
    DIV_C, DIV_D, DIV_I,
    DROP,
    ENTER,
    EQ_A, EQ_C, EQ_D, EQ_I,
    GREATER_C, GREATER_D, GREATER_I,
    GREATEREQ_C, GREATEREQ_D, GREATEREQ_I,
    HALT,
    INSERT,
    JF_A, JF_C, JF_D, JF_I,
    JMP,
    JT_A, JT_C, JT_D, JT_I,
    LESS_C, LESS_D, LESS_I,
    LESSEQ_C, LESSEQ_D, LESSEQ_I,
    LOAD,
    MUL_C, MUL_D, MUL_I,
    NEG_C, NEG_D, NEG_I,
    NOP,
    NOT_A, NOT_C, NOT_D, NOT_I,
    NOTEQ_A, NOTEQ_C, NOTEQ_D, NOTEQ_I,
    OFFSET,
    OR_A, OR_C, OR_D, OR_I,
    PUSHFPADDR,
    PUSHCT_A, PUSHCT_C, PUSHCT_D, PUSHCT_I,
    RET,
    STORE,
    SUB_C, SUB_D, SUB_I
};

std::string to_hex_string(uint64_t x);
#endif //CT_VM_H
