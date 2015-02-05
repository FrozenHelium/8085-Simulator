#pragma once

#include "common.h"

enum SIGNALSTATE{ HIGH = 1, LOW = 0 };


// A general representation of hardware IC pins
class Pins
{
public:
    Pins() :m_noOfPins(0){}
    void SetNoOfPins(int n)
    {
        m_noOfPins = n;
        int vecSize = (n >> 3) + 1;
        m_data.resize(vecSize);
    }

    SIGNALSTATE PinState(int pin)
    {
        unsigned short vecIndex = pin >> 3;
        unsigned short bitPosition = pin % 8;
        unsigned char data = m_data[vecIndex];
        data = (data >> bitPosition) & 0x01;
        return data ? SIGNALSTATE::HIGH : SIGNALSTATE::LOW;
    }

    void SetPinState(int pin, SIGNALSTATE state)
    {
        if (state == SIGNALSTATE::HIGH)
        {
            this->SetPin(pin);
        }
        else
        {
            this->ResetPin(pin);
        }
    }

    void SetPinStates(std::vector<int> pins, std::vector<SIGNALSTATE> values)
    {
        int i = 0;
        for (auto pin : pins)
        {
            this->SetPinState(pin, values[i++]);
        }
    }

    void SetPins(std::vector<int> pins)
    {
        for (auto pin : pins)
        {
            this->SetPin(pin);
        }
    }
    void SetPin(int pin)
    {
        unsigned short vecIndex = pin >> 3;
        unsigned short bitPosition = pin % 8;
        m_data[vecIndex] |= (0x01 << bitPosition);
    }
    void ResetPins(std::vector<int> pins)
    {
        for (auto pin : pins)
        {
            this->ResetPin(pin);
        }
    }
    void ResetPin(int pin)
    {
        unsigned short vecIndex = pin >> 3;
        unsigned short bitPosition = pin % 8;
        m_data[vecIndex] &= ~(0x01 << bitPosition);
    }
protected:

    int m_noOfPins;
    std::vector<unsigned char> m_data;
};



enum PIN_8085{
    X1, X2, RESETOUT, SOD, SID, TRAP, RST75, RST65, RST55,
    INTR, INTA_BAR, AD0, AD1, AD2, AD3, AD4, AD5, AD6, AD7, GND,
    A8, A9, A10, A11, A12, A13, A14, A15, S0, ALE, WR_BAR, RD_BAR,
    S1, IO__M_BAR, READY, RESETIN_BAR, CLK, HLDA, HOLD, VCC
};

class Pins_8085 :public Pins
{
public:
    Pins_8085()
    {
        

        this->SetNoOfPins(40);
        this->ResetPins({ X1, X2, RESETOUT, SOD, SID, TRAP, RST75, RST65, RST55, INTR, AD0, AD1, AD2, AD3, AD4, AD5,
            AD6, AD7, A8, A9, A10, A11, A12, A13, A14, A15, S0, S1, ALE, READY, CLK, HLDA, HOLD, VCC });
        this->SetPins({ INTA_BAR, WR_BAR, RD_BAR, IO__M_BAR });
        m_addressPins.resize(16);
        m_addressPins = { AD0, AD1, AD2, AD3, AD4, AD5, AD6, AD7, A8, A9, A10, A11, A12, A13, A14, A15 };
        m_dataPins = m_addressPins;
        m_dataPins.resize(8);
    }
    Register_16_bit GetAddress()
    {
        unsigned short shiftAmount = 0;
        unsigned short data = 0;
        for (auto pin : m_addressPins)
        {
            data |= (this->PinState(pin) << (shiftAmount++));
        }
        return data;
    }
    void SetAddress(const Register_16_bit& addr)
    {
        this->SetPinStates(m_addressPins,
            [addr]()->std::vector < SIGNALSTATE > 
            {
                std::vector<SIGNALSTATE> values;
                values.resize(16);
                unsigned short shiftAmount = 0;
                for (auto &value : values)
                {
                    value = ((addr.Value() >> (shiftAmount++)) & 0x0001) ? SIGNALSTATE::HIGH : SIGNALSTATE::LOW;
                }
                return values;
            }());
    }
    void SetData(const Register_8_bit& data)
    {
        this->SetPinStates(m_dataPins,
            [data]()->std::vector < SIGNALSTATE >
            {
                std::vector<SIGNALSTATE> values;
                values.resize(16);
                unsigned short shiftAmount = 0;
                for (auto &value : values)
                {
                    value = ((data.Value() >> (shiftAmount++)) & 0x01) ? SIGNALSTATE::HIGH : SIGNALSTATE::LOW;
                }
                return values;
            }());
    }
    Register_8_bit GetData()
    {
        unsigned char shiftAmount = 0;
        unsigned char data = 0;
        for (auto pin : m_dataPins)
        {
            data |= (this->PinState(pin) << (shiftAmount++));
        }
        return data;
    }
private:
    std::vector<int> m_addressPins;
    std::vector<int> m_dataPins;
};