/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "Player.h"

#include "../Common/DoubleLinkedList.h"
#include "../Asset/AssetCollection.h"
#include "../Asset/Unit.h"
#include "../Asset/Building.h"
#include "../Defs/Defs.h"
#include "../Defs/DefManager.h"

//for AI loading
#include "../Common/Config.h"
#include <string>

#include "../Asset/RadarBuilding.h"

Player::Player(const int index)
{
    m_Index = index;
    if (index == 0) m_Id = 0;
    else m_Id = 1 << (index - 1);

    m_Enemies = 0;
    m_Allies = 0;
    
    m_pFog = new Fog();
    m_pFog->setOwner(this);

    m_pAI = NULL;
    m_pUnitAI = NULL;
    m_Ore = 0;
    m_EnergyConsumed = 0;
    m_EnergyProduced = 0;
    m_bHasRadar = false;

    //HACK: make all players AI except first one
    Config* c = Config::getInstance();
    string strIsAI = "AI player ";
    strIsAI += Config::intToString(index);

    if(c->getValueAsBool("scenario", strIsAI, false))
    {
        m_pAI = new LameAI(this);
        m_pUnitAI = new CombatAI(this);
    }
}

Player::~Player()
{
    if (m_pFog)
    {
        delete m_pFog;
        m_pFog = NULL;
    }
    if (m_pAI)
    {
        m_pAI->Release();
        delete m_pAI;
        m_pAI = NULL;
    }
    if (m_pUnitAI)
    {
        delete m_pUnitAI;
        m_pUnitAI = NULL;
    }
}

bool Player::hasAsset(int reqTag) const
{
    return hasAsset(DefManager::getInstance()->getAssetDef(reqTag));
}

// @TODO: this is slow.. do not overuse (no better way to get player's asset anyway?)
bool Player::hasAsset(AssetDef* reqDef) const
{
    if(!reqDef)
        return false;
    bool found = false;
    int reqTag = reqDef->tag;
    if(reqDef->concreteType == 2) { // building
        ListNode<Building*>* node = AssetCollection::getAllBuildings()->headNode();
        while(!found && node) {
            if(node->item->getTypeTag() == reqTag && node->item->getOwner() == this && node->item->getState() != IAsset::STATE_BEING_BUILT)
                return true;
            node = node->next;
        }
    } else if(reqDef->concreteType == 1) { // unit
        ListNode<Unit*>* node = AssetCollection::getAllUnits()->headNode();
        while(!found && node) {
            if(node->item->getTypeTag() == reqTag && node->item->getOwner() == this)
                return true;
            node = node->next;
        }
    }
    return false;
}

void Player::UpdateAI(float fFrametime)
{
    if(m_pAI)
    {
        m_pAI->Update(fFrametime);
    }
    if(m_pUnitAI)
    {
        m_pUnitAI->Update(fFrametime);
    }
}

bool Player::CalculateEnergyBalance(void)
{
    //cycle thru our buildings
    int produced = 0;
    int consumed = 0;
    DoubleLinkedList<Building*>* buildings = AssetCollection::getAllBuildings();
    ListNode<Building*>* pNode = buildings->headNode();
    while(pNode)
    {
        if(pNode->item)
        {
            if(pNode->item->getOwner()->getId() == m_Id)
            {
                if(pNode->item->getState() != IAsset::STATE_BEING_BUILT && pNode->item->getState() != IAsset::STATE_DESTROYED ) {
                    produced += pNode->item->getEnergyProduction();
                    consumed += pNode->item->getEnergyConsumption();
                }
            }
        }
        pNode = pNode->next;
    }

	setEnergyProduced(produced);
	setEnergyConsumed(consumed);

    //if total is negative, shut down buildings, otherwise, turn them on
    if(produced < consumed)
    {
        PowerSwitch(false);
        return false;
    }
    else
    {
        PowerSwitch(true);
        return true;
    }
}

void Player::PowerSwitch(bool enable)
{
    //go thru buildings that consume energy and enable (true)/disable (false) them
    DoubleLinkedList<Building*>* buildings = AssetCollection::getAllBuildings();
    ListNode<Building*>* pNode = buildings->headNode();
    while(pNode)
    {
        Building* b = (Building*)pNode->item;
        if(b)
        {
            if(b->getOwner()->getId() == m_Id && b->getState() != IAsset::STATE_DESTROYED)
            {
                if(b->getEnergyConsumption() > 0)
                {
                    if(b->getDef()->tag != BUILDING_TYPE_RADAR)
                    {
                        b->setPower(enable);
                    }
                    else {
                        ((RadarBuilding*)b)->setPower(enable);
                    }
                }
            }
        }
        pNode = pNode->next;
    }
}

const int Player::getPredictedEnergyProduction()
{
    int produced = 0;
    DoubleLinkedList<Building*>* buildings = AssetCollection::getAllBuildings();
    ListNode<Building*>* pNode = buildings->headNode();
    while(pNode)
    {
        if(pNode->item->getOwner()->getId() == m_Id)
        {
            if(pNode->item->getState() != IAsset::STATE_DESTROYED ) {
                produced += pNode->item->getEnergyProduction();
            }
        }
        pNode = pNode->next;
    }
	return produced;
}

const int Player::getPredictedEnergyConsumption()
{
    int consumed = 0;
    DoubleLinkedList<Building*>* buildings = AssetCollection::getAllBuildings();
    ListNode<Building*>* pNode = buildings->headNode();
    while(pNode)
    {
        if(pNode->item->getOwner()->getId() == m_Id)
        {
            if(pNode->item->getState() != IAsset::STATE_DESTROYED ) {
                consumed += pNode->item->getEnergyConsumption();
            }
        }
        pNode = pNode->next;
    }
    return consumed;
}
