/**
 * SoundEngine.cpp source file
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

#include "SoundEngine.h"


CSoundEngine::CSoundEngine(void)
{
    // init the members
    m_pDS = NULL;
    m_pDSBPrimary = NULL;
    m_hWnd = NULL;
    m_iOriginalMasterVolume = 0;
}


CSoundEngine::~CSoundEngine(void)
{
}


// Create
// initialise sound engine. Parameters are
// used to set up the primary mixing buffer
HRESULT CSoundEngine::Create(    HWND hWnd,
                                DWORD dwFrequency,
                                DWORD dwBitsPerSample,
                                DWORD dwChannels,
                                DWORD dwFlags)
{
    m_hWnd = hWnd;

    // create direct sound interface
    HRESULT    hres;
    hres = ::DirectSoundCreate8(NULL, &m_pDS, NULL);
    if (FAILED(hres))
    {
        return hres;
    }

    // set the sound co-op level
    // DSSCL_EXCLUSIVE
    // DSSCL_NORMAL
    // DSSCL_PRIORITY
    // DSSCL_WRITEPRIMARY
    hres = m_pDS->SetCooperativeLevel(m_hWnd, DSSCL_PRIORITY);
    if (FAILED(hres))
    {
        Release();
        return hres;
    }

    // create primary sound buffer
    DSBUFFERDESC    desc;
    ::memset(&desc, 0, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags =    DSBCAPS_PRIMARYBUFFER |
                    DSBCAPS_CTRLPAN |
                    DSBCAPS_CTRLVOLUME;

    hres = m_pDS->CreateSoundBuffer(&desc, &m_pDSBPrimary, NULL);
    if (FAILED(hres))
    {
        Release();
        return hres;
    }

    // set the main mixing format
    WAVEFORMATEX            wfx;
    ::memset(&wfx, 0, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = (WORD)dwChannels;
    wfx.nSamplesPerSec = dwFrequency;
    wfx.wBitsPerSample = (WORD)dwBitsPerSample;
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    hres = m_pDSBPrimary->SetFormat(&wfx);
    if (FAILED(hres))
    {
        Release();
        return hres;
    }

    // get the original master volume
    m_iOriginalMasterVolume = GetMasterVolume();

    // set the primary buffer playing
    m_pDSBPrimary->Play(0, 0, DSBPLAY_LOOPING);
    return S_OK;
}


// Release
void CSoundEngine::Release(void)
{
    if (m_pDSBPrimary)
    {
        m_pDSBPrimary->Release();
        m_pDSBPrimary = NULL;
    }

    if (m_pDS)
    {
        m_pDS->Release();
        m_pDS = NULL;
    }
    m_hWnd = NULL;
    m_iOriginalMasterVolume = 0;
}


// GetMasterVolume
// returns currently set master volume
int CSoundEngine::GetMasterVolume(void)
{
    if (m_pDSBPrimary)
    {
        long lVolume;
        m_pDSBPrimary->GetVolume(&lVolume);
        return lVolume;
    }
    return DSBVOLUME_MIN;
}


// SetMasterVolume
// set volume of the entire sound system
// iVolume is value in range 0 to -10000
// where 0 is maximum volume and -10000 is mute
void CSoundEngine::SetMasterVolume(int iVolume)
{
    // sanity check
    if (iVolume > DSBVOLUME_MAX)
    {
        iVolume = DSBVOLUME_MAX;
    }
    else if (iVolume < DSBVOLUME_MIN)
    {
        iVolume = DSBVOLUME_MIN;
    }

    if (m_pDSBPrimary)
    {
        m_pDSBPrimary->SetVolume(iVolume);
    }
}


