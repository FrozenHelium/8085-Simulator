#pragma once

#include "common.h"
#include "Register.h"
#include "Pins.h"

template<class BusDataType>
class Bus
{
public:
    void PutData(BusDataType data)
    {
        m_data = data;
        for (Register<BusDataType> *reg : m_attachedRegisters)
        {
            reg->SetData(data);
        }
        int i = 0;
        int shiftAmount = 0;
        BusDataType mask = 1;
        for (Pins *pins : m_connectedPins)
        {
            shiftAmount = 0;
            for (int pin : m_pinList[i])
            {
                pins->SetPinState(pin, ((m_data >> shiftAmount++)&mask)?SIGNALSTATE::HIGH:SIGNALSTATE::LOW);
            }
        }
    }

    // Updates the bus with all attached register and connected pins
    void Update()
    {
        BusDataType data = 0;
        for (Register<BusDataType> *reg : m_attachedRegisters)
        {
            if (!reg->Tristated())
            {
                data = reg->Value();
                if (m_data != data)
                {
                    this->PutData(data);
                }
            }
        }
        int i = 0;
        int shiftAmount = 0;
        for (Pins *pins : m_connectedPins)
        {
            shiftAmount = 0;
            data = 0;
            for (auto pin : m_pinList[i])
            {
                data |= (pins->PinState(pin) << (shiftAmount++));
            }
            if (data != m_data)
            {
                this->PutData(data);
            }
        }
    }
    void AttachRegister(Register<BusDataType>* reg)
    {
        m_attachedRegisters.push_back(reg);
    }
    void ConnectPins(Pins* pins, std::vector<int> pinsToConnect)
    {
        m_connectedPins.push_back(pins);
        m_pinList.push_back(pinsToConnect);
    }
    BusDataType GetData() const
    {
        return m_data;
    }
protected:
    BusDataType m_data;
    std::vector<Register<BusDataType>*> m_attachedRegisters;
    std::vector<Pins*> m_connectedPins;
    std::vector<std::vector<int>> m_pinList;
};

class Bus_8_bit:public Bus<unsigned char>
{
public:
    Bus_8_bit()
    {
        m_data = 0;
    }
    Bus_8_bit(unsigned char busData)
    {
        m_data = busData;
    }
};

class Bus_16_bit:public Bus<unsigned short>
{
public:
    Bus_16_bit(const Bus_8_bit& highOrder, const Bus_8_bit& lowOrder)
    {
        m_data = static_cast<unsigned short>((highOrder.GetData() << 8) | lowOrder.GetData());
    }
    Bus_16_bit(unsigned short busData)
    {
        m_data = busData;
    }
    Bus_16_bit()
    {
        m_data = 0;
    }
protected:
};