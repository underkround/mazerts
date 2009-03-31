/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __SOUNDCONTROLLER_H__
#define __SOUNDCONTROLLER_H__

#include "IUIController.h"
#include "../Camera/Camera.h"
#include "../../Model/Common/Config.h"

class SoundController : public IUIController
{
public:

    SoundController();

    ~SoundController();

    /**
     * @see IUIController
     */
    virtual void updateControls(const float frameTime);

    /**
     * @see IUIController
     */
    virtual void loadConfigurations();

private:

    int m_KeySoundToggle;
    int m_KeyMusicToggle;
    int m_KeyVolumeUp;
    int m_KeyVolumeDown;

};

#endif // __SOUNDCONTROLLER_H__
