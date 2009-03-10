#include "SoundManager.h"

SoundManager::SoundManager(void)
{
    // initialize member variables
    m_SoundsEnabled = true;
    m_MusicEnabled = true;
    m_LoopMusic = true;
}

SoundManager::~SoundManager(void)
{
}

HRESULT SoundManager::create(IApplication* pApp)
{
    SoundManager* pInstance = getInstance();

    // HERE BE ALL SOUNDS FOR NOW
    static const Sound soundsList[] = 
    {
        {_T("explosion.wav"), EXPLOSION},
        {_T("ready.wav"), READY},
    };

    pInstance->m_MusicMap[BACKGROUND] = _T("music.mp3");

    HRESULT hres;

    // init sound engine
    hres = pInstance->m_SoundEngine.Create(pApp->GetWindow(), DEFAULT_FREQUENCY, DEFAULT_BPS, DEFAULT_CHANNELS, DEFAULT_FLAGS);
    if (FAILED(hres))
        return hres;

    // load sounds
    int amount = sizeof(soundsList) / sizeof(soundsList[0]);
    for (int i = 0; i < amount; i++)
    {
        CSoundWave* pWave = new CSoundWave;
        hres = pWave->Load(	pInstance->m_SoundEngine,
                            soundsList[i].text,
					        DEFAULT_DUPLICATE_AMOUNT,
					        DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFX);
        if (FAILED(hres))
            return hres;

        pInstance->m_SoundsMap[soundsList[i].type] = pWave;
    }

    return S_OK;
}

void SoundManager::release()
{
    SoundManager* pInstance = getInstance();
    
    // release sounds
    map<SoundTypes, CSoundWave*>::iterator it;
    for (it = pInstance->m_SoundsMap.begin(); it != pInstance->m_SoundsMap.end(); it++)
    {
        CSoundWave* pWave = (*it).second;
        pWave->Release();
        delete pWave;
    }
    pInstance->m_SoundsMap.clear();

    // release musics
    /**
     * Not needed then, I guess
    map<MusicTypes, TCHAR*>::iterator it2;
    for (it2 = pInstance->m_MusicMap.begin(); it2 != pInstance->m_MusicMap.end(); it2++)
    {
        TCHAR* pText = (*it2).second;
        OutputDebugString(pInstance->m_MusicMap[BACKGROUND]);
        delete pText;
    }
    */
    pInstance->m_MusicMap.clear();

    // release sound engine
    pInstance->m_SoundEngine.Release();

    // release music player
    stopMusic();
}

void SoundManager::update()
{
    SoundManager* pInstance = getInstance();

    if (pInstance->m_MusicEnabled)
        {
	    // loop music
	    if (pInstance->m_MusicPlayer.IsAtEnd() && pInstance->m_LoopMusic)
	    {
//		    pInstance->m_MusicPlayer.Stop();
//		    pInstance->m_MusicPlayer.Play();
            pInstance->m_MusicPlayer.SetPosition(0);
	    }
        else if (pInstance->m_MusicPlayer.IsAtEnd())
        {
            // music has ended but looping is off
            pInstance->m_MusicPlayer.Release();
        }
    }
}

void SoundManager::playSound(const SoundTypes type, const float distort)
{
    SoundManager* pInstance = getInstance();
    if (!pInstance->m_SoundsEnabled) return;

    CSoundWave* pWave = pInstance->m_SoundsMap[type];
	DWORD duplicate = pWave->GetNextFreeDuplicate();
    const int dist = (int)(pWave->getOriginalFrequency() * distort);
    const int minDistort = pWave->getOriginalFrequency() - dist;
    const int maxDistort = pWave->getOriginalFrequency() + dist;

    pWave->SetFrequency(IApplication::RandInt(minDistort, maxDistort), duplicate);
	//pWave->SetVolume(0, dwDuplicate);
	//pWave->SetPan(0, dwDuplicate);
	//pWave.RePlay(FALSE, dwDuplicate);    
    pWave->Play(false, duplicate);
}

/*
void playSound3D(const SoundTypes type, const D3DXVECTOR3 position, const float distort)
{
    playSound(type, distort);
}
*/

void SoundManager::stopSounds()
{
    SoundManager* pInstance = getInstance();
    
    map<SoundTypes, CSoundWave*>::iterator it;
    for (it = pInstance->m_SoundsMap.begin(); it != pInstance->m_SoundsMap.end(); it++)
    {
        CSoundWave* pWave = (*it).second;
        for (DWORD i = 0; i < pWave->GetBufferCount(); i++)
        {
            pWave->Stop(i);
            pWave->SetPosition(0, i);
        }
    }
}

void SoundManager::playMusic(const MusicTypes type, bool looping)
{
    SoundManager* pInstance = getInstance();
    if (!pInstance->m_MusicEnabled) return;

    stopMusic();
    pInstance->m_MusicPlayer.Create(pInstance->m_MusicMap[type]);
    pInstance->m_MusicPlayer.Play();

    pInstance->m_LoopMusic = looping;
    pInstance->m_CurrentMusic = type;
}

void SoundManager::stopMusic()
{
    SoundManager* pInstance = getInstance();

    pInstance->m_MusicPlayer.Stop();
    pInstance->m_MusicPlayer.Release();
}
