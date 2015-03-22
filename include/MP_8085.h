#pragma once

#include "common.h"
#include "Register.h"
#include "Pins.h"
#include "Bus.h"
#include "Memory.h"
#include "Flags.h"

#include <iomanip>

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


class MP_8085
{
public:
    int m_currentTstate;
    void MemoryReadCycle(int tState)
    {
        switch (tState)
        {
        case 0:
            m_address_buffer = m_reg_PC.HighByte();
            m_address_data_buffer = m_reg_PC.LowByte();
            m_pins.SetPin(ALE);
            ++m_reg_PC;
            break;
        case 1:
            m_pins.ResetPin(ALE);
            m_pins.ResetPin(RD_BAR);  // Memory read mode
            break;
        case 2:
            // data recieved
            m_pins.SetPin(RD_BAR);
            break;
        }
    }
    void MemoryWriteCycle(int tState)
    {
        switch (tState)
        {
        case 0:
            m_address_buffer = m_reg_PC.HighByte();
            m_address_data_buffer = m_reg_PC.LowByte();
            m_pins.SetPin(ALE);
            ++m_reg_PC;
            break;
        case 1:
            m_pins.ResetPin(ALE);
            m_pins.ResetPin(WR_BAR);  // Memory write mode
            break;
        case 2:
            // data sent
            m_pins.SetPin(WR_BAR);
            break;
        }
    }
    void ALU_Add(const Register_8_bit& opReg)
    {
        int result = m_accumulator.Value() + opReg.Value() + m_flags.Get(CARRY);
        if (result > 255)
        {
            m_flags.Set(CARRY);
        }
        else
        {
            m_flags.Reset(CARRY);
        }
        if ((m_accumulator.Value() & 0x0f + opReg.Value() & 0x0f + m_flags.Get(CARRY)) > 0x0f)
        {
            m_flags.Set(AUXILARY_CARRY);
        }
        else
        {
            m_flags.Reset(AUXILARY_CARRY);
        }
        if (result == 0)
        {
            m_flags.Set(ZERO);
        }
        else
        {
            m_flags.Reset(ZERO);
        }
        m_flags.Reset(SIGN);
        m_accumulator.SetData(static_cast<unsigned char>(result));
        int noOf1s = 0;
        for (int i = 0; i < 8; i++)
        {
            if (m_accumulator[i])
            {
                ++noOf1s;
            }
        }
        if (noOf1s % 2)
        {
            m_flags.Set(PARITY);
        }
        else
        {
            m_flags.Reset(PARITY);
        }
    }


