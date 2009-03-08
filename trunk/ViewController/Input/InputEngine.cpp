#include "InputEngine.h"
 
CInputEngine::CInputEngine(void)
{
    m_pDI = NULL;
    m_hWnd = NULL;
}
 
CInputEngine::~CInputEngine(void)
{
}
 
 
HRESULT CInputEngine::Create(HWND hWnd)
{
    m_hWnd = hWnd;
    return DirectInput8Create(    ::GetModuleHandle(NULL),
                                0x800,
                                IID_IDirectInput8,
                                (void**)&m_pDI,
                                NULL);
}
 
void CInputEngine::Release(void)
{
    if(m_pDI)
    {
        m_pDI->Release();
        m_pDI = NULL;
    }
    m_hWnd = NULL;
}
 
int CInputEngine::GetKeyboards(vector<DEVICE>& arrDevices)
{
    if(m_pDI == NULL) //dinput not initialized
    {
        return 0;
    }
 
    const int count = arrDevices.size();
    m_pDI->EnumDevices( DI8DEVCLASS_KEYBOARD,
        EnumDevicesCallback,
        &arrDevices,
        DIEDFL_ATTACHEDONLY);
 
    return arrDevices.size() - count;
}
 
int CInputEngine::GetMice(vector<DEVICE>& arrDevices)
{
    if(m_pDI == NULL) //dinput not initialized
    {
        return 0;
    }
 
    const int count = arrDevices.size();
    m_pDI->EnumDevices( DI8DEVCLASS_POINTER,
        EnumDevicesCallback,
        &arrDevices,
        DIEDFL_ATTACHEDONLY);
 
    return arrDevices.size() - count;
}
 
int CInputEngine::EnumDevicesCallback(  LPCDIDEVICEINSTANCE pInstance, 
                                        void* pParameters)
{
    // get the parameter array
    vector<DEVICE>* parrDevices = (vector<DEVICE>*)pParameters;
 
    DEVICE        device;
    // don't use this device.gdDevice = pInstance->guidInstance; causes problems
    ::memcpy(&device.gdDevice, &pInstance->guidInstance, sizeof(GUID));
    ::memset(device.strName, 0, sizeof(TCHAR) * 256);
    ::_tcscpy_s(device.strName, pInstance->tszProductName); //microsoft secure tchar copy ;)
 
    parrDevices->push_back(device);
    return DIENUM_CONTINUE;
}