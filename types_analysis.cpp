//
// Created by Vlad on 19-May-22.
//

#include "symbol_table.h"
#include "semantic_analysis.h"

using namespace std;

void cast(const Type & dst, const Type &src)
{
    if(src.nElements > -1)
    {
        if(dst.nElements > -1)
        {
            if(src.typeBase != dst.typeBase)
            {
                cout << "An array cannot be converted to an array of another type!\n";
                exit(0);
            }
        }
        else
        {
            cout<<"An array cannot be converted to a non-array!\n";
            exit(0);
        }
    }
    else
    {
        if(dst.nElements > -1)
        {
            cout<<"A non-array cannot be converted to an array!\n";
        }
    }

    switch(src.typeBase)
    {
        case TB_CHAR:
        case TB_INT:
        case TB_DOUBLE:
            switch(dst.typeBase)
            {
                case TB_CHAR:
                case TB_INT:
                case TB_DOUBLE:
                    return;
            }
        case TB_STRUCT:
            if(dst.typeBase==TB_STRUCT){
                if(src.struct_type != dst.struct_type)
                {
                    cout << "A structure cannot be converted to another one!\n";
                    exit(0);
                }
                return;
            }
    }
    cout<<"Cast failed! Incompatible types!\n";
    exit(0);
}

Type getArithType(const Type & t1, const Type & t2)
{
    //for now
    return t2;
}