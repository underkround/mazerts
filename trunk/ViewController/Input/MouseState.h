/**
 * Static mouse helpers, keeps track of mouse state
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __MOUSESTATE_H__ 
#define __MOUSESTATE_H__

#include "../App/IApplication.h"
#include "InputEngine.h"
#include "InputMouse.h"

#include "d3dx9.h"

class MouseState
{
public:

    /**
     * Positions mouse correctly and creates arrays
     * @param pApplication Pointer to IApplication instance
     */
    static void create(IApplication* pApplication);

    /**
     * Cleans up
     */
    inline static void release()
    {
        if(mouseButton)
        {
            delete [] mouseButton;       
        }

        if(mouseButtonOld)
        {
            delete [] mouseButtonOld;
        }

        if(mouseButtonReleased)
        {
            delete [] mouseButtonReleased;
        }
    }

    /**
     * Updates mouse data and prevents mouse cursor from leaving screen/window
     */
    static void update();

    /**
     * Sets the mouse sensitivity factor, x- and y-speed are multiplied by this
     * @param sens Mouse sensitivity multiplier
     */
    inline static void setSensitivity(float sens)
    {
        mouseSens = sens;
    }

    /**
     * Gets the sensitivity
     * @return Mouse sensitivity multiplier
     */
    inline static float getSensitivity() { return mouseSens; }

    /**
     * Mouse data, left public on purpose
     */
    static int mouseX;
    static int mouseY;
    static int mouseZ;
    static int mouseXSpeed;
    static int mouseYSpeed;
    static int mouseZSpeed;
    static bool* mouseButton;           //Current mousebutton state, true = pressed
    static bool* mouseButtonReleased;   //Mousebutton state change between last two update-calls, 
                                        //true if the button was released between them

    /** 
     * Project 2d-mouse position as a ray into 3d-space
     * @param matView View-matrix
     * @param matProj Projection-matrix
     * @param rayOrigin D3DXVECTOR3 to store the ray origin into
     * @param rayDir D3DXVECTOR3 to store the ray direction into
     */
    static void transformTo3D(D3DXMATRIX& matView, D3DXMATRIX& matProj, D3DXVECTOR3& rayOrigin, D3DXVECTOR3& rayDir);

private:
    
    MouseState() {}

    /**
     * Mouse sensitivity factor
     */
    static float mouseSens; 

    /**
     * Old states of mouse buttons, used to recognize mouse button up/down-events
     */
    static bool* mouseButtonOld;
};


#endif //__MOUSESTATE_H__