    bool OnClockTick()
    {
        bool instructionCycleCompleted = false;
        static std::vector<Register_8_bit*> regs = { &m_reg_B, &m_reg_C, &m_reg_D, &m_reg_E, &m_reg_H, &m_reg_L, 0, &m_accumulator };
        static std::vector<Register_8_bit*> regPairsHigh = { &m_reg_B, &m_reg_D, &m_reg_H, 0 };
        static std::vector<Register_8_bit*> regPairsLow = { &m_reg_C, &m_reg_E, &m_reg_L, 0 };
        static int flagTypes[] = { ZERO, CARRY, PARITY, SIGN };

        switch (m_currentTstate)
        {
        case 0:
            m_address_buffer = m_reg_PC.HighByte();
            m_address_data_buffer = m_reg_PC.LowByte();
            m_pins.SetPin(ALE);
            ++m_reg_PC;
            break;
        case 1:
            m_pins.ResetPin(ALE);
            m_pins.ResetPin(RD_BAR);  // Memory read mode
            break;
        case 2:
            // Opcode recieved
            m_pins.SetPin(RD_BAR);
            break;
        case 3:
            m_IR = m_address_data_buffer.Value();

        default:
            if (m_IR[7] == 0 && m_IR[6] == 1) // MOV or HLT
            {
                int dest = (m_IR.Value() >> 3) & 0x07, src = m_IR.Value() & 0x07;

                if (src == 6 && dest == 6)  // HLT
                {
                    m_currentTstate = 0;
                    instructionCycleCompleted = true;
                    throw "Halt";
                }
                else if (src == 0x06) // MOV R, M
                {
                    switch (m_currentTstate)
                    {
                    case 4:
                        m_address_data_buffer = m_reg_L;
                        m_address_buffer = m_reg_H;
                        m_pins.SetPin(ALE);
                        break;
                    case 5:
                        m_pins.ResetPin(ALE);
                        m_pins.ResetPin(RD_BAR);    // Active low
                        break;
                    case 6:
                        m_pins.SetPin(RD_BAR);
                        *regs[dest] = m_address_data_buffer.Value();
                        std::cout << static_cast<int>(m_reg_B.Value()) << std::endl;
                        instructionCycleCompleted = true;
                        break;
                    }
                }
                else if (dest == 0x06) // MOV M, R
                {
                    switch (m_currentTstate)
                    {
                    case 4:
                        m_address_data_buffer = m_reg_L;
                        m_address_buffer = m_reg_H;
                        m_pins.SetPin(ALE);
                        break;
                    case 5:
                        m_pins.ResetPin(ALE);
                        m_pins.ResetPin(WR_BAR);    // Active low
                        m_address_data_buffer = *regs[src];
                        break;
                    case 6:
                        m_pins.SetPin(WR_BAR);
                        instructionCycleCompleted = true;
                        break;
                    }
                }
                else
                {
                    *regs[dest] = *regs[src];
                    instructionCycleCompleted = true;
                }
            }
            else if (m_IR[7] == 1 && m_IR[6] == 0)  // initial condition for ADD
            {
                if (m_IR[5] == 0 && m_IR[4] == 0)   // ADD & ADC
                {
                    if ((m_IR.Value() & 0x07) != 0x06)    // ADD R
                    {
                        if (m_IR[3] == 0)   // condition for ADD (without carry)
                        {
                            m_flags.Reset(CARRY);
                        }
                        this->ALU_Add(*regs[m_IR.Value() & 0x07]);
                        instructionCycleCompleted = true;
                    }
                    else    // ADD M
                    {
                        switch (m_currentTstate)
                        {
                        case 4:
                            m_address_data_buffer = m_reg_L;
                            m_address_buffer = m_reg_H;
                            m_pins.SetPin(ALE);
                            break;
                        case 5:
                            m_pins.ResetPin(ALE);
                            m_pins.ResetPin(RD_BAR);    // Active low
                            break;
                        case 6:
                            m_pins.SetPin(RD_BAR);
                            m_reg_W = m_address_data_buffer.Value();
                            if (m_IR[3] == 0)   // condition for ADD (without carry)
                            {
                                m_flags.Reset(CARRY);
                            }
                            this->ALU_Add(m_reg_W);
                            instructionCycleCompleted = true;
                            break;
                        }
                    }
                }
            }
            else if (m_IR[7] == 0 && m_IR[6] == 0) // initial condition for MVI, LXI, STA, LDA, SHLD, LHLD, STAX, LDAX
            {
                if ((m_IR.Value() & 0x07) == 0x06 && ((m_IR.Value() >> 3) & 0x07) == 0x06) // MVI M
                {
                    switch (m_currentTstate)
                    {
                    case 4:
                        m_address_buffer = m_reg_PC.HighByte();
                        m_address_data_buffer = m_reg_PC.LowByte();
                        m_pins.SetPin(ALE);
                        ++m_reg_PC;
                        break;
                    case 5:
                        m_pins.ResetPin(ALE);
                        m_pins.ResetPin(RD_BAR);
                        break;
                    case 6:
                        m_pins.SetPin(RD_BAR);
                        m_temp = m_address_data_buffer;
                        break;
                    case 7:
                        m_address_data_buffer = m_reg_L;
                        m_address_buffer = m_reg_H;
                        m_pins.SetPin(ALE);
                        break;
                    case 8:
                        m_pins.ResetPin(ALE);
                        m_pins.ResetPin(WR_BAR);
                        m_address_data_buffer = m_temp;
                        break;
                    case 9:
                        m_pins.SetPin(WR_BAR);
                        instructionCycleCompleted = true;
                        break;
                    }
                }
                else if ((m_IR.Value() & 0x07) == 0x06)  // MVI R
                {
                    switch (m_currentTstate)
                    {
                    case 4:
                        m_address_buffer = m_reg_PC.HighByte();
                        m_address_data_buffer = m_reg_PC.LowByte();
                        m_pins.SetPin(ALE);
                        ++m_reg_PC;
                        break;
                    case 5:
                        m_pins.ResetPin(ALE);
                        m_pins.ResetPin(RD_BAR);
                        break;
                    case 6:
                        m_pins.SetPin(RD_BAR);
                        *regs[(m_IR.Value() >> 3) & 0x07] = m_address_data_buffer;
                        instructionCycleCompleted = true;
                        break;
                    }
                }
                else if ((m_IR.Value() & 0x0f) == 0x01)   // LXI
                {
                    int dest = (m_IR.Value() >> 4) & 0x03;
                    switch (m_currentTstate)
                    {
                    case 6:
                        if (dest != 3) *regPairsLow[dest] = m_address_data_buffer;
                        else m_reg_Z = m_address_buffer;
                    case 5:
                    case 4:
                        this->MemoryReadCycle(m_currentTstate - 4);
                        break;
                    case 9:
                        if (dest != 3) *regPairsHigh[dest] = m_address_data_buffer;
                        else
                        {
                            m_reg_W = m_address_data_buffer;
                            m_reg_SP = Register_16_bit(m_reg_W, m_reg_Z);
                        }
                        instructionCycleCompleted = true;
                    case 8:
                    case 7:
                        this->MemoryReadCycle(m_currentTstate - 7);
                        break;
                    }

                }
                else if ((m_IR.Value() & 0x0f) == 0x02)   // STAX
                {
                }
                else if ((m_IR.Value() & 0x0f) == 0x0A)   // LDAX
                {
                }
                else
                {
                    switch (m_IR.Value())
                    {
                    case 0x32:  // STA
                    case 0x3A:  // LDA
                    case 0x22:  // SHLD
                    case 0x2A:  // LHLD
                        break;
                    }
                }

            }
            else if (m_IR[7] == 1 && m_IR[6] == 1)
            {

                if ((m_IR.Value() & 0x0f) == 0x05) // PUSH
                {
                    static int src = 0;
                    switch (m_currentTstate)
                    {
                    case 4:
                        --m_reg_SP;
                        m_address_buffer = m_reg_SP.HighByte();
                        m_address_data_buffer = m_reg_SP.LowByte();
                        m_pins.SetPin(ALE);
                        
                        break;
                    case 5:
                        m_pins.ResetPin(ALE);
                        m_pins.ResetPin(WR_BAR);
                        src = (m_IR.Value() >> 4) & 0x03;
                        m_address_data_buffer = regPairsHigh[src]->Value();
                        break;
                    case 6:
                        m_pins.SetPin(WR_BAR);
                        break;
                    case 7:
                        break;
                    case 8:
                        --m_reg_SP;
                        m_address_buffer = m_reg_SP.HighByte();
                        m_address_data_buffer = m_reg_SP.LowByte();
                        m_pins.SetPin(ALE);
                        break;
                    case 9:
                        m_pins.ResetPin(ALE);
                        m_pins.ResetPin(WR_BAR);
                        src = (m_IR.Value() >> 4) & 0x03;
                        m_address_data_buffer = regPairsLow[src]->Value();
                        break;
                    case 10:
                        m_pins.SetPin(WR_BAR);
                        break;
                    case 11:
                        instructionCycleCompleted = true;
                        break;
                    }

                }
                else if ((m_IR.Value() & 0x0f) == 0x01) // POP
                {
                    static int dest = 0; 
                    switch (m_currentTstate)
                    {
                    case 4:
                        m_address_buffer = m_reg_SP.HighByte();
                        m_address_data_buffer = m_reg_SP.LowByte();
                        m_pins.SetPin(ALE);
                        ++m_reg_SP;
                        break;
                    case 5:
                        m_pins.ResetPin(ALE);
                        m_pins.ResetPin(RD_BAR);
                        break;
                    case 6:
                        dest = (m_IR.Value() >> 4) & 0x03;
                        *regPairsLow[dest] = m_address_data_buffer;
                        m_pins.SetPin(RD_BAR);
                        break;
                    case 7:
                        m_address_buffer = m_reg_SP.HighByte();
                        m_address_data_buffer = m_reg_SP.LowByte();
                        m_pins.SetPin(ALE);
                        ++m_reg_SP;
                        break;
                    case 8:
                        m_pins.ResetPin(ALE);
                        m_pins.ResetPin(RD_BAR);
                        break;
                    case 9:
                        dest = (m_IR.Value() >> 4) & 0x03;
                        *regPairsHigh[dest] = m_address_data_buffer;
                        m_pins.SetPin(RD_BAR);
                        break;
                    }
                }
                else if ((m_IR.Value() & 0x07) == 0x02) // Conditional jumps
                {
                    static int instFlagValue = 0;       // flag value required for jump
                    static int flagValue = 0;           // flag value in the flags
                    
                    static int flagType = 0;
                    switch (m_currentTstate)
                    {
                    case 4:
                        instFlagValue = m_IR[3];
                        flagType = ((m_IR[4] & m_IR[5]) >> 4);
                        flagValue = m_flags.Get(flagTypes[flagType]);
                    case 5:
                        this->MemoryReadCycle(m_currentTstate - 4);
                        break;
                    case 6:
                        this->MemoryReadCycle(m_currentTstate - 4);
                        this->m_reg_Z = m_address_data_buffer;
                        if (flagValue != instFlagValue)
                        {
                            ++m_reg_PC;
                            instructionCycleCompleted = true;
                        }
                        break;
                    case 9:
                        this->m_reg_W = m_address_data_buffer;
                        m_reg_PC = Register_16_bit(m_reg_W, m_reg_Z);
                        instructionCycleCompleted = true;
                    case 7:
                    case 8:
                        this->MemoryReadCycle(m_currentTstate - 7);
                        break;
                    }
                }
                else if ((m_IR.Value() & 0x07) == 0x04) // Conditional calls
                {
                }
                else if ((m_IR.Value() & 0x07) == 0x03) // Conditional returns
                {
                }
                else
                {
                    switch(m_IR.Value())    // XCHG
                    {
                    case 0xEB:
                        m_reg_W = m_reg_H;
                        m_reg_H = m_reg_D;
                        m_reg_D = m_reg_W;
                        m_reg_Z = m_reg_L;
                        m_reg_L = m_reg_E;
                        m_reg_E = m_reg_Z;
                        instructionCycleCompleted = true;
                        break;
                    case 0xE3:  // XTHL
                        break;
                    case 0xF9:  // SPHL
                        break;
                    }
                }
            }
            break;
        }
        ++m_currentTstate;
        m_dataBus.Update();
        if (instructionCycleCompleted)
        {
            m_currentTstate = 0;
        }
        return instructionCycleCompleted;
    }

