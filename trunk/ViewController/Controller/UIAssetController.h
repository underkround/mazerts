/**
 * UIAssetController is responsible of handling and controlling of the
 * current player's units and buildings in the game.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __UIASSETCONTROLLER_H__
#define __UIASSETCONTROLLER_H__

#include "IUIController.h"
#include "../Asset/Selector.h"
#include "../../Model/Command/UnitCommandDispatcher.h"
#include "../../Model/Asset/IAssetCollectionListener.h" // for listenin of destroyed assets that might be selected
#include "../../Model/Common/Config.h"
#include "../Camera/Camera.h"
#include "../Camera/UnitCamera.h"
#include "../UIComponent/UIContainer.h"
#include "../UIComponent/BasicButton.h"
#include "../UIComponent/IButtonListener.h"
//#include "BuildButtonWrapper.h"
#include "BuildQueue.h"

#include <map>

#include "d3d9.h"

class UIAssetController : public IUIController, public IAssetCollectionListener, public IButtonListener
{
public:

    enum State
    {
        STATE_ASSET_CONTROL = 0,
        STATE_BUILDING_PLACEMENT,
        STATE_NUKE
    };

    enum PointerState
    {
        IDLE = 0,
        CLICK,
        DRAG
    };

    // what kind of assets are selected
    enum SelectedType
    {
        NONE = 0,
        UNIT,
        BUILDING,
        ENEMY // enemy / enemies are selected, so you cannot move them
    };

    UIAssetController(const LPDIRECT3DDEVICE9 pDevice, Selector* pSelector, Player* pCurrentPlayer);
    ~UIAssetController();

// ===== IButtonListener methods

    virtual void onButtonClick(BasicButton* pSrc);

    virtual void onButtonAltClick(BasicButton* pSrc);

// =====

    void createBuildingButton(AssetDef* pAssetDef);

    /**
     * Release all resources
     */
    virtual void release();

    /**
     * Read input devices and do the things
     */
    virtual void updateControls(const float frameTime);

    /**
     * @see IUIController
     */
    virtual void loadConfigurations();

    /**
     * Clear all units under selection
     */
    void clearSelection();

// ===== AssetCollectionListener methods

    virtual void handleCreatedAsset(IAsset* instance);

    /**
     * Check if we have released asset selected
     */
    virtual void handleReleasedAsset(IAsset* instance);

    void toggleFirstPersonCamera();
    void UIAssetController::toggleFirstPersonCamera(UIUnit* pUnit);

    inline void setNukeSilo(Building* pSilo) { m_pCurrentNukeSilo = pSilo; }

private:

    void changeState(State newState);

    void setupBuildQueue(IAsset* selectedAsset);

    void createBuildQueue(IAsset* pAsset);

// ===== HUD control

    void clearPanel();

// ===== Input handlers

    void onPickButton(const float frameTime);

    void onPickRelease(const float frameTime);

    void onActionButton(const float frameTime);

    void onActionRelease(const float frameTime);

    void updateBuildButtons();

// ===== Members

    // confs
    int m_KeyMouseActionButton;
    int m_KeyMousePickButton;
    int m_KeyAddToSelection;
    int m_MousePickDragThreshold;
    int m_MouseActionDragThreshold;
    int m_KeyPickModifier;
    int m_KeyActionModifier;
    int m_KeyQueueCommands;
    bool m_KeyActionWhileDragging;
    int m_KeyFirstPersonCamera;

    float m_TooltipTreshold;
    float m_TooltipLifetime;
    bool m_TooltipShown;

    // General state of the assetcontroller
    State                       m_State;

    // container holding the build buttons
    UIContainer*                m_pButtonPanel;
    UIContainer*                m_pInstanceControlPanel;

    // current state of the selection
    PointerState                m_SelectionState;
    PointerState                m_ActionState;

    // temporary values for mouse coordinate
    int m_TempMouseX;
    int m_TempMouseY;

    // device used with picking ray
    LPDIRECT3DDEVICE9           m_pDevice;

    // selector object that is used in the picking of the assets
    Selector*                   m_pSelector;

    // the unit command dispatcher in the model side that is used to do the
    // command dispatching to the actual model-units
    UnitCommandDispatcher*      m_pUnitCommandDispatcher;

    // TODO: either building command dispatcher, or static (non target) command
    // dispatcher for all assets

    DoubleLinkedList<UIAsset*>  m_SelectedUIAssets;
    SelectedType                m_SelectedAssetType; // what kind of assets are selected

//    DoubleLinkedList<BuildButtonWrapper*> m_BuildWrappers;
//    BuildButtonWrapper*         m_pCurrentWrapper;

//    UnitCamera*                 m_pUnitCamera;
    UnitCamera                  m_UnitCamera;
    UIUnit*                     m_pUnitCarryingCamera;

    // who's being controlled
    Player*                     m_pCurrentPlayer;


    // ore taken from player, stored while between build subject selection and finished build (for canceling & refund)
    int             m_CurrentBuildOreTaken;
    AssetDef*       m_pCurrentBuildAssetDef;
    BasicButton*    m_pCurrentBuildButton;
    IAsset*         m_pCurrentBuildAsset;
    // holds buttons for assets that are being build - for updating the buttons
    DoubleLinkedList<BasicButton*> m_ButtonsToUpdate;
    DoubleLinkedList<BasicButton*> m_BuildingButtons;

    BasicButton*    m_pStopButton;
    BasicButton*    m_pNukeButton;

    map<IAsset*,BuildQueue*> m_BuildQueues;
    BuildQueue*     m_pCurrentBuildQueue;

    Building*       m_pCurrentNukeSilo;

};

#endif // __UIASSETCONTROLLER_H__
