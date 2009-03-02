/**
 * TheApp.h header file
 * Interface for the CTimer class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 29.1.2009
 * 
 * Concrete game application class
 */

#ifndef __THEAPP_H__
#define __THEAPP_H__

#include "ID3DApplication.h"
#include "vertices.h"
#include "../Terrain/UITerrain.h"

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

    /**
     * OnKeyDown
     * virtual keypress message handler
     */
    virtual void OnKeyDown(DWORD dwKey);

    /**
     * Read keys and do stuff
     */
    void ReadKeys(void);

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

    LPD3DXMESH m_TestCube;
    LPD3DXMESH m_TargetSphere;    

    D3DMATERIAL9* m_pMat;
};


#endif    // __THEAPP_H__
