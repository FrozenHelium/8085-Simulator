#pragma once

#include "common.h"
#include "Register.h"
#include "Pins.h"
#include "Bus.h"

enum MNEMONICS{
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

class Memory_64K
{
public:
    void Access(Bus_8_bit& addr, Bus_8_bit& addr_data, const SIGNALSTATE& ale, const SIGNALSTATE& wr_bar, const SIGNALSTATE& rd_bar)
    {
        if (ale == SIGNALSTATE::HIGH)
        {
            m_addr = Bus_16_bit(addr, addr_data);
        }
        else
        {
            if (wr_bar == SIGNALSTATE::HIGH)
            {
                (*this)[m_addr] = addr_data.GetData();
            }

            if (rd_bar == SIGNALSTATE::HIGH)
            {
                addr_data = (*this)[m_addr];
            }
        }
    }
    Memory_64K()
    {
        memset(m_data, 0, sizeof(m_data));
    }
private:
    unsigned char& operator[](Bus_16_bit addr)
    {
        return m_data[addr.GetData()];
    }

    unsigned char m_data[65536];
    Bus_16_bit m_addr;
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
        else if (io_m == SIGNALSTATE::HIGH && s1 == SIGNALSTATE::LOW && s0 == SIGNALSTATE::HIGH)
        {
            // IO READ
        }
        else if (io_m == SIGNALSTATE::LOW && s1 == SIGNALSTATE::HIGH && s0 == SIGNALSTATE::HIGH)    // Opcode fetch
        {
            // 1st clock tick
            if (m_pins.PinState(PIN_8085::ALE) == SIGNALSTATE::LOW && m_pins.PinState(PIN_8085::RD_BAR) == SIGNALSTATE::HIGH)
            {
                m_pins.SetAddress({ m_address_buffer, m_address_data_buffer });
                m_pins.SetPin(PIN_8085::ALE);
            }

            // 2nd clock tick
            if (m_pins.PinState(PIN_8085::ALE) == SIGNALSTATE::HIGH && m_pins.PinState(PIN_8085::RD_BAR) == SIGNALSTATE::HIGH)
            {
                m_pins.ResetPin(PIN_8085::ALE);
                m_pins.ResetPin(PIN_8085::RD_BAR);
            }

        }
        else if (io_m == SIGNALSTATE::HIGH && s1 == SIGNALSTATE::HIGH && s0 == SIGNALSTATE::HIGH)
        {
            // Interrupt Acknowledgement
        }
    }
    void Update()
    {
        
    }
    void Test()
    {
        m_pins.SetPins({ 0, 1, 2, 3, 4 });
    }
    void ConnectBus(Bus_8_bit* address_bus, Bus_8_bit* address_data_bus, Bus_8_bit* control_bus)
    {
        m_address_bus = address_bus;
        m_address_data_bus = address_data_bus;
        m_control_bus = control_bus;

        m_address_data_bus->AttachRegister(&m_address_data_buffer);
    }
    void ConnectPins(Pins_8085* &pins)
    {
        pins = &m_pins;
    }
private:
    Register_8_bit m_accumulator, m_temp;
    Flags_8085 m_flags;
    Register_8_bit m_IR;
    Register_8_bit m_reg_W, m_reg_Z, m_reg_B, m_reg_C, m_reg_D, m_reg_E, m_reg_H, m_reg_L;
    Register_16_bit m_SP, m_PC;
    Register_8_bit m_address_buffer, m_address_data_buffer;
    Bus_8_bit m_dataBus;
    Pins_8085 m_pins;

    Bus_8_bit* m_address_bus;
    Bus_8_bit* m_address_data_bus;
    Bus_8_bit* m_control_bus;
};

#include <thread>

class Clock
{
public:
    // Frequency in Hz at which the clock ticks
    Clock(unsigned long long tickFrequency, std::function<void()> onTickCallback)
    {
        m_onTick = onTickCallback;
        m_delayDuration = 1000000000L / tickFrequency;
    }
    void Start()
    {
        m_running = true;
        while (m_running)
        {
            m_onTick();
            //std::this_thread::sleep_until()
        }
    }
    void StartAsync()
    {
        //m_tickThread = std::thread(std::bind(Clock::Start, this));
    }
    void Stop()
    {
        m_running = false;
        if (m_tickThread.joinable())
        {
            m_tickThread.join();
        }
    }
    
private:
    unsigned long long m_frequency;
    // delay duration in nanoseconds
    unsigned long long m_delayDuration;
    std::function<void()> m_onTick;
    bool m_running;
    std::thread m_tickThread;
};

class MPSystem_8085
{
public:
    MPSystem_8085() /*:m_clock(3000000L, std::bind(MP_8085::OnClockTick, m_cpu))*/
    {
        m_cpu.ConnectBus(&m_address_bus, &m_address_data_bus, &m_control_bus);
        m_cpu.ConnectPins(m_cpuPins);
    }
private:
    MP_8085 m_cpu;
    Memory_64K m_memory;
    Bus_8_bit m_address_bus, m_address_data_bus, m_control_bus;
    //Clock m_clock;
    Pins_8085* m_cpuPins;
};