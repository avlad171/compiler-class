//
// Created by Vlad on 26-May-22.
//

#include "vm.h"

using namespace std;

string to_hex_string(uint64_t x)
{
    string ans = "0x";

    bool reachedNonzeroBit = false;

    for(int nibble_id = 15; nibble_id >= 0; --nibble_id)
    {
        uint64_t mask = 0xF;
        mask = mask << (4 * nibble_id);

        uint64_t extractedDigit = x & mask;
        extractedDigit = extractedDigit >> (4 * nibble_id);

        if(extractedDigit)
            reachedNonzeroBit = true;

        if(!reachedNonzeroBit)
            continue;

        if(extractedDigit < 10)
            ans += (char)(extractedDigit + 48);

        else
            ans += (char)(extractedDigit + 55);
    }

    if(!reachedNonzeroBit)
        ans += '0';

    return ans;
}