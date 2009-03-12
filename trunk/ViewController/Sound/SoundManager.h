#pragma once

#include "SoundEngine.h"
#include "../App/IApplication.h"
#include "SoundWave.h"
#include "SoundMP3Player.h"
#include "../Camera/Camera.h"

#include <tchar.h>
#include <map>
#include <d3dx9.h>

#define DEFAULT_FREQUENCY 44100
#define DEFAULT_BPS 16
#define DEFAULT_CHANNELS 2
#define DEFAULT_FLAGS 0
#define DEFAULT_DUPLICATE_AMOUNT 0
#define HEARING_DISTANCE 300

#define MAX_TEXT_LENGTH 64

using namespace std;

class SoundManager
{
public:
    /**
     * Different kinds of sounds
     */
    enum SoundTypes 
    {
        EXPLOSION = 0,
        READY,
        __END = 0xff
    };
    
    /**
     * Different kinds of music
     */
    enum MusicTypes
    {
        BACKGROUND = 0,
        _END = 0xff
    };

    /**
     * Struct for storing sound
     */
    struct Sound
    {
	    TCHAR		text[MAX_TEXT_LENGTH];
	    SoundTypes	type;
    };

    /**
     * Struct for storing sound
     */
    struct Music
    {
	    TCHAR		text[MAX_TEXT_LENGTH];
	    MusicTypes	type;
    };





    virtual ~SoundManager(void);

    /**
     * Creates the manager and loads necessary sounds
     */
    static HRESULT create(IApplication* pApp);

    /**
     * Releases used sounds, music etc.
     */
    static void release();

    /**
     * Checks if sounds/music needs anything done.
     */
    static void update();

    /**
     * Gets the SoundManager-instance
     * @return Pointer to UI3DObjectManager
     */
    inline static SoundManager* getInstance() 
    { 
        static SoundManager instance;
        return &instance; 
    }





    /**
     * Plays sound. Uses max volume and center pan.
     * @param type      which sound to play
     * @param distort   how much sound is to be distorted (in percents)
     */
    static void playSound(const SoundTypes type, const float distort);

    /**
     * Plays sound with provided volume and pan.
     * @param type      which sound to play
     * @param distort   how much sound is to be distorted (in percents)
     * @param volume    how loud the sound is (0 = max, -10000 = mute)
     * @param pan       left<->right speaker, (-10k = full left, 10k full right)
     */
    static void playSound(const SoundTypes type, const float distort, const int volume, const int pan);

    /**
     * Plays sound. Calculates panning and volume according to camera & target.
     * @param type      which sound to play
     * @param distort   how much sound is to be distorted (in percents)
     * @param pSoundPos  where in world the sound is coming from
     * @param pCamera    pointer to camera
     */
    static void playSound(const SoundTypes type, const float distort, const D3DXVECTOR3* pSoundPos, Camera* pCamera);

    /**
     * Stops all currently playing sound effects.
     */
    static void stopSounds();





    /**
     * Plays music. If music is already played, stops that and starts playing new one.
     * @param type          which music to play
     * @param looping       if music should be looped
     */
    static void playMusic(const MusicTypes type, const bool looping);

    /**
     * Stops music playing
     */
    static void stopMusic();





    /**
	 * SetMasterVolume
	 * set volume of the entire sound system
	 * @param iVolume is value in range 0 to -10000
	 * where 0 is maximum volume and -10000 is mute
	 */
    inline static void setMasterVolume(const int volume) { getInstance()->m_SoundEngine.SetMasterVolume(volume); }
    inline static const int getMasterVolume() { return getInstance()->m_SoundEngine.GetMasterVolume(); }

	/**
	 * SetMusicVolume
	 * set the volume of music
	 * @param iVolume range of
	 * 0 to -10000, where 0 is max volume
	 * and -10000 is mute
	 */
    inline static void setMusicVolume(const int volume) { getInstance()->m_MusicPlayer.SetVolume(volume); }
    inline static  const int getMusicVolume() { return getInstance()->m_MusicPlayer.GetVolume(); }

    /**
     * Enable/disable sounds
     */
    inline static void setSoundsEnabled(const bool enabled) { 
        getInstance()->m_SoundsEnabled = enabled; 
        if (!enabled) stopSounds();
    }

    inline static const bool getSoundsEnabled() { return getInstance()->m_SoundsEnabled; }

    /**
     * Enable/disable music
     */
    inline static void setMusicEnabled(const bool enabled) { 
        getInstance()->m_MusicEnabled = enabled; 
        if (enabled) playMusic(getInstance()->m_CurrentMusic, getInstance()->m_LoopMusic);
        else stopMusic();
    }

    inline static const bool getMusicEnabled() { return getInstance()->m_MusicEnabled; }


private:
    // private constructor
    SoundManager(void);

    CSoundEngine m_SoundEngine;

    // containers for sound & music
    map<SoundTypes, CSoundWave*> m_SoundsMap;
    map<MusicTypes, TCHAR*> m_MusicMap;

    // music player
    CSoundMP3Player m_MusicPlayer;

    // whether sounds are on or off
    bool m_SoundsEnabled;
    bool m_MusicEnabled;

    bool m_LoopMusic;

    MusicTypes m_CurrentMusic; // currently playing music

};
