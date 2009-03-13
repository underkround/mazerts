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

#include "../../Model/Common/Config.h"

class UIAssetController : public IUIController
{
public:

    UIAssetController();
    ~UIAssetController();

    /**
     * Create, doh..
     */
    void create(Selector* selector);

    /**
     * Release all resources
     */
    void release();

    /**
     * Read input devices and do the things
     */
    virtual void updateControls(float frameTime);

    /**
     * @see IUIController
     */
    virtual void loadConfiguration();

private:

    // confs
    int m_KeyMouseDragButton;
    int m_KeyMousePickButton;
    int m_KeyMouseRotateButton;

    // selector object that is used in the picking of the assets
    Selector*               m_pSelector;

    // the unit command dispatcher in the model side that is used to do the
    // command dispatching to the actual model-units
    UnitCommandDispatcher*  m_pUnitCommandDispatcher;

    // TODO: either building command dispatcher, or static (non target) command
    // dispatcher for all assets

};

#endif // __UIASSETCONTROLLER_H__
