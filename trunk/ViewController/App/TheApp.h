/**
 * TheApp.h header file
 * Interface for the CTimer class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 29.1.2009
 * 
 * Concrete game application class
 */

#pragma once

#include "ID3DApplication.h"
#include "vertices.h"
#include "../Terrain/UITerrain.h"

#include "../Unit/UI3DObjectManager.h"
#include "../Unit/UIUnit.h"
#include "../../Model/Asset/Unit.h"

#include "InputEngine.h"
#include "InputKeyboard.h"
#include "InputMouse.h"

class CTheApp : public ID3DApplication
{
public:
    CTheApp(void);
    virtual ~CTheApp(void);

    /**
     * OnCreate
     * pure virtual function for the application to initialise
     * all application specific resources, graphics, sounds, whatever.
     * Called after application initialisation is complete.
     * @return S_OK if successful, or any error code to cancel app startup
     */
    virtual HRESULT OnCreate(void);

    /**
     * OnRelease
     * pure virtual function for the application to release
     * all application specific resources, graphics, sounds, whatever.
     * Called just before the application is shut down.
     */
    virtual void OnRelease(void);

    /**
     * OnFlip
     * pure virtual application main loop function
     * this function is called by the engine just before
     * graphics are transferred to the screen
     */
    virtual void OnFlip(void);

private:
    //Input engine members
	int							    m_TextRow;
	int								m_iMouseX;
	int								m_iMouseY;
	int								m_iMouseZ;
    CInputEngine                    m_InputEngine;
	vector<CInputEngine::DEVICE>	m_arrInputDevices;
    CInputKeyboard			        m_Keyboard;
	CInputMouse			            m_Mouse;

    /**
     * DrawTextRow
     * writes text to screen and advances cursor one row
     * @param text to be written on screen
     * @param color text color
     */
    void DrawTextRow(LPCTSTR text, DWORD color);

    /**
     * OnKeyDown
     * virtual keypress message handler
     */
    virtual void OnKeyDown(DWORD dwKey);

    /**
	 * UpdateKeyboard
	 * update keyboard status and print it to screen
	 */
	void UpdateKeyboard(void);
 
	/**
	 * UpdateKeyboard
	 * update mouse status and print it to screen
	 */
	void UpdateMouse(void);

    /**
     * CheckTexturingCaps
     * check what kind of texturing capabilities
     * the display accelerator has
     */
    void CheckTexturingCaps(void);

    /**
     * UI-terrain pointer, POC
     */
    UITerrain* pUITerrain;
    
    float m_fX;
    float m_fY;
    float m_fZ;

    bool m_Help;

    UI3DObjectManager* m_pManager;

};