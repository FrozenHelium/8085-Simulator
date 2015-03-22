

#include "Application.h"
#include "MP_8085.h"

int main(int argc, char* argv[])
{
    /*InstructionParser_8085 parser;
    
    MPSystem_8085 mp;
    mp.Test(parser.ParseFile("test.asm"));
    std::cin.get();*/
    
    Application app;
    app.Initialize(&argc, &argv);
    app.Run();
    return 0;
}

#ifdef _WIN32
#pragma comment(lib, "gtk-win32-3.0.lib")
#pragma comment(lib, "gobject-2.0.lib")
//#pragma comment(lib, "cairo.lib")
#pragma comment(lib, "glib-2.0.lib")
#pragma comment(lib, "gdk-win32-3.0.lib")
#endif
