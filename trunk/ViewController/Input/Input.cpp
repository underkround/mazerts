#include "Input.h"

CInputEngine Input::inputEngine;
CInputMouse Input::mouse;
CInputKeyboard Input::keyboard;
IApplication* Input::pApp = NULL;
std::vector<CInputEngine::DEVICE> Input::arrDevices;

HRESULT Input::create(IApplication* pApplication)
{
    HRESULT hres;

    pApp = pApplication;

    //Initialize input engine
    hres = inputEngine.Create(pApp->GetWindow());

    if(FAILED(hres))
    {
        return hres;
    }

    //Enumerate attached input devices
    if(!inputEngine.GetMice(arrDevices) || !inputEngine.GetKeyboards(arrDevices))
    {
        //No pointer- or keyboard-device found?
        return ERROR_DEVICE_NOT_AVAILABLE;
    }

    hres = mouse.Create(inputEngine, NULL, 0);
    if(FAILED(hres))
    {
        return hres;
    }
    hres = keyboard.Create(inputEngine, NULL, 0);
    if(FAILED(hres))
    {
        return hres;
    }

    MouseState::create(pApp);
    KeyboardState::create();

    return S_OK;
} 

void Input::update()
{
    MouseState::update();
    KeyboardState::update();
}

void Input::release()
{
    KeyboardState::release();
    MouseState::release();
    mouse.Release();
    keyboard.Release();
    inputEngine.Release();
}