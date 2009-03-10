/**
 * SoundWave.cpp source file
 * Implementation for the CSoundWave class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 16.2.2009
 * 
 * This class implements the wave sound
 * capable of loading itself from the file
 * and to use duplicates to play same sound
 * multiple times simultaneusly
 */

#include "SoundWave.h"
#include "WaveLoader.h"


CSoundWave::CSoundWave(void)
{
	// init the members
	m_ppDSB = NULL;
	m_dwBufferCount = 0;
}



CSoundWave::~CSoundWave(void)
{
}


// Create
// create 'empty' sound buffer into the memory
HRESULT CSoundWave::Create(	CSoundEngine& soundEngine,
							DWORD dwFrequency, 
							DWORD dwBitsPerSample,
							DWORD dwChannels,
							DWORD dwBytes,
							DWORD dwDuplicates,
							DWORD dwFlags)
{
    // set original freq
    m_OriginalFrequency = dwFrequency;

	HRESULT			hres;
	WAVEFORMATEX	wfx;
	DSBUFFERDESC	desc;

	// init the wave format
	::memset(&wfx, 0, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)dwChannels;
	wfx.nSamplesPerSec = dwFrequency;
	wfx.wBitsPerSample = (WORD)dwBitsPerSample;
	wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	// init the buffer desc
	::memset(&desc, 0, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = dwFlags;
	desc.dwBufferBytes = dwBytes;
	desc.lpwfxFormat = &wfx;

	// create sound buffer(s)...
	LPDIRECTSOUND8	pDS = soundEngine.GetDirectSound();
	m_ppDSB = new LPDIRECTSOUNDBUFFER8[dwDuplicates + 1];
	if (!m_ppDSB)
	{
		return E_OUTOFMEMORY;
	}

	// set pointers in array to NULL
	::memset(m_ppDSB, 0, sizeof(LPDIRECTSOUNDBUFFER8) * (dwDuplicates + 1));
	m_dwBufferCount = dwDuplicates + 1;


	// create the first sound buffer
	// first sound buffer is special one,
	// it is the only one that actually contains
	// any sound data. The duplicate buffers have only
	// separate play positions, pointing into the data
	// in first buffer
	LPDIRECTSOUNDBUFFER		pDSB = NULL;
	hres = pDS->CreateSoundBuffer(&desc, &pDSB, NULL);
	if (FAILED(hres))
	{
		Release();
		return hres;
	}

	// query the latest sound buffer interface
	hres = pDSB->QueryInterface(	IID_IDirectSoundBuffer8,
									(void**)&m_ppDSB[0]);
	pDSB->Release();
	if (FAILED(hres))
	{
		Release();
		return hres;
	}


	// create the duplicates
	DWORD i;
	for (i=0; i<dwDuplicates; i++)
	{
		if ((dwFlags & DSBCAPS_CTRLFX))
		{
			// real time effects required, all duplicate buffers
			// must have their own wave data
			hres = pDS->CreateSoundBuffer(&desc, &pDSB, NULL);
		}
		else
		{
			// no real time effect required, use duplicate buffers
			// to save memory
			hres = pDS->DuplicateSoundBuffer(m_ppDSB[0], &pDSB);
		}

		if (SUCCEEDED(hres))
		{
			hres = pDSB->QueryInterface(	IID_IDirectSoundBuffer8,
											(void**)&m_ppDSB[i + 1]);
			pDSB->Release();
			if (FAILED(hres))
			{
				Release();
				return hres;
			}
		}
		else
		{
			if (FAILED(hres))
			{
				Release();
				return hres;
			}
		}
	}

	return S_OK;
}


// Release
// release all sound buffers
void CSoundWave::Release(void)
{
	if (m_ppDSB)
	{
		DWORD i;
		for (i=0; i<m_dwBufferCount; i++)
		{
			if (m_ppDSB[i])
			{
				m_ppDSB[i]->Release();
				m_ppDSB[i] = NULL;
			}
		}

		delete m_ppDSB;
		m_ppDSB = NULL;
	}
	m_dwBufferCount = 0;
}


// Load
// load wave file into the buffer and create
// the duplicates
HRESULT CSoundWave::Load(	CSoundEngine& soundEngine,
							LPCTSTR strFilename,
							DWORD dwDuplicates,
							DWORD dwFlags)
{
	HRESULT			hres;
	CWaveLoader		loader;

	// try to open the file
	hres = loader.Open(	(LPTSTR)strFilename,
						NULL,
						WAVEFILE_READ);
	if (FAILED(hres))
	{
		// failed to open the file, maybe
		// file is not found, is unknown format or so...
		return hres;
	}


	// create the sound buffer(s)
	hres = Create(	soundEngine,
					loader.m_pwfx->nSamplesPerSec,
					loader.m_pwfx->wBitsPerSample,
					loader.m_pwfx->nChannels,
					loader.GetSize(),
					dwDuplicates,
					dwFlags);
	if (FAILED(hres))
	{
		loader.Close();
		return hres;
	}

	// load wave data into the buffer
	int i;
	int iCount = 1;
	if ((dwFlags & DSBCAPS_CTRLFX))
	{
		// real time effects required, load sound
		// data into every sound buffer
		iCount = m_dwBufferCount;
	}

	for (i=0; i<iCount; i++)
	{
		void* pBufferData = NULL;
		DWORD dwLength;

		hres = m_ppDSB[i]->Lock(	0,
									0,
									&pBufferData,
									&dwLength,
									NULL,
									NULL,
									DSBLOCK_ENTIREBUFFER);
		if (SUCCEEDED(hres))
		{
			// copy wave data into the buffer
			DWORD dwRead = 0;
			loader.ResetFile();
			loader.Read(	(BYTE*)pBufferData,
							dwLength,
							&dwRead);

			m_ppDSB[i]->Unlock(pBufferData, dwLength, NULL, 0);
		}
	}

	loader.Close();
	return S_OK;
}


// Play
// play the wave file or its duplicate
void CSoundWave::Play(BOOL bLoop, DWORD dwDuplicate)
{
	DWORD dwFlags = 0;
	if (bLoop)
	{
		dwFlags = DSBPLAY_LOOPING;
	}

	if (dwDuplicate == NEXT_FREE_DUPLICATE)
	{
		dwDuplicate = GetNextFreeDuplicate();
	}

	m_ppDSB[dwDuplicate]->Play(0, 0, dwFlags);
}


// GetNextFreeDuplicate
// returns index of duplicate buffer that
// is currently not playing. In case of
// all buffers already playing, return 0
DWORD CSoundWave::GetNextFreeDuplicate(void)
{
	DWORD i;
	for (i=0; i<m_dwBufferCount; i++)
	{
		if (IsPlaying(i) == FALSE)
		{
			return i;
		}
	}
	return 0;
}


// IsPlaying
// returns TRUE if certain buffer is playing
BOOL CSoundWave::IsPlaying(DWORD dwDuplicate)
{
	DWORD dwFlags;
	m_ppDSB[dwDuplicate]->GetStatus(&dwFlags);
	return (dwFlags & DSBSTATUS_PLAYING) ? TRUE : FALSE;
}


// Stop
// stop the wave playback
void CSoundWave::Stop(DWORD dwDuplicate)
{
	m_ppDSB[dwDuplicate]->Stop();
}


// RePlay
// start wave playback from the beginning of sound
void CSoundWave::RePlay(BOOL bLoop, DWORD dwDuplicate)
{
	if (dwDuplicate == NEXT_FREE_DUPLICATE)
	{
		dwDuplicate = GetNextFreeDuplicate();
	}
	Stop(dwDuplicate);
	m_ppDSB[dwDuplicate]->SetCurrentPosition(0);
	Play(bLoop, dwDuplicate);
}


// IsLooping
// returns TRUE if wave is playing in looping mode
BOOL CSoundWave::IsLooping(DWORD dwDuplicate)
{
	DWORD dwFlags;
	m_ppDSB[dwDuplicate]->GetStatus(&dwFlags);
	return (dwFlags & DSBSTATUS_LOOPING) ? TRUE : FALSE;
}


// MillisecondsToBytes
// converts given time value into the position
// in sound buffer
int CSoundWave::MillisecondsToBytes(int iMS)
{
	WAVEFORMATEX	wfx;
	m_ppDSB[0]->GetFormat(&wfx, sizeof(WAVEFORMATEX), NULL);
	return iMS * wfx.nAvgBytesPerSec / 1000;
}


// BytesToMilliseconds
// converts given buffer position into the time value
int CSoundWave::BytesToMilliseconds(int iBytes)
{
	WAVEFORMATEX	wfx;
	m_ppDSB[0]->GetFormat(&wfx, sizeof(WAVEFORMATEX), NULL);
	return iBytes * 1000 / wfx.nAvgBytesPerSec;
}


// GetPosition
// returns playback position in milliseconds
DWORD CSoundWave::GetPosition(DWORD dwDuplicate)
{
	DWORD dwPosition;
	m_ppDSB[dwDuplicate]->GetCurrentPosition(&dwPosition, NULL);
	return BytesToMilliseconds(dwPosition);
}


// SetPosition
// set the playback position in milliseconds
void CSoundWave::SetPosition(DWORD dwPosition, DWORD dwDuplicate)
{
	dwPosition = MillisecondsToBytes(dwPosition);
	m_ppDSB[dwDuplicate]->SetCurrentPosition(dwPosition);
}


// SetFrequency
// set the playback speed
void CSoundWave::SetFrequency(DWORD dwFrequency, DWORD dwDuplicate)
{
	m_ppDSB[dwDuplicate]->SetFrequency(dwFrequency);
}


// GetFrequency
// get the current playback speed
DWORD CSoundWave::GetFrequency(DWORD dwDuplicate)
{
	DWORD dwFrequency;
	m_ppDSB[dwDuplicate]->GetFrequency(&dwFrequency);
	return dwFrequency;
}


// GetVolume
// returns current wave playback volume
// volume value is in between 0 to -10000
// where 0 is max volume, and -10000 is mute
int CSoundWave::GetVolume(DWORD dwDuplicate)
{
	long lVolume;
	m_ppDSB[dwDuplicate]->GetVolume(&lVolume);
	return lVolume;
}


// SetVolume
// set wave playback volume
// volume value is in between 0 to -10000
// where 0 is max volume, and -10000 is mute
void CSoundWave::SetVolume(int iVolume, DWORD dwDuplicate)
{
	m_ppDSB[dwDuplicate]->SetVolume(iVolume);
}


// GetPan
// returns current playback panning value
// panning is:
// -10000		full left
// 0			center
// 10000		full right
int CSoundWave::GetPan(DWORD dwDuplicate)
{
	long lPan;
	m_ppDSB[dwDuplicate]->GetPan(&lPan);
	return lPan;
}


// SetPan
// set the playback panning value
// panning is:
// -10000		full left
// 0			center
// 10000		full right
void CSoundWave::SetPan(int iPan, DWORD dwDuplicate)
{
	m_ppDSB[dwDuplicate]->SetPan(iPan);
}


// SetEffect
// set some standard environment effects into the wave
// Wave must be created with DSBCAPS_CTRLFX flag
HRESULT CSoundWave::SetEffect(GUID gdEffect, DWORD dwDuplicate)
{
	// we cannot set any effects while sound is playing
	Stop(dwDuplicate);

	// check if parameter is GUID_NULL
	if (::IsEqualGUID(gdEffect, GUID_NULL))
	{
		// no effect specified, stop any previous effects
		return m_ppDSB[dwDuplicate]->SetFX(0, NULL, NULL);
	}

	// fill effect description structure
	DSEFFECTDESC	desc;
	::memset(&desc, 0, sizeof(DSEFFECTDESC));
	desc.dwSize = sizeof(DSEFFECTDESC);
	desc.guidDSFXClass = gdEffect;

	// activate the effect
	return m_ppDSB[dwDuplicate]->SetFX(1, &desc, NULL);
}


