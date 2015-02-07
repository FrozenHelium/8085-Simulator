#pragma once

#include "common.h"
#include "Register.h"
#include "Pins.h"
#include "Bus.h"
#include "Memory.h"

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

std::vector<unsigned char> g_arithematicInstructions = {ADD_A, ADD_B};
std::vector<unsigned char> g_logicInstructions;
std::vector<unsigned char> g_dataTransferInstructions = { MOV_A_A, MOV_A_B };
std::vector<unsigned char> g_branchingInstructions;
std::vector<unsigned char> g_miscInstructions;


/*
Format for control signals through control bus
----------------------------------------------

+----------+----------+----------+----------+----------+----------+----------+----------+
|    C7    |    C6    |    C5    |    C4    |    C3    |    C2    |    C1    |    C0    |
+----------+----------+----------+----------+----------+----------+----------+----------+
|          |          |  WR_BAR  |  RD_BAR  |    S1    |    S0    |    ALE   | IO/M_BAR |
+----------+----------+----------+----------+----------+----------+----------+----------+

*/




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
        m_data = m_data & (~(1 << type));
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
            else
            {
                flags.Reset(CARRY);
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
    int Decode(unsigned char instruction)
    {
        

        return 0;
    }
    CU_8085()
    {
        m_maxTStates = 4;
        m_currentInstruction = 0;
        m_currentTStates = 0;
    }
private:
    unsigned char m_currentInstruction;
    int m_instructionType;
    int m_currentTStates;
    int m_maxTStates;
};




class MP_8085
{
public:
    int m_currentTstate;
    void OnClockTick()
    {
        bool instructionCycleCompleted = false;
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
            int D7 = m_IR[7];
            int D6 = m_IR[6];

            if (D7 == 0 && D6 == 1) // MOV or HLT
            {
                int dest = (m_IR.Value() >> 3) & 0x07, src = m_IR.Value() & 0x07;
                static std::vector<Register_8_bit*> regs = { &m_reg_B, &m_reg_C, &m_reg_D, &m_reg_E, &m_reg_H, &m_reg_L, 0, &m_accumulator };
                if (src == 6 && dest == 6)  // HLT
                {
                    
                }
                else if (src == 6) // MOV R, M
                {
                    switch (m_currentTstate)
                    {
                    case 3:
                        break;
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
                else if (dest == 6) // MOV M, R
                {
                    switch (m_currentTstate)
                    {
                    case 3:
                        break;
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
            break;
        }
        ++m_currentTstate;
        m_dataBus.Update();
        if (instructionCycleCompleted)
        {
            m_currentTstate = 0;
        }
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
        m_dataBus.AttachRegister(&m_address_data_buffer);
        m_reg_PC = 0x8000;
        m_reg_H = 0x80;
        m_reg_L = 0x05;
        m_accumulator = 0;
        m_reg_B = 0xff;
        m_currentTstate = 0;
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

    ALU_8085 m_alu;
    CU_8085 m_cu;
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
        m_memory.SetData(0x8000, MOV_A_B);
        m_memory.SetData(0x8001, 0x46);
        m_memory.SetData(0x8005, 0x05);
        for (int i = 0; i < 11; i++)
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