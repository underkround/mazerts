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
#include "../Unit/Selector.h"
#include "../../Model/Command/UnitCommandDispatcher.h"
#include "../Camera/Camera.h"
#include "../../Model/Common/Config.h"

#include "d3d9.h"

class UIAssetController : public IUIController
{
public:

    UIAssetController(const LPDIRECT3DDEVICE9 pDevice, Camera* pCamera, Selector* pSelector);
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
    virtual void loadConfiguration();

private:

    // confs
    int m_KeyMouseDragButton;
    int m_KeyMousePickButton;
    int m_KeyMouseRotateButton;

    // device used with picking ray
    LPDIRECT3DDEVICE9           m_pDevice;

    // selector object that is used in the picking of the assets
    Selector*                   m_pSelector;

    Camera*                     m_pCamera;

    // the unit command dispatcher in the model side that is used to do the
    // command dispatching to the actual model-units
    UnitCommandDispatcher*      m_pUnitCommandDispatcher;

    // TODO: either building command dispatcher, or static (non target) command
    // dispatcher for all assets

    DoubleLinkedList<UIUnit*>   m_SelectedUIUnits;

};

#endif // __UIASSETCONTROLLER_H__
