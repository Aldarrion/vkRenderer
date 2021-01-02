#include "EngineMain.h"

#if HS_WINDOWS
    //------------------------------------------------------------------------------
    int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd)
    {
        int result = hs::EngineMainWin32(instance, prevInstance, cmdLine, showCmd);
        return result;
    }
#elif HS_LINUX
    //------------------------------------------------------------------------------
    int main(int argc, char** argv)
    {
        int result = hs::EngineMainLinux(argc, argv);
        return result;
    }
#endif
