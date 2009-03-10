/**
 * SoundWave.h header file
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

#ifndef __SOUNDWAVE_H__
#define __SOUNDWAVE_H__

#include "SoundEngine.h"

// use this define as 'dwDuplicate' for 
// play functions to automatically play next
// available duplicate sound
#define NEXT_FREE_DUPLICATE		0x0fffffff


class CSoundWave
{
public:
	CSoundWave(void);
	virtual ~CSoundWave(void);

	/**
	 * Create
	 * create 'empty' sound buffer into the memory
	 * @param soundEngine reference to sound engine
	 * @param dwFrequency buffer frequency
	 * @param dwBitsPerSample (8,16)
	 * @param dwChannels (1=mono, 2=stereo)
	 * @param dwBytes size of buffer in bytes
	 * @param dwDuplicates max number of duplicates
	 * @param dwFlags creation flags
	 * @return S_OK or an error code
	 */
	HRESULT Create(	CSoundEngine& soundEngine,
					DWORD dwFrequency,
					DWORD dwBitsPerSample,
					DWORD dwChannels,
					DWORD dwBytes,
					DWORD dwDuplicates,
					DWORD dwFlags);

	/**
	 * Release
	 * release all sound buffers
	 */
	void Release(void);

	/**
	 * Load
	 * load wave file into the buffer and create
	 * the duplicates
	 * @param soundEngine reference to sound engine
	 * @param strFilename file to load
	 * @param dwDuplicates max number of duplicates
	 * @param dwFlags creation flags
	 * @return S_OK or an error code
	 */
	HRESULT Load(	CSoundEngine& soundEngine,
					LPCTSTR strFilename,
					DWORD dwDuplicates,
					DWORD dwFlags);

	/**
	 * Play
	 * play the wave file or its duplicate
	 * @param bLoop TRUE to play looping
	 * @param dwDuplicate index of the duplicate
	 */
	void Play(BOOL bLoop, DWORD dwDuplicate);

	/**
	 * Stop
	 * stop the wave playback
	 * @param dwDuplicate index of the duplicate
	 */
	void Stop(DWORD dwDuplicate);

	/**
	 * RePlay
	 * replay (start from beginning) the wave file or its duplicate
	 * @param bLoop TRUE to play looping
	 * @param dwDuplicate index of the duplicate
	 */
	void RePlay(BOOL bLoop, DWORD dwDuplicate);

	/**
	 * IsPlaying
	 * @param dwDuplicate index of the duplicate
	 * @return TRUE if certain buffer is playing
	 */
	BOOL IsPlaying(DWORD dwDuplicate);

	/**
	 * IsLooping
	 * @param dwDuplicate index of the duplicate
	 * @return TRUE if wave is playing in looping mode
	 */
	BOOL IsLooping(DWORD dwDuplicate);

	/**
	 * GetPosition
	 * @param dwDuplicate index of the duplicate
	 * @return playback position in milliseconds
	 */
	DWORD GetPosition(DWORD dwDuplicate);

	/**
	 * SetPosition
	 * set the playback position in milliseconds
	 * @param dwPosition position to set to
	 * @param dwDuplicate index of the duplicate
	 */
	void SetPosition(DWORD dwPosition, DWORD dwDuplicate);

	/**
	 * SetFrequency
	 * set the playback speed
	 * Wave must be created with DSBCAPS_CTRLFREQUENCY flag
	 * @param dwFrequency frequency to play
	 * @param dwDuplicate index of the duplicate
	 */
	void SetFrequency(DWORD dwFrequency, DWORD dwDuplicate);

	/**
	 * GetFrequency
	 * get the current playback speed
	 * Wave must be created with DSBCAPS_CTRLFREQUENCY flag
	 * @param dwDuplicate index of the duplicate
	 * @return current playback frequency
	 */
	DWORD GetFrequency(DWORD dwDuplicate);

	/**
	 * GetVolume
	 * Wave must be created with DSBCAPS_CTRLVOLUME flag
	 * volume value is in between 0 to -10000
	 * where 0 is max volume, and -10000 is mute
	 * @param dwDuplicate index of the duplicate
	 * @return current wave playback volume
	 */
	int GetVolume(DWORD dwDuplicate);

	/**
	 * SetVolume
	 * set wave playback volume
	 * Wave must be created with DSBCAPS_CTRLVOLUME flag
	 * volume value is in between 0 to -10000
	 * where 0 is max volume, and -10000 is mute
	 * @param iVolume volume to set
	 * @param dwDuplicate index of the duplicate
	 */
	void SetVolume(int iVolume, DWORD dwDuplicate);

	/**
	 * GetPan
	 * Wave must be created with DSBCAPS_CTRLPAN flag
	 * panning is:
	 * -10000		full left
	 * 0			center
	 * 10000		full right
	 * @param dwDuplicate index of the duplicate
	 * @return current playback panning value
	 */
	int GetPan(DWORD dwDuplicate);

	/**
	 * SetPan
	 * set the playback panning value
	 * Wave must be created with DSBCAPS_CTRLPAN flag
	 * panning is:
	 * -10000		full left
	 * 0			center
	 * 10000		full right
	 * @param iPan new pan value
	 * @param dwDuplicate index of the duplicate
	 */
	void SetPan(int iPan, DWORD dwDuplicate);

	/**
	 * GetBufferCount
	 * @return the number of buffers in this wave
	 */
	inline DWORD GetBufferCount(void) const { return m_dwBufferCount; }

	/**
	 * GetNextFreeDuplicate
	 * @return index of duplicate buffer that
	 * is currently not playing. In case of
	 * all buffers already playing, return 0
	 */
	DWORD GetNextFreeDuplicate(void);

	/**
	 * SetEffect
	 * set some standard environment effects into the wave
	 * Wave must be created with DSBCAPS_CTRLFX flag
	 * @param gdEffect guid of the effect
	 * @param dwDuplicate index of the duplicate
	 * @return S_OK or an error code
	 */
	HRESULT SetEffect(GUID gdEffect, DWORD dwDuplicate);

    /**
     * getOriginalFrequency
     * returns the original frequency of the wave
     */
    inline const DWORD getOriginalFrequency() const { return m_OriginalFrequency; }

private:
	/**
	 * MillisecondsToBytes
	 * converts given time value into the position
	 * in sound buffer
	 * @param iMS milliseconds
	 * @return bytes needed for 'iMS' of sound
	 */
	int MillisecondsToBytes(int iMS);

	/**
	 * BytesToMilliseconds
	 * converts given buffer position into the time value
	 * @param iBytes bytes
	 * @return milliseconds of sound with 'iBytes' 
	 */
	int BytesToMilliseconds(int iBytes);

	// direct sound buffer(s)
	LPDIRECTSOUNDBUFFER8*		m_ppDSB;
	DWORD						m_dwBufferCount;

    // original frequency
    DWORD                       m_OriginalFrequency;
};

#endif	// __SOUNDWAVE_H__
