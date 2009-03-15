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

SoundController::SoundController(Camera* camera)
{
    m_pCamera = camera;

    // Play music
    SoundManager::playMusic(SoundManager::BACKGROUND, true);
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
    m_KeySoundToggle = c.getValueAsInt("toggle sound");
    m_KeyMusicToggle = c.getValueAsInt("toggle music");
    m_KeyVolumeUp = c.getValueAsInt("master volume up");
    m_KeyVolumeDown = c.getValueAsInt("master volume down");
}

void SoundController::updateControls(const float frameTime)
{
    // sound things
    if (KeyboardState::keyReleased[m_KeySoundToggle])
        SoundManager::playSound(SoundManager::EXPLOSION, 0.1f, D3DXVECTOR3(50, 100, 0), m_pCamera);
//        SoundManager::setSoundsEnabled(!SoundManager::getSoundsEnabled());
    if (KeyboardState::keyReleased[m_KeyMusicToggle])
        SoundManager::setMusicEnabled(!SoundManager::getMusicEnabled());
    if (KeyboardState::keyDown[m_KeyVolumeUp])
        SoundManager::setMasterVolume(SoundManager::getMasterVolume() + 50);
    if (KeyboardState::keyDown[m_KeyVolumeDown])
        SoundManager::setMasterVolume(SoundManager::getMasterVolume() - 50);
}
