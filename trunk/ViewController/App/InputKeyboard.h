#pragma once
#include "iinputdevice.h"
 
class CInputKeyboard : public IInputDevice
{
public:
	CInputKeyboard(void);
	virtual ~CInputKeyboard(void);
 
	HRESULT Create(	CInputEngine& inputEngine,
			CInputEngine::DEVICE* pDevice, //pointer instead of reference so we can send null to send default keyboard
			DWORD dwFlags);
 
	virtual HRESULT Update(void);
 
	virtual BOOL GetButton(DWORD dwButtonIndex) const;
 
private:
	BYTE	m_byKeys[256]; //maximum amount of buttons defined by directinput
};