/**
 * SoundMP3Player.cpp source file
 * Implementation for the CSoundMP3Player class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 16.2.2009
 * 
 * A simple MP3 player class
 *
 * Requires:	strmiids.lib
 * DirectShow component from
 * WindowsPlatform SDK.
 * CoInitialize(NULL) must be called
 * before using this class
 */

#include "SoundMP3Player.h"

CSoundMP3Player::CSoundMP3Player(void)
{
	// init the members
	m_eState = eSTATE_STOPPED;
	m_pGraph = NULL;
	m_pPosition = NULL;
}


CSoundMP3Player::~CSoundMP3Player(void)
{
}


// Create
// initialise player to play a mp3 file
HRESULT CSoundMP3Player::Create(LPCTSTR strFilename)
{
	HRESULT hres;

	// create the graph builder COM object
	hres = ::CoCreateInstance(	CLSID_FilterGraph,
								NULL,
								CLSCTX_INPROC_SERVER,
								IID_IGraphBuilder,
								(void**)&m_pGraph);
	if (FAILED(hres))
	{
		return hres;
	}

	// the mp3 filename MUST be 16 bit unicode string,
	// regardless of build type... convert if necessary
	WCHAR filename[MAX_PATH];

	#ifdef _UNICODE
	wcscpy_s(filename, strFilename);
	#else
	// convert 8 bit ansi string into the 16bit
	// unicode
	::MultiByteToWideChar(	CP_ACP,
							0,
							strFilename,
							-1,
							filename,
							MAX_PATH);
	#endif

	// start the player
	hres = m_pGraph->RenderFile(filename, NULL);
	if (FAILED(hres))
	{
		Release();
		return hres;
	}

	// Create the media position interface
	// this is used to control playback position
	hres = m_pGraph->QueryInterface(	IID_IMediaPosition,
										(void**)&m_pPosition);
	if (FAILED(hres))
	{
		Release();
		return hres;
	}

	m_eState = eSTATE_STOPPED;
	return S_OK;
}


// Release
void CSoundMP3Player::Release(void)
{
	if (m_pPosition)
	{
		m_pPosition->Release();
		m_pPosition = NULL;
	}
	if (m_pGraph)
	{
		m_pGraph->Release();
		m_pGraph = NULL;
	}
	m_eState = eSTATE_STOPPED;
}


// Play
// start playing the mp3
void CSoundMP3Player::Play(void)
{
	// sanity check
	if (!m_pGraph || m_eState == eSTATE_PLAYING)
	{
		return;
	}

	HRESULT	hres;
	IMediaControl*		pMC = NULL;

	hres = m_pGraph->QueryInterface(	IID_IMediaControl,
										(void**)&pMC);
	if (FAILED(hres))
	{
		return;
	}

	pMC->Run();
	pMC->Release();
	m_eState = eSTATE_PLAYING;
}


// Stop
// stop playing the mp3
void CSoundMP3Player::Stop(void)
{
	// sanity check
	if (!m_pGraph || m_eState == eSTATE_STOPPED)
	{
		return;
	}

	HRESULT	hres;
	IMediaControl*		pMC = NULL;

	hres = m_pGraph->QueryInterface(	IID_IMediaControl,
										(void**)&pMC);
	if (FAILED(hres))
	{
		return;
	}

	pMC->Stop();
	pMC->Release();
	SetPosition(0.0);
	m_eState = eSTATE_STOPPED;
}


// Pause
// pause the mp3 playback
void CSoundMP3Player::Pause(void)
{
	// sanity check
	if (!m_pGraph || m_eState != eSTATE_PLAYING)
	{
		return;
	}

	HRESULT	hres;
	IMediaControl*		pMC = NULL;

	hres = m_pGraph->QueryInterface(	IID_IMediaControl,
										(void**)&pMC);
	if (FAILED(hres))
	{
		return;
	}

	pMC->Pause();
	pMC->Release();
	m_eState = eSTATE_PAUSE;
}


// GetPosition
// get the playback position
double CSoundMP3Player::GetPosition(void)
{
	if (!m_pPosition)
	{
		return 0.0;
	}

	double dPosition = 0.0;
	m_pPosition->get_CurrentPosition(&dPosition);
	return dPosition;
}


// SetPosition
// set the playback position
void CSoundMP3Player::SetPosition(double dPosition)
{
	if (!m_pPosition)
	{
		return;
	}

	m_pPosition->put_CurrentPosition(dPosition);
}


// GetDuration
// returns the duration (time) of mp3
double CSoundMP3Player::GetDuration(void)
{
	if (!m_pPosition)
	{
		return 0.0;
	}

	double dDuration = 0.0;
	m_pPosition->get_Duration(&dDuration);
	return dDuration;
}


// SetVolume
// set the volume of mp3
// iVolume parameter is in range of
// 0 to -10000, where 0 is max volume
// and -10000 is mute
void CSoundMP3Player::SetVolume(int iVolume)
{
	// sanity check
	if (!m_pGraph)
	{
		return;
	}
	if (iVolume > 0)
	{
		iVolume = 0;
	}
	else if (iVolume < -10000)
	{
		iVolume = -10000;
	}

	HRESULT hres;
	IBasicAudio*	pBA = NULL;
	hres = m_pGraph->QueryInterface(	IID_IBasicAudio,
										(void**)&pBA);
	if (FAILED(hres))
	{
		return;
	}

	pBA->put_Volume(iVolume);
	pBA->Release();
}


// GetVolume
// returns current mp3 volume
int CSoundMP3Player::GetVolume(void)
{
	if (!m_pGraph)
	{
		return -10000;
	}

	HRESULT hres;
	IBasicAudio*	pBA = NULL;
	hres = m_pGraph->QueryInterface(	IID_IBasicAudio,
										(void**)&pBA);
	if (FAILED(hres))
	{
		return -10000;
	}

	long lVolume;
	pBA->get_Volume(&lVolume);
	pBA->Release();
	return lVolume;
}


// IsAtEnd
// returns TRUE if playback has reached the end of file
BOOL CSoundMP3Player::IsAtEnd(void)
{
	// sanity check
	if (!m_pGraph || m_eState != eSTATE_PLAYING)
	{
		return FALSE;
	}

	HRESULT hres;
	IMediaEvent*		pME = NULL;

	hres = m_pGraph->QueryInterface(	IID_IMediaEvent,
										(void**)&pME);
	if (FAILED(hres))
	{
		return FALSE;
	}


	long lEvent = 0;
	hres = pME->WaitForCompletion(0, &lEvent);
	pME->Release();
	if (FAILED(hres))
	{
		return FALSE;
	}

	if (lEvent)
	{
		m_eState = eSTATE_STOPPED;
		return TRUE;
	}
	return FALSE;
}


