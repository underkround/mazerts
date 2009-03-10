/**
 * SoundEngine.h header file
 * Implementation for the CSoundEngine class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 16.2.2009
 * 
 * The sound engine with Direct Sound
 *
 * Requires:
 * Project wide define:
 * DIRECTSOUND_VERSION=0x900
 *
 * Libs:
 * dsound.lib winmm.lib
 */


#ifndef __SOUNDENGINE_H__
#define __SOUNDENGINE_H__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>
#include <dsound.h>


class CSoundEngine
{
public:
	CSoundEngine(void);
	virtual ~CSoundEngine(void);

	/**
	 * Create
	 * initialise sound engine. Parameters are
	 * used to set up the primary mixing buffer
	 * @param hWnd application main window
	 * @param dwFrequency main frequency
	 * @param dwBitsPerSample 8,16...
	 * @param dwChannels 1 for mono, 2 stereo
	 * @param dwFlags startup flags
	 * @return S_OK or an error code
	 */
	HRESULT Create(	HWND hWnd,
					DWORD dwFrequency,
					DWORD dwBitsPerSample,
					DWORD dwChannels,
					DWORD dwFlags);

	/**
	 * Release
	 * release the sound engine
	 */
	void Release(void);

	/**
	 * GetMasterVolume
	 * @return currently set master volume
	 */
	int GetMasterVolume(void);

	/**
	 * SetMasterVolume
	 * set volume of the entire sound system
	 * @param iVolume is value in range 0 to -10000
	 * where 0 is maximum volume and -10000 is mute
	 */
	void SetMasterVolume(int iVolume);

	/**
	 * GetDirectSound
	 * @return DirectSound object
	 */
	inline LPDIRECTSOUND8 GetDirectSound(void) { return m_pDS; }

	/**
	 * GetPrimaryBuffer
	 * @return primary mixing buffer
	 */
	inline LPDIRECTSOUNDBUFFER GetPrimaryBuffer(void) { return m_pDSBPrimary; }

	/**
	 * GetWindow
	 * @return handle to associated window
	 */
	inline HWND GetWindow(void) { return m_hWnd; }

private:
	LPDIRECTSOUND8			m_pDS;
	LPDIRECTSOUNDBUFFER		m_pDSBPrimary;

	HWND					m_hWnd;
	int						m_iOriginalMasterVolume;
};


#endif	// __SOUNDENGINE_H__