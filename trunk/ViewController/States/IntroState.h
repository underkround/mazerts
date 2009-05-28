/**
 * Intro-state of the application
 *
 * $Revision: 395 $
 * $Date: 2009-05-27 02:16:14 +0300 (ke, 27 touko 2009) $
 * $Id: IntroState.h 395 2009-05-26 23:16:14Z murgo $
 */

#ifndef __INTROSTATE_H__
#define __INTROSTATE_H__

#include "IState.h"
#include "../App/ID3DApplication.h"
#include "../Asset/UI3DObjectManager.h"
#include "../Asset/Selector.h"
#include "../Terrain/UITerrain.h"
#include "../Camera/Camera.h"
#include "../Camera/UnitCamera.h"

#include "../Controller/IUIController.h"

#include "../Controller/Cursor.h"

//#include "../UIComponent/DummyComponent.h"
//#include "../UIComponent/UIContainer.h"
#include "../UIComponent/RootContainer.h"

#include "../UIComponent/IButtonListener.h"
#include "../UIComponent/BasicButton.h"
#include "../UIComponent/MenuGui.h"

class CTheApp;

class IntroState : public IState, IButtonListener
{
public:
    IntroState();
    virtual ~IntroState();

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
        
        hres = m_pRootContainer->onRestore(m_pDevice);
        if(FAILED(hres))
        {
            return hres;
        }

        return S_OK;
    }

    /**
     * Regenerates UI terrain
     */
    void redrawTerrain();

    /**
     * Returns player who's in charge
     */
    inline Player* getCurrentPlayer() { return m_pCurrentPlayer; }

    /**
     * Sets current player
     */
    inline void setCurrentPlayer(Player* pPlayer) { m_pCurrentPlayer = pPlayer; }

// ===== IButtonListener methods

    virtual void onButtonClick(BasicButton* pSrc);

    virtual void onButtonAltClick(BasicButton* pSrc);

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

    // Root container covering the whole screen and managing components
    RootContainer*      m_pRootContainer;

    // Player who is currently playing
    Player*             m_pCurrentPlayer;

    // switch this to 0 to escape from menu
    bool                m_NotFinished;

    MenuGui*            m_pMenuGui;
    BasicButton*        m_pLogo;

    UnitCamera*         m_UnitCamera;

    float               m_Timer;

};

#endif //__GAMESTATE_H__
