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
//#include "../App/ID3DApplication.h"
#include "../Asset/UI3DObjectManager.h"
#include "../Asset/Selector.h"
#include "../Terrain/UITerrain.h"
#include "../Camera/Camera.h"

#include "../Controller/IUIController.h"

#include "../Controller/Cursor.h"

//#include "../UIComponent/DummyComponent.h"
//#include "../UIComponent/UIContainer.h"
#include "../UIComponent/RootContainer.h"

class CTheApp;

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
    virtual HRESULT create(CTheApp* pApplication);

    /**
     * Called before the state is going to be destroyed, releases resources
     */
    virtual void release();

    /** 
     * Updates the game controls and game logic
     * @param fFrameTime Time passed since last update in seconds
     * @return True, if the game keeps running, false if it is terminated
     */
    virtual bool update(const float frameTime);

    /**
     * Sets lights and camera
     * @param pDevice Pointer to D3d-device
     */
    virtual void prepareForRender(const LPDIRECT3DDEVICE9 pDevice, const float frameTime);

    /**
     * Renders the game
     * @param pDevice Pointer to D3d-device
     */
    virtual void render(const LPDIRECT3DDEVICE9 pDevice);

    /**
     * Releases D3DPOOL_DEFAULT-stuff on device lost
     */
    virtual void onDeviceLost()
    {
        if(m_pUITerrain)
            m_pUITerrain->onLostDevice();
        m_Selector.onDeviceLost();
        m_pRootContainer->onDeviceLost();
    }

    /**
     * Recreates D3DPOOL_DEFAULT-stuff after device reset
     * @return HRESULT
     */
    virtual HRESULT onRestore()
    {
        Camera::getCurrent()->forceUpdate();
        HRESULT hres = m_pUITerrain->onRestore(m_pDevice);
        if(FAILED(hres))
        {
            return hres;
        }
        
        hres = m_Selector.onRestore(m_pDevice);
        if(FAILED(hres))
        {
            return hres;
        }

        hres = m_pRootContainer->onRestore(m_pDevice);
        if(FAILED(hres))
        {
            return hres;
        }

        return S_OK;
    }

    /**
     * Returns player who's in charge
     */
    inline Player* getCurrentPlayer() { return m_pCurrentPlayer; }

    /**
     * Sets current player
     */
    inline void setCurrentPlayer(Player* pPlayer) { m_pCurrentPlayer = pPlayer; }

    /**
     * Regenerates UI terrain
     */
    void redrawTerrain();

    void win();
    void lose();

private:

    /** 
     * Updates keyboard and mouse controls
     * @param fFrameTime Time passed since last update in seconds
     */
    void updateControls(const float frameTime);

    /**
     * initializes controls
     */
    void loadConfigurations(void);

    /**
     * enable/disable win condition checking
     */
    void toggleGameConditionChecking(bool setting) { m_bWinConditionsEnabled = setting; }

    /**
     * Checks win/lose condition
     * @return false if game is over
     */
    bool checkGameConditions(const float fFrameTime);

    /**
     * Main application
     */
    CTheApp* m_pApp;

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
     * Controllers that receive updateControls -calls
     */
    DoubleLinkedList<IUIController*> m_UIControllers;

    /**
     * Key configuration
     */
    int m_KeyGenerateNewTerrain;
    int m_KeyGeneratePassability;
    int m_KeyToggleWireframe;
    int m_KeyTerrainDetailUp;
    int m_KeyTerrainDetailDown;
    int m_KeyTerrainRedraw;

    // aa
    bool m_Antialias;

    // condition update timers
    float m_ConditionUpdateTime;
    float m_ConditionUpdateInterval;

    //check for win conditions
    bool m_bWinConditionsEnabled;

    // Root container covering the whole screen and managing components
    RootContainer*      m_pRootContainer;

    UIContainer*        m_pCont1;
    UIContainer*        m_pCont2;

    // Player who is currently playing
    Player*             m_pCurrentPlayer;
};

#endif //__GAMESTATE_H__