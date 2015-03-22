#pragma once

#include <string>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <fstream>
#include <map>
#include <vector>

#include <iostream>

enum TokenType_8085{INSTRUCTION, REGISTER, DATA, COMMENT, ERROR, JUMPLABEL, NORMALTEXT};

class Token
{
public:
    int Type(){ return m_type; }
    Token() :m_type(-1){}
    void Set(int type, std::string data, int startOffset, int endOffset)
    {
        m_type = type;
        m_data = data;
        m_startOffset = startOffset;
        m_endOffset = endOffset;
    }
    Token(int type, std::string data, int startOffset, int endOffset)
    {
        this->Set(type, data, startOffset, endOffset);
    }
    int StartOffset(){ return m_startOffset; }
    int EndOffset(){ return m_endOffset; }
protected:
    std::string m_data;
    int m_type;
    int m_startOffset, m_endOffset; // location in current line
};

class InstructionParser_8085
{
public:
    std::map<std::string, unsigned char> m_instructionbase;
    std::map<std::string, unsigned char> m_regaddr;
    std::map<std::string, unsigned char> m_regpairaddr;

    std::vector<unsigned char> m_program;

    InstructionParser_8085()
    {
        m_instructionbase["ADD"] = 0x80;
        m_instructionbase["ADC"] = 0x88;

        m_instructionbase["MOV"] = 0x40;
        m_instructionbase["MVI"] = 0x06;
        m_instructionbase["LXI"] = 0x01;
        m_instructionbase["STA"] = 0x00;
        m_instructionbase["LDA"] = 0x00;
        m_instructionbase["SHLD"] = 0x00;
        m_instructionbase["LHLD"] = 0x00;
        m_instructionbase["STAX"] = 0x00;
        m_instructionbase["LDAX"] = 0x00;
        m_instructionbase["JNZ"] = 0xc2;
        m_instructionbase["JZ"] = 0xcA;
        m_instructionbase["HLT"] = 0x76;
        

        m_regaddr["A"] = 0x07;
        m_regaddr["B"] = 0x00;
        m_regaddr["C"] = 0x01;
        m_regaddr["D"] = 0x02;
        m_regaddr["E"] = 0x03;
        m_regaddr["H"] = 0x04;
        m_regaddr["L"] = 0x05;
        m_regaddr["M"] = 0x06;

        m_regpairaddr["B"] = 0x00;
        m_regpairaddr["D"] = 0x01;
        m_regpairaddr["H"] = 0x02;
        m_regpairaddr["SP"] = 0x03;
    }

    std::vector<Token> GetTokens(std::string line)
    {
        std::vector<Token> tokens;
        //this->trim(line);
        std::istringstream iss(line);
        std::string token;
        int currentOffset = iss.tellg();
        bool instructionFound = false;
        int nTokens = 0;
        while (std::getline(iss, token, ' '))
        {
            if (m_instructionbase.find(token) != m_instructionbase.end())
            {
                if (instructionFound == false)
                {
                    tokens.push_back(Token(INSTRUCTION, token, currentOffset, currentOffset + token.length()));
                    instructionFound = true;
                    ++nTokens;
                }
                else
                {
                    tokens.push_back(Token(ERROR, token, currentOffset, currentOffset + token.length()));
                }
            }
            else
            {

                if (instructionFound && ((token == "A" || token == "B" || token == "C" || token == "D" || token == "E" || token == "H" || token == "L")
                    || (token == "A," || token == "B," || token == "C," || token == "D," || token == "E," || token == "H," || token == "L,")))
                {
                    tokens.push_back(Token(REGISTER, token, currentOffset, currentOffset + token.length()));
                    ++nTokens;
                }
                else if (token != "" && token != " " && token != "\n" && token != "\n\r")
                {
                    ++nTokens;
                    tokens.push_back(Token(NORMALTEXT, token, currentOffset, currentOffset + token.length()));
                }
                //else tokens.push_back(Token(NORMALTEXT, token, currentOffset, currentOffset + token.length()));
                //std::strtol(token.c_str(), 0, 16);
            }
            currentOffset = iss.tellg();
            
        }
        return tokens;
    }

    std::string trim(std::string &s)
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
            if (m_instructionbase.find(instruction) != m_instructionbase.end())
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
                if (tokens == 2)
                {
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
                        unsigned long data = std::strtol(secondToken.c_str(), 0, 16);
                        if (data > 255)
                        {
                            std::cout << "\"MVI\": data out of range (truncating to 1 byte)" << std::endl;
                        }
                        else
                        {
                            m_program.push_back(static_cast<unsigned char>(data));
                        }
                        std::cout << "Encoded instruction = " << std::hex << static_cast<int>(encodedInstruction)
                            << ", Data = " << secondToken << std::endl;
                    }
                }
                else if (tokens == 1)
                {
                    if (instruction == "ADD" || instruction == "ADC")
                    {
                        encodedInstruction = encodedInstruction | m_regaddr[firstToken];
                        m_program.push_back(encodedInstruction);
                        std::cout << "Encoded instruction = " << std::hex << static_cast<int>(encodedInstruction);
                    }
                    else if (instruction == "LXI")
                    {
                        encodedInstruction = encodedInstruction | (m_regpairaddr[firstToken] << 4);
                        m_program.push_back(encodedInstruction);
                        std::cout << "Encoded instruction = " << std::hex << static_cast<int>(encodedInstruction);
                    }
                    else if (instruction == "JNZ" || instruction == "JZ")
                    {
                        m_program.push_back(encodedInstruction);
                        unsigned long data = std::strtol(firstToken.c_str(), 0, 16);
                        m_program.push_back(static_cast<unsigned char>(data & 0x00ff));
                        m_program.push_back(static_cast<unsigned char>((data & 0xff00)>>8));
                        std::cout << "Encoded instruction = " << std::hex << static_cast<int>(encodedInstruction) 
                            << ", Data = " << static_cast<int>(data & 0x00ff) << ", "
                            << std::hex << static_cast<int>((data & 0xff00) >> 8) << std::endl;
                    }
                }
                else if (tokens == 0)
                {
                    m_program.push_back(encodedInstruction);
                    std::cout << "Encoded instruction = " << std::hex << static_cast<int>(encodedInstruction);
                }

            }
            else
            {
                std::cout << "Unknown instruction: \"" << instruction << "\"" << std::endl;
            }
            std::cout << std::endl;
        }
    }
    std::vector<unsigned char> ParseString(std::string buffer)
    {
        std::stringstream ss(buffer);
        std::string line;
        while (std::getline(ss, line))
        {
            this->ParseLine(line);
        }
        return m_program;
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