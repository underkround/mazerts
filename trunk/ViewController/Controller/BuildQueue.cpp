#include "BuildQueue.h"

#include "../../Model/Defs/Defs.h"
#include "../../Model/Defs/DefManager.h"
#include "../../Model/Asset/AssetCollection.h"
#include "../../Model/Asset/AssetFactory.h"
#include "../../Model/Asset/Unit.h"
#include "../../Model/Command/Target.h"

#include "../UIComponent/RootContainer.h"

#include <stdio.h>

BuildQueue::BuildQueue(IAsset* pAsset)
    : m_pAsset(pAsset)
{
    m_pAsset->registerListener(this);
    m_PosX = m_pAsset->getAbsoluteEntrypointX();
    m_PosY = m_pAsset->getAbsoluteEntrypointY();
    m_CacheTmp = 0;
    m_pCurrentBuild = 0;

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
    button->setRandomObject(pAssetDef);
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
    ListNode<AssetDef*>* node = m_Queue.headNode();
    while(node) {
        if(node->item->tag == assetTag)
            ++count;
    }
    return count;
}


void BuildQueue::update(bool updateVisual)
{
    if(!m_pAsset)
    {
        return;
    }

    if(m_pAsset->getState() != IAsset::STATE_ACTIVE)
        return;
    // update current
    if(m_pCurrentBuild) {
        BasicButton* button = getButton(m_pCurrentBuild->getDef()->tag);
        // remove current if it's build
        if(m_pCurrentBuild->getState() != IAsset::STATE_BEING_BUILT) {
            getButton(m_pCurrentBuild->getDef()->tag)->clearLoading();
            m_CacheTmp = -1;
            button->clearLoading();
            m_pCurrentBuild = 0;
            return; // let's check new things in next update, no hurry
        } else {
            if(updateVisual && m_CacheTmp != button->getLoadingValue()) {
                const int percentage = (int)(m_pCurrentBuild->getHitpoints() / m_pCurrentBuild->getHitpointsMax() * 100);
                button->setLoadingStatus(percentage);
            }
        }
    }
    else {
        // @TODO: check if the place is open
        AssetDef* top = m_Queue.peekHead();
        if(top) {
            DoubleLinkedList<IAsset*>* list = new DoubleLinkedList<IAsset*>();
            AssetCollection::getAssetsAt(list, m_PosX-2, m_PosY-2, top->width+4, top->height+4);
            if(list->empty() || (list->count() == 1 && list->peekHead() == m_pAsset)) {
                m_pCurrentBuild = AssetFactory::createAsset(m_pAsset->getOwner(), top->tag, m_PosX, m_PosY, true);
                if(m_pCurrentBuild) {
                    if(m_pCurrentBuild->getAssetType() == IAsset::UNIT) {
                        Unit* u = (Unit*)m_pCurrentBuild;
                        if(u->getMovingLogic()) {
                            // go away when finished
                            // @TODO: make this better!
                            u->getMovingLogic()->addTarget(new Target(m_pAsset->getCenterGridX(), m_pAsset->getGridY() - u->getHeight(), false));
                        }
                    }
                    getButton(top->tag)->clearLoading();
                    m_Queue.popHead();
                }
            }
            delete list;
        }
    }
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


bool BuildQueue::add(AssetDef* pAssetDef)
{
    if(m_pAsset->getOwner()->getOre() < pAssetDef->constructionCostOre)
        return false;
    m_pAsset->getOwner()->modifyOre(-pAssetDef->constructionCostOre);
    m_Queue.pushTail(pAssetDef);
    return true;
}

bool BuildQueue::cancel(AssetDef* pAssetDef)
{
    const int buildCost = pAssetDef->constructionCostOre;
    if(m_Queue.remove(pAssetDef)) {
        // refund
        m_pAsset->getOwner()->modifyOre(buildCost);
        return true;
    }
    return false;
}

void BuildQueue::onButtonClick(BasicButton* pSrc)
{
    AssetDef* def = (AssetDef*)pSrc->getRandomObject();
    if(def) {
        add(def);
        if(!m_pCurrentBuild || m_pCurrentBuild->getDef() != def) {
            getButton(def->tag)->setLoadingStatus(0);
        }
    }
}

void BuildQueue::onButtonAltClick(BasicButton* pSrc)
{
    AssetDef* def = (AssetDef*)pSrc->getRandomObject();
    if(def) {
        cancel(def);
        if(getCount(def->tag) < 1) {
            getButton(def->tag)->clearLoading();
        } else {
            getButton(def->tag)->setLoadingStatus(0);
        }
    }
}
