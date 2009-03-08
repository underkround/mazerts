#pragma once
 
#include "InputEngine.h"
 
class IInputDevice
{
public:
    IInputDevice(void);
    virtual ~IInputDevice(void);
 
    virtual void Release(void);
 
    virtual HRESULT Update(void) = 0;
    virtual BOOL GetButton(DWORD dwButtonIndex) const = 0;
 
    inline LPDIRECTINPUTDEVICE8 GetDevice() { return m_pDevice; }
    inline DWORD GetButtonCount() const { return m_dwButtonCount; }
 
protected:
    LPDIRECTINPUTDEVICE8    m_pDevice;
    DWORD                    m_dwButtonCount;
};