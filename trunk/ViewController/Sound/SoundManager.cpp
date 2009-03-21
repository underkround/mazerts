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
    // MOVE ELSEWHERE PLZ
    static const Sound soundsList[] = 
    {
        {_T("..//data//sounds//explosion.wav"), SOUND_DEBUG},
        {_T("..//data//sounds//ready.wav"), SOUND_READY},
        {_T("..//data//sounds//shoot1.wav"), SOUND_SHOOT},
        {_T("..//data//sounds//shoot2.wav"), SOUND_SHOOT},
        {_T("..//data//sounds//shoot3.wav"), SOUND_SHOOT},
        {_T("..//data//sounds//shoot4.wav"), SOUND_SHOOT},
        {_T("..//data//sounds//ok1.wav"), SOUND_OK},
        {_T("..//data//sounds//ok2.wav"), SOUND_OK},
        {_T("..//data//sounds//no1.wav"), SOUND_NO}
    };

    pInstance->m_MusicMap[MUSIC_BACKGROUND] = _T("..//data//music//music.mp3");

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
        DoubleLinkedList<CSoundWave*>* pDll = NULL;
        pDll = pInstance->m_SoundsMap[soundsList[i].type];
        if (!pDll)
        {
            pDll = new DoubleLinkedList<CSoundWave*>;
            pInstance->m_SoundsMap[soundsList[i].type] = pDll;
        }

        hres = pWave->Load(    pInstance->m_SoundEngine,
                            soundsList[i].text,
                            DEFAULT_DUPLICATE_AMOUNT,
                            DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFX);
        if (FAILED(hres))
            return hres;

        pDll->pushHead(pWave);
    }

    return S_OK;
}

