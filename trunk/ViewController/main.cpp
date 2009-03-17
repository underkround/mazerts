/*
 * main.cpp
 *
 * Entry point for a Windows application
 * 
 */

#define MAZERTS_DX 1

#include "App/TheApp.h"

int APIENTRY WinMain(   HINSTANCE hInstance,
                        HINSTANCE hPreviousInstance,
                        LPSTR strCmdLine,
                        int iShowCmd)
{
    HRESULT hres;
    CTheApp    theApp;

    // create application
    hres = theApp.Create(    1024,
                            768,
                            32,
                            TRUE,
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