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
#include "../Asset/UI3DObjectManager.h"
#include "../Asset/Selector.h"
#include "../Terrain/UITerrain.h"
#include "../Camera/Camera.h"

#include "../Controller/IUIController.h"

#include "../Controller/Cursor.h"

//#include "../UIComponent/DummyComponent.h"
//#include "../UIComponent/UIContainer.h"
#include "../UIComponent/RootContainer.h"

class GameState : public IState
{
public:
    GameState();
    virtual ~GameState();

    enum GAMECONDITION {
        CONDITION_NONE,
        CONDITION_WIN,
        CONDITION_LOSE
    };

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

    inline const GAMESTATE getStateType() { return GAMESTATE_GAME; }

    inline const GAMECONDITION getGameCondition() { return m_Condition; }
    inline void setGameCondition(const GAMECONDITION cond) { m_Condition = cond; }

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
     * Checks win/lose condition
     * @return false if game is over
     */
    bool checkGameConditions(const float fFrameTime);

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

    // current game condition
    GAMECONDITION m_Condition;

    // Root container covering the whole screen and managing components
    RootContainer*      m_pRootContainer;

    UIContainer*        m_pCont1;
    UIContainer*        m_pCont2;

    // Player who is currently playing
    Player*             m_pCurrentPlayer;
};

#endif //__GAMESTATE_H__