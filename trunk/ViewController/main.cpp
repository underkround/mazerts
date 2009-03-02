/*
 * main.cpp
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 26.1.2009
 *
 * Entry point for a Windows application
 * 
 */

#include "App/TheApp.h"


int APIENTRY WinMain(    HINSTANCE hInstance,
                        HINSTANCE hPreviousInstance,
                        LPSTR strCmdLine,
                        int iShowCmd)
{
    HRESULT hres;
    CTheApp    theApp;

    // create application
    hres = theApp.Create(    1000,
                            600,
                            32,
                            TRUE,
                            _T("Game Engine - Texturing"),
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


