/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#include "SoundController.h"

#include "../../Model/Common/Config.h"
#include "../Sound/SoundManager.h"
#include "../Input/KeyboardState.h"

SoundController::SoundController()
{
    // Play music
    SoundManager::playMusic(SoundManager::BACKGROUND, true);

    // Default controls
    int m_KeySoundToggle    = 63;
    int m_KeyMusicToggle    = 64;
    int m_KeyVolumeUp       = 78;
    int m_KeyVolumeDown     = 74;
}

SoundController::~SoundController()
{
}

void SoundController::loadConfiguration(const bool confFileLoaded)
{
    Config & c = * Config::getInstance();
    if(!confFileLoaded)
    {
        c.setFilename("controls.ini");
        c.readFile();
    }
    c.updateInt("key toggle sound",         m_KeySoundToggle);
    c.updateInt("key toggle music",         m_KeyMusicToggle);
    c.updateInt("key master volume up",     m_KeyVolumeUp);
    c.updateInt("key master volume down",   m_KeyVolumeDown);
}

void SoundController::updateControls(const float frameTime)
{
    // sound things
    if (KeyboardState::keyReleased[m_KeySoundToggle])
        SoundManager::playSound(SoundManager::DEBUG, 0.1f, D3DXVECTOR3(50, 100, 0));
//        SoundManager::setSoundsEnabled(!SoundManager::getSoundsEnabled());
    if (KeyboardState::keyReleased[m_KeyMusicToggle])
        SoundManager::setMusicEnabled(!SoundManager::getMusicEnabled());
    if (KeyboardState::keyDown[m_KeyVolumeUp])
        SoundManager::setMasterVolume(SoundManager::getMasterVolume() + 50);
    if (KeyboardState::keyDown[m_KeyVolumeDown])
        SoundManager::setMasterVolume(SoundManager::getMasterVolume() - 50);
}
