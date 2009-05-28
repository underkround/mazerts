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
#include "../Asset/Building.h"

//for AI loading
#include "../Common/Config.h"
#include <string>

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
        if(pNode->item->getOwner()->getId() == m_Id)
        {
            if(pNode->item->getState() != IAsset::STATE_BEING_BUILT && pNode->item->getState() != IAsset::STATE_DESTROYED ) {
                produced += pNode->item->getEnergyProduction();
                consumed += pNode->item->getEnergyConsumption();
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
        if(b->getOwner()->getId() == m_Id && b->getState() != IAsset::STATE_DESTROYED)
        {
            if(b->getEnergyConsumption() > 0)
            {
                b->setPower(enable);
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
