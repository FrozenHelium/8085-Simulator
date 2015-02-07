#pragma once

#include "common.h"
#include "Bus.h"

// A general representation of 16-bit address mapped 64k memory
class Memory_64K
{
public:
    void OnClockTick()
    {
        auto control = m_control->GetData();

        // chip select for memory (Active Low)
        if ((control & 0x01) == HIGH) return;

        // ALE signal
        if (((control & 0x02) >> 1) == HIGH)
        {
            m_addrBuff = Bus_16_bit(*m_addr, *m_addr_data).GetData();
        }
        else
        {
            // Read (Active Low)
            if (((control & 0x10) >> 4) == LOW)
            {
                m_addr_data->PutData((*this)[m_addrBuff]);
            }

            // Write (Active Low)
            if (((control & 0x20) >> 5) == LOW)
            {
                (*this)[m_addrBuff] = m_addr_data->GetData();
            }
        }
    }

    void ConnectBus(Bus_8_bit* addr, Bus_8_bit* addr_data, Bus_8_bit* control)
    {
        m_addr = addr;
        m_addr_data = addr_data;
        m_control = control;
    }
    Memory_64K()
    {
        memset(m_data, 0, sizeof(m_data));
    }
    void SetData(unsigned short addr, unsigned char data)
    {
        m_data[addr] = data;
    }
private:
    unsigned char& operator[](Register_16_bit addr)
    {
        return m_data[addr.Value()];
    }
    unsigned char m_data[65536];
    Bus_8_bit* m_addr;
    Bus_8_bit* m_addr_data;
    Bus_8_bit* m_control;
    Register_16_bit m_addrBuff;
};
