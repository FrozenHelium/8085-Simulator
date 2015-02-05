#pragma once

#include "common.h"
#include "Register.h"
#include "Pins.h"
#include "Bus.h"

enum MNEMONICS_8085{
    ACI = 0xCE,
    ADC_A = 0x8f, ADC_B = 0x88, ADC_C = 0x89, ADC_D = 0x8A, ADC_E = 0x8B, ADC_H = 0x8C, ADC_L = 0x8D, ADC_M = 0x8E,
    ADD_A = 0x87, ADD_B = 0x80, ADD_C = 0x81, ADD_D = 0x82, ADD_E = 0x83, ADD_H = 0x84, ADD_L = 0x85, ADD_M = 0x86,

    MVI_A = 0x3E, MVI_B = 0x06, MVI_C = 0x0E, MVI_D = 0x16, MVI_E = 0x1E, MVI_H = 0x26, MVI_L = 0x2E, MVI_M = 0x36,

    MOV_A_A = 0x7F, MOV_A_B = 0x78, MOV_A_C, MOV_A_D, MOV_A_E, MOV_A_H, MOV_A_L, MOV_A_M,
    MOV_B_A, MOV_B_B, MOV_B_C, MOV_B_D, MOV_B_E, MOV_B_H, MOV_B_L, MOV_B_M,
    MOV_C_A, MOV_C_B, MOV_C_C, MOV_C_D, MOV_C_E, MOV_C_H, MOV_C_L, MOV_C_M,
    MOV_D_A, MOV_D_B, MOV_D_C, MOV_D_D, MOV_D_E, MOV_D_H, MOV_D_L, MOV_D_M,
    MOV_E_A, MOV_E_B, MOV_E_C, MOV_E_D, MOV_E_E, MOV_E_H, MOV_E_L, MOV_E_M,
    MOV_H_A, MOV_H_B, MOV_H_C, MOV_H_D, MOV_H_E, MOV_H_H, MOV_H_L, MOV_H_M,
    MOV_L_A, MOV_L_B, MOV_L_C, MOV_L_D, MOV_L_E, MOV_L_H, MOV_L_L, MOV_L_M,
    MOV_M_A, MOV_M_B, MOV_M_C, MOV_M_D, MOV_M_E, MOV_M_H, MOV_M_L,

    HLT = 0xEF
};


/*

Format for control signals through control bus
----------------------------------------------

+----------+----------+----------+----------+----------+----------+----------+----------+
|    C7    |    C6    |    C5    |    C4    |    C3    |    C2    |    C1    |    C0    |
+----------+----------+----------+----------+----------+----------+----------+----------+
|          |          |  WR_BAR  |  RD_BAR  |    S1    |    S0    |    ALE   | IO/M_BAR |
+----------+----------+----------+----------+----------+----------+----------+----------+

*/



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
            if (((control & 0x10)>>4) == LOW)
            {
                m_addr_data->PutData((*this)[m_addrBuff]);
            }

            // Write (Active Low)
            if (((control & 0x20)>>5) == LOW)
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

enum ALU_8085_OPERATION{
    ADDITION, SUBTRACTION, LOGICAL_AND, LOGICAL_OR, 
    LEFT_SHIFT, RIGHT_SHIFT, INCREMENT, DECREMENT, 
    LEFT_ROTATE_WITH_CARRY, RIGHT_ROTATE_WITH_CARRY, 
    LEFT_ROTATE_WITHOUT_CARRY, RIGHT_ROTATE_WITHOUT_CARRY,
};

enum FLAG_STATE{
    SET = 1, RESET = 0
};

enum FLAG_TYPE_8085{
    CARRY = 0, PARITY = 2, AUXILARY_CARRY = 4, SIGN = 7, ZERO = 6
};

class Flags_8085
{
public:
    void Set(FLAG_TYPE_8085 type)
    {
        m_data = m_data | (1 << type);
    }
    void Reset(FLAG_TYPE_8085 type)
    {
        m_data = m_data & (!(1 << type));
    }
    FLAG_STATE Get(FLAG_TYPE_8085 type)
    {
        return ((m_data >> type) & 0x01) ? FLAG_STATE::SET : FLAG_STATE::RESET;
    }
private:
    unsigned char m_data;
};

class ALU_8085
{
public:
    void Operate(Register_8_bit& accum, const Register_8_bit& temp, Flags_8085& flags)
    {
        unsigned short tempResult;
        switch (m_operationToPerform)
        {
        case ADDITION:
            tempResult = (accum + temp).Value() + flags.Get(CARRY);
            if (tempResult > 0xff)
            {
                flags.Set(CARRY);
            }
            accum = static_cast<unsigned char>(tempResult);
        }
    }
    void Operate(Register_8_bit& accum, Register_8_bit& flags)
    {

    }
    void Set(ALU_8085_OPERATION operation)
    {
        m_operationToPerform = operation;
    }
    ALU_8085()
    {
        m_operationToPerform = ADDITION;
    }
private:
    ALU_8085_OPERATION m_operationToPerform;
};

class CU_8085
{
public:
    void OnClockTick(Pins_8085& pins, ALU_8085& alu)
    {

    }
};




