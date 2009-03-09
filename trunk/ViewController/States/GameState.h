/**
 * Game-state of the application
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include "IState.h"
#include "../App/ID3DApplication.h"
#include "../Unit/UI3DObjectManager.h"
#include "../Unit/Selector.h"
#include "../Terrain/UITerrain.h"

class GameState : public IState
{
public:
    GameState();
	virtual ~GameState();

    /**
     * Called when the state is created
     * @param pApplication Pointer to I3DApplication
     * @return S_OK if state was initialized successfully, otherwise error code
     */
    virtual HRESULT create(ID3DApplication* pApplication);

    /**
     * Called before the state is going to be destroyed, releases resources
     */
    virtual void release();

    /** 
     * Updates the game controls and game logic
     * @param fFrameTime Time passed since last update in seconds
     * @return True, if the game keeps running, false if it is terminated
     */
    virtual bool update(float frameTime);

    /**
     * Sets lights and camera
     * @param pDevice Pointer to D3d-device
     */
    virtual void prepareForRender(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Renders the game
     * @param pDevice Pointer to D3d-device
     */
    virtual void render(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Releases D3DPOOL_DEFAULT-stuff on device lost
     */
    virtual void onDeviceLost()
    {
        m_pUITerrain->onLostDevice();
    }

    /**
     * Recreates D3DPOOL_DEFAULT-stuff after device reset
     * @return HRESULT
     */
    virtual HRESULT onRestore()
    {
        return m_pUITerrain->onRestore(m_pDevice);
    }


private:

    /** 
     * Updates keyboard and mouse controls
     * @param fFrameTime Time passed since last update in seconds
     */
    void updateControls(float frameTime);

    /**
     * Main application
     */
    ID3DApplication* m_pApp;

    /** 
     * Pointer to Direct3d-device
     */
    LPDIRECT3DDEVICE9 m_pDevice;

    /**
     * Object manager, rendering and update of objects is done through this
     */
    UI3DObjectManager* m_pManager;

    /**
     * Game-terrain
     */
    UITerrain* m_pUITerrain;

    /**
     * Selector mesh
     */
    Selector m_Selector;

    /**
     * Camera location
     */
    float m_MainCameraX;
    float m_MainCameraY;
    float m_MainCameraZ;
};

#endif //__GAMESTATE_H__