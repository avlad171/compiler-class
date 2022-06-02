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

std::ostream& operator<<(std::ostream& os, const Instr & ins)
{
    switch(ins.opcode)
    {
        case ADD_C:
        {
            os<<"ADD_C not done yet!\n";
        }
            break;


        case ADD_D:
        {
            os<<"ADD_D not done yet!\n";
        }
            break;


        case ADD_I:
        {
            os<<"ADD_I not done yet!\n";
        }
            break;


        case AND_A:
        {
            os<<"AND_A not done yet!\n";
        }
            break;


        case AND_C:
        {
            os<<"AND_C not done yet!\n";
        }
            break;


        case AND_D:
        {
            os<<"AND_D not done yet!\n";
        }
            break;


        case AND_I:
        {
            os<<"AND_I not done yet!\n";
        }
            break;


        case CALL:
        {
            int callAddr = ins.args[0].i;
            os<<"CALL\t"<<callAddr<<"\n";
        }
        break;


        case CALLEXT:
        {
            os<<"CALLEXT\t"<<to_hex_string(reinterpret_cast<uint64_t>(ins.args[0].addr))<<"\n";
        }
        break;


        case CAST_C_D:
        {
            os<<"CAST_C_D not done yet!\n";
        }
            break;


        case CAST_C_I:
        {
            os<<"CAST_C_I not done yet!\n";
        }
        break;


        case CAST_D_C:
        {
            os<<"CAST_D_C not done yet!\n";
        }
            break;


        case CAST_D_I:
        {
            os<<"CAST_D_I not done yet!\n";
        }
            break;


        case CAST_I_C:
        {
            os<<"CAST_I_C not done yet!\n";
        }
            break;


        case CAST_I_D:
        {
            os<<"CAST_I_D not done yet!\n";
        }
            break;


        case DIV_C:
        {
            os<<"DIV_C not done yet!\n";
        }
            break;


        case DIV_D:
        {
            os<<"DIV_D not done yet!\n";
        }
            break;


        case DIV_I:
        {
            os<<"DIV_I not done yet!\n";
        }
            break;


        case DROP:
        {
            os<<"DROP not done yet!\n";
        }
            break;


        case ENTER:
        {
            int n = ins.args[0].i;
            os<<"ENTER "<<n<<"\n";
        }
        break;


        case EQ_A:
        {
            os<<"EQ_A not done yet!\n";
        }
            break;


        case EQ_C:
        {
            os<<"EQ_C not done yet!\n";
        }
            break;


        case EQ_D:
        {
            os<<"EQ_D not done yet!\n";
        }
            break;


        case EQ_I:
        {
            os<<"EQ_I not done yet!\n";
        }
            break;


        case GREATER_C:
        {
            os<<"GREATER_C not done yet!\n";
        }
            break;


        case GREATER_D:
        {
            os<<"GREATER_D not done yet!\n";
        }
            break;


        case GREATER_I:
        {
            os<<"GREATER_I\n";
        }
        break;


        case GREATEREQ_C:
        {
            os<<"GREATEREQ_C not done yet!\n";
        }
            break;


        case GREATEREQ_D:
        {
            os<<"GREATEREQ_D not done yet!\n";
        }
            break;


        case GREATEREQ_I:
        {
            os<<"GREATEREQ_I not done yet!\n";
        }
            break;


        case HALT:
        {
            os<<"HALT\n";
        }
            break;


        case INSERT:
        {
            os<<"INSERT not done yet!\n";
        }
            break;


        case JF_A:
        {
            os<<"JF_A not done yet!\n";
        }
            break;


        case JF_C:
        {
            os<<"JF_C not done yet!\n";
        }
            break;


        case JF_D:
        {
            os<<"JF_D not done yet!\n";
        }
            break;


        case JF_I:
        {
            int i = ins.args[0].i;
            os<<"JF_I "<<i<<"\n";
        }
        break;


        case JMP:
        {
            int i = ins.args[0].i;
            os<<"JMP "<<i<<"\n";
        }
        break;


        case JT_A:
        {
            os<<"JT_A not done yet!\n";
        }
            break;


        case JT_C:
        {
            os<<"JT_C not done yet!\n";
        }
            break;


        case JT_D:
        {
            os<<"JT_D not done yet!\n";
        }
            break;


        case JT_I:
        {
            os<<"JT_I not done yet!\n";
        }
            break;


        case LESS_C:
        {
            os<<"LESS_C not done yet!\n";
        }
            break;


        case LESS_D:
        {
            os<<"LESS_D not done yet!\n";
        }
            break;


        case LESS_I:
        {
            os<<"LESS_I not done yet!\n";
        }
            break;


        case LESSEQ_C:
        {
            os<<"LESSEQ_C not done yet!\n";
        }
            break;


        case LESSEQ_D:
        {
            os<<"LESSEQ_D not done yet!\n";
        }
            break;


        case LESSEQ_I:
        {
            os<<"LESSEQ_I not done yet!\n";
        }
            break;


        case LOAD:
        {
            int i = ins.args[0].i;
            os<<"LOAD\t"<<i<<"\n";
        }
        break;


        case MUL_C:
        {
            os<<"MUL_C not done yet!\n";
        }
            break;


        case MUL_D:
        {
            os<<"MUL_D not done yet!\n";
        }
            break;


        case MUL_I:
        {
            os<<"MUL_I not done yet!\n";
        }
            break;


        case NEG_C:
        {
            os<<"NEG_C not done yet!\n";
        }
            break;


        case NEG_D:
        {
            os<<"NEG_D not done yet!\n";
        }
            break;


        case NEG_I:
        {
            os<<"NEG_I not done yet!\n";
        }
            break;


        case NOP:
        {
            os<<"NOP\n";
        }
        break;


        case NOT_A:
        {
            os<<"NOT_A not done yet!\n";
        }
            break;


        case NOT_C:
        {
            os<<"NOT_C not done yet!\n";
        }
            break;


        case NOT_D:
        {
            os<<"NOT_D not done yet!\n";
        }
            break;


        case NOT_I:
        {
            os<<"NOT_I not done yet!\n";
        }
            break;


        case NOTEQ_A:
        {
            os<<"NOTEQ_A not done yet!\n";
        }
            break;


        case NOTEQ_C:
        {
            os<<"NOTEQ_C not done yet!\n";
        }
            break;


        case NOTEQ_D:
        {
            os<<"NOTEQ_D not done yet!\n";
        }
            break;


        case NOTEQ_I:
        {
            os<<"NOTEQ_I not done yet!\n";
        }
            break;


        case OFFSET:
        {
            os<<"OFFSET not done yet!\n";
        }
            break;


        case OR_A:
        {
            os<<"OR_A not done yet!\n";
        }
            break;


        case OR_C:
        {
            os<<"OR_C not done yet!\n";
        }
            break;


        case OR_D:
        {
            os<<"OR_D not done yet!\n";
        }
            break;


        case OR_I:
        {
            os<<"OR_I not done yet!\n";
        }
            break;


        case PUSHFPADDR:
        {
            os<<"PUSHFPADDR not done yet!\n";
        }
            break;


        case PUSHCT_A:
        {
            void * a = ins.args[0].addr;
            os<<"PUSHCT_A\t"<<to_hex_string((uint64_t)a)<<"\n";
        }
        break;


        case PUSHCT_C:
        {
            char c = ins.args[0].i & 0xFF;
            os<<"PUSHCT_C\t"<<c<<"\n";
        }
        break;


        case PUSHCT_D:
        {
            os<<"PUSHCT_D not done yet!\n";
        }
            break;


        case PUSHCT_I:
        {
            int i = ins.args[0].i;
            os<<"PUSHCT_I\t"<<i<<"\n";
        }
        break;


        case RET:
        {
            int sizeArgs = ins.args[0].i;
            int sizeofRetType = ins.args[1].i;
            os<<"RET\t"<<sizeArgs<<", "<<sizeofRetType<<"\n";
        }
        break;


        case STORE:
        {
            int i = ins.args[0].i;
            os<<"STORE\t"<<i<<"\n";
        }
        break;


        case SUB_C:
        {
            os<<"SUB_C not done yet!\n";
        }
            break;


        case SUB_D:
        {
            os<<"SUB_D not done yet!\n";
        }
            break;


        case SUB_I:
        {
            os<<"SUB_I not done yet!\n";
        }
            break;


    }
    return os;
}