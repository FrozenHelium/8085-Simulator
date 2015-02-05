#pragma once

#include "common.h"
//#include "Bus.h"

template<class StorageType>
class Register
{
public:
    Register<StorageType>& operator = (const StorageType& data)
    {
        m_data = data;
        return *this;
    }
    void SetData(StorageType data)
    {
        m_data = data;
    }
    StorageType Value() const
    {
        return m_data;
    }
    bool Tristated()
    {
        return m_tristated;
    }
    Register() :m_tristated(false){}
protected:
    StorageType m_data;
    bool m_tristated;
};


class Register_8_bit: public Register<unsigned char>
{
public:

    Register_8_bit operator + (const Register_8_bit& op)
    {
        return Register_8_bit(m_data + op.m_data);
    }
    Register_8_bit(unsigned char data)
    {
        m_data = data;
    }
    Register_8_bit() 
    {
        m_data = 0;
    }
protected:
};

class Register_16_bit: public Register<unsigned short>
{
public:
    Register_16_bit operator ++()
    {
        return Register_16_bit(++m_data);
    }
    Register_8_bit LowByte()
    {
        return static_cast<unsigned char>(m_data);
    }
    Register_8_bit HighByte()
    {
        return static_cast<unsigned char>(m_data >> 8);
    }
    Register_16_bit(const Register_8_bit& high, const Register_8_bit& low)
    {
        m_data = static_cast<unsigned short>((high.Value() << 8) | low.Value());
    }
    Register_16_bit(unsigned short data)
    {
        m_data = data;
    }
    Register_16_bit()
    {
        m_data = 0;
    }
protected:
};
