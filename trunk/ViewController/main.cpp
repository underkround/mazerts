/*
 * main.cpp
 *
 * Entry point for a Windows application
 * 
 */

#define MAZERTS_DX 1

#include "App/TheApp.h"
#include "../Model/Common/Config.h"

int APIENTRY WinMain(   HINSTANCE hInstance,
                        HINSTANCE hPreviousInstance,
                        LPSTR strCmdLine,
                        int iShowCmd)
{
    HRESULT hres;
    CTheApp    theApp;

    // Load basic config
    Config& c = *Config::getInstance();
    c.setFilename("../data/config.ini");
    c.readFile();
    int resw    = c.getValueAsInt("screen width",  1024);
    int resh    = c.getValueAsInt("screen height",   768);
    int bpp     = c.getValueAsInt("screen bpp",  32);
    bool windowed = c.getValueAsBool("screen windowed",   true);

    // create application
    hres = theApp.Create(   resw,
                            resh,
                            bpp,
                            windowed,
                            _T("Maze Game Engine"),
                            0);
    if (FAILED(hres))
    {
        // application failed to start
        ::MessageBox(    NULL,
                        _T("Failed to start game application"),
                        _T("ERROR"),
                        MB_OK);
        return -1;
    }

    // run the application
    int retval = theApp.Run();
    theApp.Release();
    return retval;
}
