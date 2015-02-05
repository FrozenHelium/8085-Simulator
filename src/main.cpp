#include <iostream>
#include <vector>
#include "MP_8085.h"


int main()
{
    Pins_8085 pins;
    Bus_16_bit addr;
    addr.ConnectPins(&pins, { AD0, AD1, AD2, AD3, AD4, AD5, AD6, AD7, A8, A9, A10, A11, A12, A13, A14, A15 });
    Register_16_bit addr_buff;
    addr.AttachRegister(&addr_buff);
    addr.Update();
    addr_buff = 0x8001;
    addr.Update();
    std::cout << pins.GetAddress().Value() << std::endl;
    pins.SetAddress(Register_16_bit(0x0005));
    addr.Update();
    std::cout << addr_buff.Value() << std::endl;

    //Memory_64K mem;
    /*Bus_8_bit addr(0x00);
    Bus_8_bit addr_data(0x00);
    addr_data = 0x02;
    mem.Access(addr, addr_data, SIGNALSTATE::HIGH, SIGNALSTATE::HIGH, SIGNALSTATE::LOW);
    addr_data = 0x0f;
    mem.Access(addr, addr_data, SIGNALSTATE::LOW, SIGNALSTATE::HIGH, SIGNALSTATE::LOW);
    addr_data = 0x02;
    mem.Access(addr, addr_data, SIGNALSTATE::HIGH, SIGNALSTATE::LOW, SIGNALSTATE::HIGH);
    mem.Access(addr, addr_data, SIGNALSTATE::LOW, SIGNALSTATE::LOW, SIGNALSTATE::HIGH);
    std::cout << static_cast<unsigned short>(addr_data.Value());*/
    //MP_8085 mp;
    //mp.Test();
    std::cin.get();
    return 0;
}

