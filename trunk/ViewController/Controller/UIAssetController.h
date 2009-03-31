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
#include "../Camera/Camera.h"
#include "../Camera/UnitCamera.h"
#include "../../Model/Common/Config.h"

#include "d3d9.h"


class UIAssetController : public IUIController
{
public:

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
        BUILDING
    };

    UIAssetController(const LPDIRECT3DDEVICE9 pDevice, Selector* pSelector);
    ~UIAssetController();

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
    virtual void loadConfiguration(const bool confFileLoaded=false);

    /**
     * Clear all units under selection
     */
    void clearSelection();

private:

    void onPickButton(const float frameTime);

    void onPickRelease(const float frameTime);

    void onActionButton(const float frameTime);

    void onActionRelease(const float frameTime);

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

//    UnitCamera*                 m_pUnitCamera;
    UnitCamera                  m_UnitCamera;
    UIUnit*                     m_pUnitCarryingCamera;

};

#endif // __UIASSETCONTROLLER_H__
