/**
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#include "BuildButtonWrapper.h"

#include "../../Model/Defs/Defs.h"
#include "../../Model/Command/BuildTask.h"
#include "../UIComponent/BasicButton.h"
#include "../UIComponent/UIContainer.h"
#include "../UIComponent/RootContainer.h"

// @TODO: add these to uiassetcontroller
// @TODO: remember to release these at uiassetcontroller
// @TODO: remember to call wrapper's updateCanBuild at some interval

#include "UIAssetController.h"
#include <stdio.h>

BuildButtonWrapper::BuildButtonWrapper(UIAssetController* pController, Player* pPlayer, UIContainer* pButtonPanel, AssetDef* pAssetDef)
    : m_pPlayer(pPlayer), m_pPanel(pButtonPanel), m_pAssetDef(pAssetDef)
{
    m_pTask = 0;

    RootContainer* rc = RootContainer::getInstance();
    // create the button
    m_pButton = new BasicButton(BUILDICON_DEFAULT_SIZE, BUILDICON_DEFAULT_SIZE, pAssetDef->tag, pController);
    // set texture, tooltip, etc
    m_pButton->setBackgroundTexture(rc->getIconTexture(pAssetDef->tag));
    char fnameC[12];
    sprintf(fnameC, "alt_%d", pAssetDef->tag);
    wchar_t fnameWTF[12];
    MultiByteToWideChar(CP_ACP, 0, fnameC, -1, fnameWTF, 12);
    m_pButton->setBackgroundTextureClicked(rc->getIconTexture(fnameWTF));
    m_pButton->setBackgroundClicked(0xFF772222);

    const char* name = m_pAssetDef->name.c_str();
    m_pButton->setTooltip(name);
    m_pButton->setEnabled(false);

    // add button to container
    m_pPanel->addComponent(m_pButton);

    m_pButton->setRandomObject(this);
}

BuildButtonWrapper::~BuildButtonWrapper()
{
    // delete the task (if any)
    if(m_pTask) {
        delete m_pTask;
    }
/*    // remove button from panel & delete
    if(m_pButton) {
        m_pPanel->removeComponent(m_pButton);
        delete m_pButton;
    }*/
}

BuildTask* BuildButtonWrapper::getBuildTask()
{
    return m_pTask;
}

bool BuildButtonWrapper::hasBuildTask() const
{
    return (m_pTask) ? true : false;
}

void BuildButtonWrapper::setBuildTask(BuildTask* pTask)
{
    if(!pTask->isFinished()) {
        if(m_pTask) {
            delete m_pTask;
            m_pTask = 0;
        }
        m_pTask = pTask;
        update();
    } else {
        delete pTask;
    }
}

void BuildButtonWrapper::update()
{
    if(m_pTask) {
        // check if the task is completed
        if(m_pTask->isFinished()) {
            // set the button's status to 100% & enabled, remove task
            delete m_pTask;
            m_pTask = 0;
            m_pButton->clearLoading();
            m_pButton->setEnabled(true);
        }
        else if(m_pTask->isStarted()) {
            if(m_pButton->isEnabled())
                m_pButton->setEnabled(false);
            // sync the status of the task to the button
            m_pButton->setLoadingStatus(m_pTask->getStatusPercentage());
        }
    }
}

void BuildButtonWrapper::updateCanBuild()
{
    // recheck if we can build the asset
    m_CanBuild = (m_pPlayer->getOre() >= m_pAssetDef->constructionCostOre) ? true : false;
    // @TODO: update button accordingly (enabled/disabled)
    if(m_CanBuild) {
        m_pButton->setEnabled(true);
    } else {
        m_pButton->setEnabled(false);
    }
}
