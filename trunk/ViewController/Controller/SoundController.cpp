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
    SoundManager::playMusic(MUSIC_BACKGROUND, true);

    // Default controls
    int m_KeySoundToggle    = 63;
    int m_KeyMusicToggle    = 64;
    int m_KeyVolumeUp       = 78;
    int m_KeyVolumeDown     = 74;
}

SoundController::~SoundController()
{
}

void SoundController::loadConfigurations()
{
    Config & c = * Config::getInstance();
    // we trust that config files have been loaded when this is called
    c.updateInt("key toggle sound",         m_KeySoundToggle);
    c.updateInt("key toggle music",         m_KeyMusicToggle);
    c.updateInt("key master volume up",     m_KeyVolumeUp);
    c.updateInt("key master volume down",   m_KeyVolumeDown);
}

void SoundController::updateControls(const float frameTime)
{
    // sound things
    if (KeyboardState::keyReleased[m_KeySoundToggle])
        SoundManager::setSoundsEnabled(!SoundManager::getSoundsEnabled());
    if (KeyboardState::keyReleased[m_KeyMusicToggle])
        SoundManager::setMusicEnabled(!SoundManager::getMusicEnabled());
    if (KeyboardState::keyDown[m_KeyVolumeUp])
        SoundManager::setMasterVolume(SoundManager::getMasterVolume() + 50);
    if (KeyboardState::keyDown[m_KeyVolumeDown])
        SoundManager::setMasterVolume(SoundManager::getMasterVolume() - 50);
}
