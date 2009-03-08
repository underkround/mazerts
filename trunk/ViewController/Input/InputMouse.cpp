#include "InputMouse.h"
 
//as you notice this class is almost same as keyboard
 
CInputMouse::CInputMouse(void)
{
    // init members
    ::memset(&m_State, 0, sizeof(DIMOUSESTATE2)); //use memset to fill mouse state with zeroes
}
 
CInputMouse::~CInputMouse(void)
{
}
 
HRESULT CInputMouse::Create(    CInputEngine& inputEngine, CInputEngine::DEVICE* pDevice, DWORD dwFlags)
{
    LPDIRECTINPUT8 pDI = inputEngine.GetInput();
    if(pDI == NULL) 
    {
        // invalid user
        return E_FAIL;
    }
 
    HRESULT hres;
    GUID gdDevice;
 
    gdDevice = GUID_SysMouse; // default mouse
    if(pDevice)
    {
        // then override default if pDevice guid is specified by user
        gdDevice = pDevice->gdDevice;
    }
 
    // create direct input device object
    hres = pDI->CreateDevice( gdDevice,
                                                        &m_pDevice,
                                                        NULL);
    if(FAILED(hres))
    {
        return hres;
    }
 
    // set the device data format
    hres = m_pDevice->SetDataFormat(&c_dfDIMouse2);
    if(FAILED(hres))
    {
        Release();
        return hres;
    }
 
    if(dwFlags == 0) 
    {
        // if user didn't specify any flags, use some sort
        // of 'defaults'
 
        // what do these flags mean:
        // DISCL_NONEXCLUSIVE gives access to input device also to other applications
        // DISCL_FOREGROUND we get the input only when we are topmost window
        dwFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
    }
 
    hres = m_pDevice->SetCooperativeLevel( inputEngine.GetWindow(), dwFlags);
    if(FAILED(hres))
    {
        Release();
        return hres;
    }
 
    m_dwButtonCount = CountButtons();
 
    //acquire the input device
    m_pDevice->Acquire();
 
    return S_OK;
}
 
HRESULT CInputMouse::Update(void)
{
    if(m_pDevice == NULL) 
    {
        // invalid user..
        return E_FAIL;
    }
 
    HRESULT hres;
 
    //ask keyboard state
    hres = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_State);
    if(FAILED(hres)) //failed to get input device status
    {
        //most likely cause for this to happen is that we have lost the focus
        //of mouse to some other application
        //try to acquire focus back to us
        m_pDevice->Acquire();
 
        //ask device state again
        hres = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_State);
        if(FAILED(hres)) //still failed
        {
            ::memset(&m_State, 0, sizeof(DIMOUSESTATE2));
            return hres;
        }
    }
 
    return S_OK;
 
}
 
BOOL CInputMouse::GetButton(DWORD dwButtonIndex) const
{
    if(dwButtonIndex >= m_dwButtonCount)
    {
        return FALSE;
    }
    return m_State.rgbButtons[dwButtonIndex] ? TRUE : FALSE;
}
 
DWORD CInputMouse::CountButtons(void)
{
    DIDEVICEOBJECTINSTANCE        didoi;
 
    ::memset(&didoi, 0, sizeof(DIDEVICEOBJECTINSTANCE));
    didoi.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);
 
    DWORD dwButtons = 0;
 
    int i;
    for(i=0;i<32;i++) //let's try mouse with maximum of 32 buttons :D
    {
        // if GetObjectInfo fails, that button is not present
        if(SUCCEEDED(m_pDevice->GetObjectInfo(    &didoi,
                            DIMOFS_BUTTON0 + i,
                            DIPH_BYOFFSET)))
        {
            ++dwButtons;
        }
    }
    return dwButtons;
}