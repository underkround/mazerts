/**
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#include "BuildButtonWrapper.h"

// @TODO: add these to uiassetcontroller
// @TODO: remember to release these at uiassetcontroller
// @TODO: remember to call wrapper's updateCanBuild at some interval

BuildButtonWrapper::BuildButtonWrapper(Player* pPlayer, UIContainer* pButtonPanel, AssetDef* pAssetDef)
    : m_pPlayer(pPlayer), m_pPanel(pButtonPanel), m_pAssetDef(pAssetDef)
{
    m_pTask = 0;
    m_CanBuild = false;
    // @TODO: create the button
    // @TODO: set texture, tooltip, etc
    // @TODO: add button to container
    updateCanBuild(); // update canbuild status
}

BuildButtonWrapper::~BuildButtonWrapper()
{
    // @TODO: remove button from panel
    // @TODO: delete the button (if any)
    // @TODO: delete the task (if any)
}

void BuildButtonWrapper::update()
{
    if(m_pTask) {
        // check if the task is completed
        if(m_pTask->isFinished()) {
            // @TODO: set the button's status to 100% & enabled
        }
        // sync the status of the task to the button
    }
}

void BuildButtonWrapper::updateCanBuild()
{
    // @TODO: recheck if we can build the asset
    m_CanBuild = BuildTask::canBuild(m_pPlayer, m_pAssetDef);
    // @TODO: update button accordingly (enabled/disabled)
}
