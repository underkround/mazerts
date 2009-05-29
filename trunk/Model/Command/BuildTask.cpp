/**
 *
 */
#include "BuildTask.h"

#include "../Defs/DefManager.h"
#include "../Asset/AssetCollection.h"
#include "../Asset/AssetFactory.h"
#include "../Asset/Building.h"
#include "../Common/DoubleLinkedList.h"

// ----- static stuff


bool BuildTask::canBuild(Player* pPlayer, int assetDefTag)
{
    return canBuild(pPlayer, DefManager::getInstance()->getAssetDef(assetDefTag));
}

bool BuildTask::canBuild(Player* pPlayer, AssetDef* assetDef)
{
    if(!pPlayer || !assetDef)
        return false;

    // check for sufficient resources
    if(assetDef->constructionCostOre > pPlayer->getOre())
        return false; // insufficient funds

    // check builder asset
    int reqTag = 0;
    reqTag = assetDef->constructionIn;
    if(reqTag > 0) {
        // check that the player has the asset
        if(!pPlayer->hasAsset(reqTag))
            return false;
    }

    // check requirement
    reqTag = assetDef->constructionRequires;
    if(reqTag > 0) {
        // check that the player has the asset
        if(!pPlayer->hasAsset(reqTag))
            return false;
    }

    // ok to build
    return true;
}

BuildTask* BuildTask::createTask(Player* pPlayer, int assetDefTag)
{
    return createTask(pPlayer, DefManager::getInstance()->getAssetDef(assetDefTag));
}

BuildTask* BuildTask::createTask(Player* pPlayer, AssetDef* assetDef)
{
    if(!canBuild(pPlayer, assetDef) || !pPlayer || !assetDef)
        return 0;
    return new BuildTask(pPlayer, assetDef);
}

// ----- instance stuff

BuildTask::BuildTask(Player* pPlayer, AssetDef* pAssetDef)
    : m_pPlayer(pPlayer), m_pAssetDef(pAssetDef)
{
    m_Started = false;
    m_Finished = false;
    m_pAsset = 0;
    m_PosX = -1;
    m_PosY = -1;
    // if the asset is build inside other asset, get it's entry point
    if(pAssetDef->constructionIn > 0) {
        bool found = false;
        ListNode<Building*>* node = AssetCollection::getAllBuildings()->headNode();
        while(node && !found) {
            if(node->item->getTypeTag() == pAssetDef->constructionIn && node->item->getOwner() == m_pPlayer) {
                // found possible builder
                m_PosX = node->item->getGridX() + node->item->getDef()->gridEntrancePointX;
                m_PosY = node->item->getGridY() + node->item->getDef()->gridEntrancePointY;
                // check if there is already unit building in entrypoint
                // @TODO: better solution ;P this might result overqueue with the last factory
                if(AssetCollection::getAssetsAt(0, m_PosX, m_PosY, m_pAssetDef->width, m_pAssetDef->height) < 1) {
                    // free place, build here
                    found = true;
                }
                // else, keep searching (coordinates are kept if this was last possibility)
            }
            node = node->next;
        }
    }
}

BuildTask::~BuildTask()
{
    m_pAssetDef = 0;
    m_pPlayer = 0;
}

bool BuildTask::isFreeToBuild() const
{
    if(!hasPosition())
        return false;
    return (AssetCollection::getAssetsAt(0, m_PosX, m_PosY, m_pAssetDef->width, m_pAssetDef->height)) ? false : true;
}

bool BuildTask::canAfford() const
{
    return (m_pPlayer->getOre() >= m_pAssetDef->constructionCostOre) ? true : false;
}

bool BuildTask::execute()
{
    if(m_Started || m_Finished)
        return false;
    if(!hasPosition())
        return false;
    // check if the position is free to build
    if(!isFreeToBuild())
        return false;
    if(!canAfford())
        return false;
    // create the asset
    m_pAsset = AssetFactory::createAsset(m_pPlayer, m_pAssetDef->tag, m_PosX, m_PosY, true);
    if(!m_pAsset)
        return false;
    // reduce ore
    m_pPlayer->modifyOre( -m_pAssetDef->constructionCostOre );
    m_Started = true;
    return true;
}

bool BuildTask::requiresPosition() const
{
    // 1) all buildings require position
    // 2) assets that don't have constructionIn, require position
    return (m_pAssetDef->concreteType == IAsset::BUILDING) ? true
        : (m_pAssetDef->constructionIn > 0) ? true : false;
}

int BuildTask::getStatusPercentage()
{
    if(!m_Started || !m_pAsset)
        return 0;
    if(isFinished())
        return 100;
    return (int)( (m_pAsset->getHitpointsMax() / m_pAsset->getHitpoints()) * 100);
}

bool BuildTask::isFinished()
{
    if(!m_Started || !m_pAsset)
        return false;
    if(m_pAsset->getState() != IAsset::STATE_BEING_BUILT)
        m_Finished = true;
    return m_Finished;
}
