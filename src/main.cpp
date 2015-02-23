#include <iostream>
#include <vector>
#include "MP_8085.h"
#include "_8085InstructionParser.h"






int main()
{
    _8085InstructionParser parser;
    //parser.ParseLine("MOV A, D");
    //parser.ParseLine("MOV B,A ");
    MPSystem_8085 mp;
    mp.Test(parser.ParseFile("test.asm"));
    std::cin.get();
    return 0;
}