void SoundManager::release()
{
    SoundManager* pInstance = getInstance();
    
    // release sounds
    map<SoundTypes, DoubleLinkedList<CSoundWave*>*>::iterator it;
    for (it = pInstance->m_SoundsMap.begin(); it != pInstance->m_SoundsMap.end(); it++)
    {
        DoubleLinkedList<CSoundWave*>* pDll = (*it).second;
        ListNode<CSoundWave*>* pLn = pDll->headNode();
        while (pLn)
        {
            CSoundWave* pWave = pLn->item;
            pWave->Release();
            delete pWave;
            pLn = pDll->removeGetNext(pLn);
        }
        delete pDll;
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

    // release currently playing list
    DoubleLinkedList<SoundNode*>* list = &pInstance->m_CurrentSounds;
    ListNode<SoundNode*>* ln = list->headNode();
    while (ln)
    {
        SoundNode* sn = ln->item;
        delete sn;
        ln = list->removeGetNext(ln);
    }
}

void SoundManager::update(const float deltaT)
{
    SoundManager* pInstance = getInstance();

    pInstance->m_LastUpdated += deltaT;
    if (pInstance->m_LastUpdated < UPDATE_INTERVAL) return;
    pInstance->m_LastUpdated = 0.0f;

    if (pInstance->m_MusicEnabled)
        {
        // loop music
        if (pInstance->m_MusicPlayer.IsAtEnd() && pInstance->m_LoopMusic)
        {
            pInstance->m_MusicPlayer.Stop();
            pInstance->m_MusicPlayer.SetPosition(0);
            pInstance->m_MusicPlayer.Play();
        }
        else if (pInstance->m_MusicPlayer.IsAtEnd())
        {
            // music has ended but looping is off
            pInstance->m_MusicPlayer.Release();
        }
    }

    // TODO: This should go into its own thread
    DoubleLinkedList<SoundNode*>* list = &pInstance->m_CurrentSounds;
    if (pInstance->m_SoundsEnabled)
    {
        if (!list->empty()) {
            ListNode<SoundNode*>* ln = list->headNode();
            while (ln)
            {
                SoundNode* sn = ln->item;
                if (sn->pWave->IsPlaying(sn->duplicate))
                {
                    fixPanVol(sn);
                    /*
                     * remove these comment marks to make sounds end if source gets
                     * too far away
                    if (!fixPanVol(sn))
                    {
                        // sound is too far away
                        sn->pWave->Stop(sn->duplicate);
                        sn->pWave->SetPosition(0, sn->duplicate);
                        ln = list->removeGetNext(ln);
                        delete sn;
                    }
                    else
                        */
                        ln = ln->next;
                }
                else
                {
                    // sound is at end
                    ln = list->removeGetNext(ln);
                    delete sn;
                }
            }
        }
    }
}

void SoundManager::playSound(const SoundTypes type, const float distort)
{
    playSound(type, distort, 0, 0);
}

void SoundManager::playSound(const SoundTypes type, const float distort, const int volume, const int pan)
{
    SoundManager* pInstance = getInstance();
    if (!pInstance->m_SoundsEnabled) return;

    // get random sound from list
    DoubleLinkedList<CSoundWave*>* pDll = pInstance->m_SoundsMap[type];
    ListNode<CSoundWave*>* pLn = pDll->headNode();
    int i = IApplication::RandInt(0, pDll->count() - 1);
    for (int j = 0; j < i; j++)
        pLn = pLn->next;

    CSoundWave* pWave = pLn->item;
    DWORD duplicate = pWave->GetNextFreeDuplicate();
    const int dist = (int)(pWave->getOriginalFrequency() * distort);
    const int minDistort = pWave->getOriginalFrequency() - dist;
    const int maxDistort = pWave->getOriginalFrequency() + dist;

    pWave->SetFrequency(IApplication::RandInt(minDistort, maxDistort), duplicate);
    pWave->SetVolume(volume, duplicate);
    pWave->SetPan(pan, duplicate);
    //pWave.RePlay(FALSE, dwDuplicate);    
    pWave->Play(false, duplicate);
}

void SoundManager::playSound(const SoundTypes type, const float distort, const D3DXVECTOR3 soundPos)
{
    // use current camera, or no camera at all if it's not set
    if(Camera::getCurrent())
        playSound(type, distort, soundPos, Camera::getCurrent());
    else
        playSound(type, distort);
}

void SoundManager::playSound(const SoundTypes type, const float distort, const D3DXVECTOR3 soundPos, Camera* pCamera)
{
    SoundManager* pInstance = getInstance();
    if (!pInstance->m_SoundsEnabled) return;

    // get random sound from list
    DoubleLinkedList<CSoundWave*>* pDll = pInstance->m_SoundsMap[type];
    ListNode<CSoundWave*>* pLn = pDll->headNode();
    int i = IApplication::RandInt(0, pDll->count() - 1);
    for (int j = 0; j < i; j++)
        pLn = pLn->next;

    CSoundWave* pWave = pLn->item;
	DWORD duplicate = pWave->GetNextFreeDuplicate();
    const int dist = (int)(pWave->getOriginalFrequency() * distort);
    const int minDistort = pWave->getOriginalFrequency() - dist;
    const int maxDistort = pWave->getOriginalFrequency() + dist;

    pWave->SetFrequency(IApplication::RandInt(minDistort, maxDistort), duplicate);

    SoundNode* sn = new SoundNode();
    sn->pWave = pWave;
    sn->pCamera = pCamera;
    sn->position = soundPos;
    sn->duplicate = duplicate;
    if (fixPanVol(sn))
    {
        pWave->Play(false, duplicate);
        pInstance->m_CurrentSounds.pushTail(sn);
    } else
        delete sn;
}

bool SoundManager::fixPanVol(SoundNode* sn)
{
    // calculate distance
    D3DXVECTOR3 distvect;
    D3DXVec3Subtract(&distvect, &sn->position, &sn->pCamera->getPosition());
    float dist = D3DXVec3Length(&distvect);
    if (dist > HEARING_DISTANCE) {
        // sound is too far away
        return false;
    }

    // calculate angle
    D3DXVECTOR2 normpos(distvect.x, distvect.y);
    D3DXVec2Normalize(&normpos, &normpos);
    D3DXVECTOR2 right(sn->pCamera->getMatrix()._11, -sn->pCamera->getMatrix()._12); // second parameter is inversed because of hitler, seems like _21 would work too
    float dot = D3DXVec2Dot(&normpos, &right);

    int pan = (int)(2000 * dot);
    int volume = 0;
    int third = (int)(0.33f * HEARING_DISTANCE);
    if (dist > third)
        volume = -(int)((dist - third) / (HEARING_DISTANCE >> 1) * 3000);

    CSoundWave* pWave = sn->pWave;
    pWave->SetPan(pan, sn->duplicate);
    pWave->SetVolume(volume, sn->duplicate);
    return true;
}

void SoundManager::stopSounds()
{
    SoundManager* pInstance = getInstance();
    
    map<SoundTypes, DoubleLinkedList<CSoundWave*>*>::iterator it;
    for (it = pInstance->m_SoundsMap.begin(); it != pInstance->m_SoundsMap.end(); it++)
    {
        DoubleLinkedList<CSoundWave*>* pDll = (*it).second;
        ListNode<CSoundWave*>* pLn = pDll->headNode();
        while (pLn)
        {
            CSoundWave* pWave = pLn->item;
            for (DWORD i = 0; i < pWave->GetBufferCount(); i++)
            {
                pWave->Stop(i);
                pWave->SetPosition(0, i);
            }
            pLn = pLn->next;
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
