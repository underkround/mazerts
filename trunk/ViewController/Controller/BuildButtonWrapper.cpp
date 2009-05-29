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

BuildButtonWrapper::BuildButtonWrapper(Player* pPlayer, UIContainer* pButtonPanel, AssetDef* pAssetDef)
    : m_pPlayer(pPlayer), m_pPanel(pButtonPanel), m_pAssetDef(pAssetDef)
{
    m_pTask = 0;
    m_CanBuild = false;

    RootContainer* rc = RootContainer::getInstance();
    // create the button
    m_pButton = new BasicButton(BUILDICON_DEFAULT_SIZE, BUILDICON_DEFAULT_SIZE, pAssetDef->tag, this);
    // set texture, tooltip, etc
    m_pButton->setBackgroundTexture(rc->getIconTexture(pAssetDef->tag));
    //m_pButton->setBackgroundTextureClicked(rc->getIconTexture(pAssetDef->tag));
    m_pButton->setBackgroundClicked(0xFF772222);

    const char* name = m_pAssetDef->name.c_str();
    m_pButton->setTooltip(name);
    m_pButton->setEnabled(false);

    // add button to container
    m_pPanel->addComponent(m_pButton);

    // update canbuild status
    updateCanBuild();
}

BuildButtonWrapper::~BuildButtonWrapper()
{
    // delete the task (if any)
    if(m_pTask) {
        delete m_pTask;
    }
    // remove button from panel & delete
    if(m_pButton) {
        m_pPanel->removeComponent(m_pButton);
        delete m_pButton;
    }
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
    m_CanBuild = BuildTask::canBuild(m_pPlayer, m_pAssetDef);
    // @TODO: update button accordingly (enabled/disabled)
    if(m_CanBuild) {
    }
}


// ===== IButtonListener methods
#include "../../Model/Console.h"

void BuildButtonWrapper::onButtonClick(BasicButton* pSrc)
{
    char* msg = new char[128];
    int newPerc = pSrc->getLoadingValue();
    if(newPerc < 100) {
        newPerc += 5;
        pSrc->setLoadingStatus(newPerc);
    }
    sprintf_s(msg, 128, "button %d clicked, set loading to %d", pSrc->getId(), newPerc);
    Console::debug(msg);
}


void BuildButtonWrapper::onButtonAltClick(BasicButton* pSrc)
{
    char* msg = new char[128];
    int newPerc = pSrc->getLoadingValue();
    if(newPerc > 0) {
        newPerc -= 5;
        pSrc->setLoadingStatus(newPerc);
    }
    sprintf_s(msg, 128, "button %d alt-clicked, set loading to %d", pSrc->getId(), newPerc);
    Console::debug(msg);
}