    void ConnectBus(Bus_8_bit* address_bus, Bus_8_bit* address_data_bus, Bus_8_bit* control_bus)
    {
        address_data_bus->AttachRegister(&m_address_data_buffer);
        address_data_bus->ConnectPins(&m_pins, { AD0, AD1, AD2, AD3, AD4, AD5, AD6, AD7 });
        address_bus->AttachRegister(&m_address_buffer);
        address_bus->ConnectPins(&m_pins, { A8, A9, A10, A11, A12, A13, A14, A15 });
        control_bus->AttachRegister(&m_control_buffer);
        control_bus->ConnectPins(&m_pins, { IO__M_BAR, ALE, S0, S1, RD_BAR, WR_BAR, INTA_BAR, HLDA });
    }

    Pins_8085* GetPins()
    {
        return &m_pins;
    }

    MP_8085()
    {
        m_dataBus.AttachRegister(&m_address_data_buffer);
        m_reg_PC = 0x8000;
        m_reg_H = 0x80;
        m_reg_L = 0x50;
        m_accumulator = 0;
        m_reg_B = 0xff;
        m_currentTstate = 0;
    }
    std::vector<unsigned short> GetRegisterValues()
    {
        return{ m_accumulator.Value(), m_flags.Value(), m_reg_B.Value(), 
            m_reg_C.Value(), m_reg_D.Value(), m_reg_E.Value(), m_reg_H.Value(), m_reg_L.Value(), m_reg_PC.Value() };
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



#include <thread>


enum ExecutionModes{NORMAL, DELAYED, SINGLESTEP, CYCLEWISE};

class MPSystem_8085
{
public:
    MPSystem_8085()
    {
        m_cpu.ConnectBus(&m_address_bus, &m_address_data_bus, &m_control_bus);
        m_cpuPins = m_cpu.GetPins();
        m_memory.ConnectBus(&m_address_bus, &m_address_data_bus, &m_control_bus);
        m_endExecutionThread = false;
        m_executing = false;
        m_executionMode = NORMAL;
        m_executionState = STOPPED;
        this->SetClockFrequency(3000000L);  // 3 MHz
        m_executionThread = std::thread([this](){this->ProgramExecutionThread(); });
    }
    std::vector<unsigned short> GetRegisterValues()
    {
        return m_cpu.GetRegisterValues();
    }
    
    void LoadProgram(std::vector<unsigned char> program)
    {
        m_program = program;
    }

    void ProgramExecutionThread()
    {
        
        while (!m_endExecutionThread)
        {
            if (m_executing)
            {
                
               // std::cout << "executing.." << std::endl;
                m_cpuPins->SetPins({ S1, S0 });
                m_cpuPins->ResetPin(IO__M_BAR);

                unsigned short startAddr = 0x8000;

                for (auto instr : m_program)
                {
                    m_memory.SetData(startAddr++, instr);
                }
                m_executionState = RUNNING;
                while (m_executing)
                {
                    if (m_executionState == RUNNING)
                    {
                        try
                        {
                            bool instructionCompleted = false;
                            instructionCompleted = m_cpu.OnClockTick();
                            m_address_bus.Update();
                            m_address_data_bus.Update();
                            m_control_bus.Update();
                            m_memory.OnClockTick();
                            if (m_executionMode == SINGLESTEP && instructionCompleted)
                            {
                                m_executionState = PAUSED;
                            }
                            //std::this_thread::sleep_for(std::chrono::nanoseconds(m_cycleDelay));
                        }
                        catch (...)
                        {
                            m_executing = false;
                            m_executionState = STOPPED;
                            break;
                        }
                    }
                    else    // wait for next step
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }
                }
            }
            while (m_executing == false && m_endExecutionThread == false)
            {
                //std::cout << "stopped.." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    }
    void Step()
    {
        if (m_executionMode == SINGLESTEP && m_executing && m_executionState == PAUSED)
        {
            m_executionState = RUNNING;
        }
    }
    void SetClockFrequency(long long frequency)
    {
        if (frequency <= 0) throw Exception("invalid frequency");
        long double t = 1 / static_cast<long double>(frequency);
        m_cycleDelay = t * 1000000000L;
        std::cout << m_cycleDelay << std::endl;
    }
    void SetExecutionMode(int mode)
    {
        m_executionMode = mode;
    }
    void StartAsync()
    {
        if (!m_executing)
        {
            m_executing = true;
        }
        else throw Exception("Execution in progress");
    }
    void Stop()
    {
        if (m_executing)
        {
            m_executing = false;
        }
    }
    void Test(std::vector<unsigned char> program)
    {
        m_cpuPins->SetPins({ S1, S0 });
        m_cpuPins->ResetPin(IO__M_BAR);

        unsigned short startAddr = 0x8000;
        for (auto instr : program)
        {
            m_memory.SetData(startAddr++, instr);
        }
        std::cout << std::endl;
        if (testThread.joinable())
        {
            testThread.join();
        }
        testThread = std::thread([this](){
            m_executing = true;
            while (true)
            {
                try
                {
                    m_cpu.OnClockTick();
                    m_address_bus.Update();
                    m_address_data_bus.Update();
                    m_control_bus.Update();
                    m_memory.OnClockTick();
                    //        std::cout << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
                catch (...)
                {
                    break;
                }
            }
            m_executing = false;
        });
    }
    bool Executing()
    {
        return m_executing;
    }
    ~MPSystem_8085()
    {
        m_endExecutionThread = true;
        if (m_executionThread.joinable())
        {
            m_executionThread.join();
        }
    }
private:
    enum ExecutionStates{ RUNNING, PAUSED, STOPPED };
    MP_8085 m_cpu;
    Memory_64K m_memory;
    Bus_8_bit m_address_bus, m_address_data_bus, m_control_bus;
    Pins_8085* m_cpuPins;
    bool m_executing;
    std::thread testThread;
    std::thread m_executionThread;
    int m_executionMode;
    std::vector<unsigned char> m_program;
    int m_cycleDelay;
    bool m_endExecutionThread;
    int m_executionState;
};