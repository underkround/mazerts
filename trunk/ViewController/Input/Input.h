/**
 * Input-side "main class", handles create and release
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __INPUT_H__
#define __INPUT_H__

#include "InputEngine.h"
#include "InputKeyboard.h"
#include "InputMouse.h"
#include "MouseState.h"
#include "KeyboardState.h"

#include <vector>

#include "../App/IApplication.h"

class Input
{
public:

    friend class MouseState;
    friend class KeyboardState;

    /**
     * Initialises the InputEngine and devices
     * @param pApplication Pointer to IApplication-instance
     * @return S_OK if everything went fine, otherwise error code
     */
    static HRESULT create(IApplication* pApplication);

    /**
     * Updates mouse and keyboard state data
     */
    static void update();

    /**
     * Releases input devices, associated data and InputEngine
     */
    static void release();

private:
    
    /**
     * Application-instance
     */
    static IApplication* pApp;

    /**
     * Main input-engine
     */
    static CInputEngine inputEngine;

    /**
     * Device array
     */
    static std::vector<CInputEngine::DEVICE> arrDevices;

    /**
     * CInputKeyboard to use
     */
    static CInputKeyboard keyboard;

    /**
     * CInputMouse to use
     */
    static CInputMouse mouse;

};

#endif //__INPUT_H__