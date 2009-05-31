#include "BuildQueue.h"

#include "../../Model/Defs/Defs.h"
#include "../../Model/Defs/DefManager.h"
#include "../UIComponent/RootContainer.h"

#include <stdio.h>

BuildQueue::BuildQueue(IAsset* pAsset)
    : m_pAsset(pAsset)
{
    m_pAsset->registerListener(this);
    m_PosX = m_pAsset->getAbsoluteEntrypointX();
    m_PosY = m_pAsset->getAbsoluteEntrypointY();
    m_CacheTmp = 0;

    // create buttons for what this asset can build
    const int myAssetTag = m_pAsset->getDef()->tag;
    ListNode<AssetDef*>* node = DefManager::getInstance()->getAssetDefNode();
    while(node) {
        if(!node->item->anonymous && node->item->constructionIn == myAssetTag) {
            createBuildingButton(node->item);
        }
        node = node->next;
    }
}

BuildQueue::~BuildQueue()
{
    if(m_pAsset) {
        m_pAsset->unregisterListener(this);
        m_pAsset = 0;
    }
    // remove buttons
    while(delete m_Buttons.popHead()) ;
    // clear tasks
    while(delete m_Queue.popHead()) ;
}

void BuildQueue::createBuildingButton(AssetDef* pAssetDef)
{
    BasicButton* button = new BasicButton(64, 64, pAssetDef->tag, this);
    RootContainer* rc = RootContainer::getInstance();
    // primary texture
    button->setBackgroundTexture(rc->getIconTexture(pAssetDef->tag));
    // alt texture name
    char nameC[64];
    sprintf_s(nameC, "alt_%d", pAssetDef->tag);
    wchar_t nameWTF[10];
    MultiByteToWideChar(CP_ACP, 0, nameC, -1, nameWTF, 10);
    button->setBackgroundTextureClicked(rc->getIconTexture(nameWTF));
    // tooltip
    sprintf_s(nameC, "%s, cost: %d", pAssetDef->name.c_str(), pAssetDef->constructionCostOre);
    button->setTooltip(nameC);
    m_Buttons.pushHead(button);
}

void BuildQueue::fillPanel(UIContainer* pPanel)
{
    ListNode<BasicButton*>* node = m_Buttons.headNode();
    while(node) {
        pPanel->addComponent(node->item);
        node = node->next;
    }
}

void BuildQueue::handleAssetStateChange(IAsset* pAsset, IAsset::State newState)
{
}

void BuildQueue::handleAssetReleased(IAsset* pAsset)
{
    m_pAsset = 0;
}

int BuildQueue::getCount(const int assetTag)
{
    if(m_Queue.empty())
        return 0;
    int count = 0;
    ListNode<BuildTask*>* node = m_Queue.headNode();
    while(node) {
        if(node->item->getAssetTag() == assetTag)
            ++count;
    }
    return count;
}

void BuildQueue::add(BuildTask* pTask)
{
    m_Queue.pushTail(pTask);
}

void BuildQueue::update(bool updateVisual)
{
    if(m_Queue.empty())
        return;
    // remove finished task
    BuildTask* current = m_Queue.peekHead();
    if(current->isFinished()) {
        const int removedId = current->getAssetTag();
        getButton(removedId)->clearLoading();
        delete m_Queue.popHead();
        m_CacheTmp = -1;
        return; // let's check new things in next update, no hurry
    }
    // check if unstarted task could be started
    if(!current->isStarted()) {
        return;
    }
    // we have task that's running, update the visual (button) status if changed
    if(updateVisual && m_CacheTmp != current->getStatusPercentage()) {
        getButton(current->getAssetTag())->setLoadingStatus(current->getStatusPercentage());
        m_CacheTmp = current->getStatusPercentage();
    }
    // @TODO: you could make the panel show the overall status of the queue..
}

BasicButton* BuildQueue::getButton(const int buttonId)
{
    // @TODO: change to map
    ListNode<BasicButton*>* node = m_Buttons.headNode();
    while(node) {
        if(node->item->getId() == buttonId)
            return node->item;
        node = node->next;
    }
    return 0;
}


void BuildQueue::onButtonClick(BasicButton* pSrc)
{
}

void BuildQueue::onButtonAltClick(BasicButton* pSrc)
{
}