class MP_8085
{
public:
    void OnClockTick()
    {
        SIGNALSTATE s0 = m_pins.PinState(PIN_8085::S0);
        SIGNALSTATE s1 = m_pins.PinState(PIN_8085::S1);
        SIGNALSTATE io_m = m_pins.PinState(PIN_8085::IO__M_BAR);

        if (io_m == SIGNALSTATE::LOW && s1 == SIGNALSTATE::LOW && s0 == SIGNALSTATE::LOW)
        {
            // Halt
        }
        else if (io_m == SIGNALSTATE::LOW && s1 == SIGNALSTATE::LOW && s0 == SIGNALSTATE::HIGH)
        {
            // Memory WRITE
        }
        else if (io_m == SIGNALSTATE::LOW && s1 == SIGNALSTATE::HIGH && s0 == SIGNALSTATE::LOW)
        {
            // Memory READ
        }
        else if (io_m == SIGNALSTATE::HIGH && s1 == SIGNALSTATE::LOW && s0 == SIGNALSTATE::HIGH)
        {
            // IO WRITE
        }
        else if (io_m == SIGNALSTATE::HIGH && s1 == SIGNALSTATE::LOW && s0 == SIGNALSTATE::HIGH)    // IO READ
        {
        }
        else if (io_m == SIGNALSTATE::LOW && s1 == SIGNALSTATE::HIGH && s0 == SIGNALSTATE::HIGH)    // Opcode fetch
        {
            if (m_pins.PinState(PIN_8085::ALE) == SIGNALSTATE::LOW && m_pins.PinState(PIN_8085::RD_BAR) == SIGNALSTATE::HIGH)
            {
                m_address_buffer = m_reg_PC.HighByte();
                m_address_data_buffer = m_reg_PC.LowByte();
                m_pins.SetPin(PIN_8085::ALE);
            }
            else if (m_pins.PinState(PIN_8085::ALE) == SIGNALSTATE::HIGH && m_pins.PinState(PIN_8085::RD_BAR) == SIGNALSTATE::HIGH)
            {
                m_pins.ResetPin(PIN_8085::ALE);
                m_pins.ResetPin(PIN_8085::RD_BAR);  // Memory read mode
            }
            else if (m_pins.PinState(PIN_8085::ALE) == SIGNALSTATE::LOW && m_pins.PinState(PIN_8085::RD_BAR) == SIGNALSTATE::LOW)
            {
                m_pins.SetPin(PIN_8085::IO__M_BAR); // Data is recieved
                m_pins.ResetPin(PIN_8085::S0);      // initialize execute cycle
                m_pins.ResetPin(PIN_8085::S1);
            }
        }
        else if (io_m == SIGNALSTATE::HIGH && s1 == SIGNALSTATE::LOW && s0 == SIGNALSTATE::LOW) // execution
        {
            std::cout << "aah! finally here :v" << std::endl;
            std::cout << static_cast<unsigned short>(m_address_data_buffer.Value()) << std::endl;
        }
        else if (io_m == SIGNALSTATE::HIGH && s1 == SIGNALSTATE::HIGH && s0 == SIGNALSTATE::HIGH)
        {
            // Interrupt Acknowledgement
        }


        m_dataBus.Update();

    }

    void ConnectBus(Bus_8_bit* address_bus, Bus_8_bit* address_data_bus, Bus_8_bit* control_bus)
    {
        address_data_bus->AttachRegister(&m_address_data_buffer);
        address_data_bus->ConnectPins(&m_pins, {AD0, AD1, AD2, AD3, AD4, AD5, AD6, AD7});

        address_bus->AttachRegister(&m_address_buffer);
        address_bus->ConnectPins(&m_pins, {A8, A9, A10, A11, A12, A13, A14, A15});

        control_bus->AttachRegister(&m_control_buffer);
        control_bus->ConnectPins(&m_pins, { IO__M_BAR, ALE, S0, S1, RD_BAR, WR_BAR, INTA_BAR, HLDA });
    }

    Pins_8085* GetPins()
    {
        return &m_pins;
    }

    MP_8085()
    {
        //m_dataBus.AttachRegister(&m_IR);
        m_dataBus.AttachRegister(&m_address_data_buffer);
        m_reg_PC = 0x8000;
    }

private:
    Register_8_bit m_accumulator, m_temp;
    Flags_8085 m_flags;
    Register_8_bit m_IR;
    Register_8_bit m_reg_W, m_reg_Z, m_reg_B, m_reg_C, m_reg_D, m_reg_E, m_reg_H, m_reg_L;
    Register_16_bit m_reg_SP, m_reg_PC;

    Register_8_bit m_address_buffer, m_address_data_buffer, m_control_buffer;
    Bus_8_bit m_dataBus;
    Pins_8085 m_pins;
};



class MPSystem_8085
{
public:
    MPSystem_8085()
    {
        m_cpu.ConnectBus(&m_address_bus, &m_address_data_bus, &m_control_bus);
        m_cpuPins = m_cpu.GetPins();
        m_memory.ConnectBus(&m_address_bus, &m_address_data_bus, &m_control_bus);
    }
    void Test()
    {
        m_cpuPins->SetPins({ S1, S0 });
        m_cpuPins->ResetPin(IO__M_BAR);
        m_memory.SetData(0x8000, MVI_A);
        for (int i = 0; i < 4; i++)
        {
            m_cpu.OnClockTick();
            m_address_bus.Update();
            m_address_data_bus.Update();
            m_control_bus.Update();
            m_memory.OnClockTick();
        }
    }
private:
    MP_8085 m_cpu;
    Memory_64K m_memory;
    Bus_8_bit m_address_bus, m_address_data_bus, m_control_bus;
    Pins_8085* m_cpuPins;
};