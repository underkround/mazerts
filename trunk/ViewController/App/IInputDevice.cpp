/**
 * IInputDevice
 *
 * abstact base class for all input devices
 */
#include "IInputDevice.h"
 
IInputDevice::IInputDevice(void)
{
	//init members
	m_pDevice = NULL;
	m_dwButtonCount = 0;
}
 
IInputDevice::~IInputDevice(void)
{
}
 
void IInputDevice::Release(void)
{
	if(m_pDevice)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}
	m_dwButtonCount = 0;
}