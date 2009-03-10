/**
 * SoundMP3Player.h header file
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


#ifndef __SOUNDMP3PLAYER_H__
#define __SOUNDMP3PLAYER_H__

// include the basic windows stuff
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

#include <dshow.h>


class CSoundMP3Player
{
public:
	// player state
	enum eSTATE
	{
		eSTATE_STOPPED=0,
		eSTATE_PLAYING,
		eSTATE_PAUSE,
		eSTATE_FORCEDWORD=0xffffffff
	};

	CSoundMP3Player(void);
	virtual ~CSoundMP3Player(void);

	/**
	 * Create
	 * initialise player to play a mp3 file
	 * @param strFilename file to load
	 * @return S_OK or an error code
	 */
	HRESULT Create(LPCTSTR strFilename);

	/**
	 * Release
	 * release the mp3 player
	 */
	void Release(void);

	/**
	 * Play
	 * start playing the mp3
	 */
	void Play(void);

	/**
	 * Stop
	 * stop playing the mp3
	 */
	void Stop(void);

	/**
	 * Pause
	 * pause the mp3 playback
	 */
	void Pause(void);

	/**
	 * GetPosition
	 * get the playback position
	 * @return playback position in range 0.0 - 1.0
	 */
	double GetPosition(void);

	/**
	 * SetPosition
	 * set the playback position
	 * @param dPosition new position in range 0.0 - 1.0
	 */
	void SetPosition(double dPosition);

	/**
	 * GetDuration
	 * @return the duration (seconds) of mp3
	 */
	double GetDuration(void);

	/**
	 * SetVolume
	 * set the volume of mp3
	 * @param iVolume range of
	 * 0 to -10000, where 0 is max volume
	 * and -10000 is mute
	 */
	void SetVolume(int iVolume);

	/**
	 * GetVolume
	 * @return current mp3 volume
	 */
	int GetVolume(void);

	/** 
	 * IsAtEnd
	 * @return TRUE if playback has reached the end of file
	 */
	BOOL IsAtEnd(void);

	/**
	 * GetState
	 * @return current state of the player
	 */
	inline eSTATE GetState(void) const { return m_eState; }

private:
	IGraphBuilder*		m_pGraph;
	IMediaPosition*		m_pPosition;
	eSTATE				m_eState;
};

#endif	// __SOUNDMP3PLAYER_H__

