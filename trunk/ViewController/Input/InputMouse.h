#pragma once
#include "iinputdevice.h"
 
class CInputMouse : public IInputDevice
{
public:
    CInputMouse(void);
    virtual ~CInputMouse(void);
 
    HRESULT Create(    CInputEngine& inputEngine,
            CInputEngine::DEVICE* pDevice, //pointer instead of reference so we can send null to send default keyboard
            DWORD dwFlags);
 
    virtual HRESULT Update(void);
 
    virtual BOOL GetButton(DWORD dwButtonIndex) const;
 
    /**
     * GetState
     * @return reference to mouse state structure
     */
    inline DIMOUSESTATE2& GetState() { return m_State; }
 
private:
    /**
     * CountButtons
     * @return number of buttons on this input device
     */
    DWORD CountButtons(void);
 
    DIMOUSESTATE2            m_State;  //direct input mouse state version 2
};