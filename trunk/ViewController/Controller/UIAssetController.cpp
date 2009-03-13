/**
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#include "UIAssetController.h"

#include "../../Model/Common/Config.h"

UIAssetController::UIAssetController()
{
    m_pUnitCommandDispatcher = new UnitCommandDispatcher(NULL); // TODO: use actual player-object
}


UIAssetController::~UIAssetController()
{
    release();
}


void UIAssetController::loadConfiguration()
{
    Config & c = * Config::getInstance();
    // Assume that the caller of this method (GameState) has loaded the file
    //c.setFilename("controls.ini");
    //c.readFile();
    m_KeyMouseDragButton = c.getValueAsInt("mouse drag button");
    m_KeyMousePickButton = c.getValueAsInt("mouse pick button");
    m_KeyMouseRotateButton = c.getValueAsInt("mouse rotate button");
}

void UIAssetController::create(Selector* selector)
{
    m_pSelector = selector;
}


void UIAssetController::release()
{
    if(m_pUnitCommandDispatcher)
    {
        delete m_pUnitCommandDispatcher;
        m_pUnitCommandDispatcher = NULL;
    }
    m_pSelector = NULL;
}


void UIAssetController::updateControls(float frameTime)
{
}
