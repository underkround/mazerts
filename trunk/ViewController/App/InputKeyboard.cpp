#include "InputKeyboard.h"
 
CInputKeyboard::CInputKeyboard(void)
{
	//init members
	::memset(m_byKeys, 0, 256);
}
 
CInputKeyboard::~CInputKeyboard(void)
{
}
 
HRESULT CInputKeyboard::Create(	CInputEngine& inputEngine, CInputEngine::DEVICE* pDevice, DWORD dwFlags)
{
	LPDIRECTINPUT8 pDI = inputEngine.GetInput();
	if(pDI == NULL) 
	{
		// invalid user
		return E_FAIL;
	}
 
	HRESULT hres;
	GUID gdDevice;
 
	gdDevice = GUID_SysKeyboard; // default keyboard
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
	hres = m_pDevice->SetDataFormat(&c_dfDIKeyboard);
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
		// you don't need to agree with immonen about these flags!
	}
 
	hres = m_pDevice->SetCooperativeLevel( inputEngine.GetWindow(), dwFlags);
	if(FAILED(hres))
	{
		Release();
		return hres;
	}
 
	m_dwButtonCount = 256;
 
	//acquire the input device
	m_pDevice->Acquire();
	::memset(m_byKeys, 0, 256);
 
	return S_OK;
}
 
HRESULT CInputKeyboard::Update(void)
{
	if(m_pDevice == NULL) 
	{
		// invalid user..
		return E_FAIL;
	}
 
	HRESULT hres;
 
	//ask keyboard state
	hres = m_pDevice->GetDeviceState(256, m_byKeys);
	if(FAILED(hres)) //failed to get input device status
	{
		//most likely cause for this to happen is that we have lost the focus
		//of keyboard to some other application
		//try to acquire focus back to us
		m_pDevice->Acquire();
 
		//ask device state again
		hres = m_pDevice->GetDeviceState(256, m_byKeys);
		if(FAILED(hres)) //still failed
		{
			::memset(m_byKeys, 0, 256);
			return hres;
		}
	}
 
	return S_OK;
}
 
BOOL CInputKeyboard::GetButton(DWORD dwButtonIndex) const
{
	return m_byKeys[dwButtonIndex] ? TRUE : FALSE;
}