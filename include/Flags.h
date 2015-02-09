#pragma once

#include "common.h"


enum FLAG_STATE{ SET = 1, RESET = 0 };

enum FLAG_TYPE_8085{ CARRY = 0, PARITY = 2, AUXILARY_CARRY = 4, SIGN = 7, ZERO = 6 };


class Flags_8085
{
public:
    void Set(int type)
    {
        m_data = m_data | (1 << type);
    }
    void Reset(int type)
    {
        m_data = m_data & (~(1 << type));
    }
    int Get(int type)
    {
        return ((m_data >> type) & 0x01) ? SET : RESET;
    }
private:
    unsigned char m_data;
};
