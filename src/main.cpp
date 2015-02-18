#include <iostream>
#include <vector>
#include "MP_8085.h"


#include <string>
#include <sstream>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <fstream>
#include <map>

class _8085InstructionParser
{
public:
    std::map<std::string, unsigned char> m_instructionbase;
    std::map<std::string, unsigned char> m_regaddr;
    std::map<std::string, unsigned char> m_regpairaddr;

    std::vector<unsigned char> m_program;


    _8085InstructionParser()
    {
        m_instructionbase["MOV"] = 0x40;
        m_instructionbase["MVI"] = 0x06;

        m_regaddr["A"] = 0x07;
        m_regaddr["B"] = 0x00;
        m_regaddr["C"] = 0x01;
        m_regaddr["D"] = 0x02;
        m_regaddr["E"] = 0x03;
        m_regaddr["H"] = 0x04;
        m_regaddr["L"] = 0x05;
        m_regaddr["M"] = 0x06;
    }

    std::string trim( std::string &s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && isspace(*it))
            it++;
        std::string::const_reverse_iterator rit = s.rbegin();
        while (rit.base() != it && isspace(*rit))
            rit++;
        s = std::string(it, rit.base());
        return s;
    }

    void ParseLine(std::string line)
    {
        this->trim(line);
        std::istringstream iss(line);
        std::string instruction;
        unsigned char encodedInstruction = 0;
        if (std::getline(iss, instruction, ' '))
        {
            if(m_instructionbase.find(instruction) != m_instructionbase.end())
            {
                encodedInstruction = m_instructionbase[instruction];
                int tokens = 0;

                std::string firstToken;
                std::string secondToken;

                if (std::getline(iss, firstToken, ','))
                {
                    ++tokens;
                    this->trim(firstToken);
                    if (std::getline(iss, secondToken, '\0'))
                    {
                        this->trim(secondToken);
                        ++tokens;
                    }
                }
                if (instruction == "MOV")
                {
                    encodedInstruction = encodedInstruction | (m_regaddr[firstToken] << 3);
                    encodedInstruction = encodedInstruction | (m_regaddr[secondToken]);
                    m_program.push_back(encodedInstruction);
                    std::cout << "Encoded instruction = " << std::hex << static_cast<int>(encodedInstruction) << std::endl;
                }
                else if (instruction == "MVI")
                {
                    encodedInstruction = encodedInstruction | (m_regaddr[firstToken] << 3);
                    m_program.push_back(encodedInstruction);
                    unsigned long data = std::strtol(firstToken.c_str(), 0, 16);
                    if (data > 255)
                    {
                        std::cout << "\"MVI\": data out of range (truncating to 1 byte)" << std::endl;
                    }
                    else
                    {
                        m_program.push_back(static_cast<unsigned char>(data));
                    }
                    //m_program.push_back(firstToken);
                    std::cout << "Encoded instruction = " << std::hex << static_cast<int>(encodedInstruction) 
                        << ", Data = " << secondToken << std::endl;
                }



                
            }
            else
            {
                std::cout << "Unknown instruction: \"" << instruction << "\"" << std::endl;
            }


            
        }
    }
    std::vector<unsigned char> ParseFile(std::string filename)
    {
        std::ifstream infile(filename);
        std::string line;
        while (std::getline(infile, line))
        {
            this->ParseLine(line);
        }
        return m_program;
    }
};


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

