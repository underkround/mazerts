#include "CombatAI.h"

#include "../Common/DoubleLinkedList.h"
#include "../Asset/Unit.h"
#include "../Command/Target.h"
#include "../Terrain/Terrain.h"

//define how often combat AI thinks it's decisions
#define COMBAT_AI_UPDATE_INTERVAL 5.0f

CombatAI::CombatAI(Player* player)
{
    m_fUpdatetime = 0.0f;
    m_pPlayer = player;
}

CombatAI::~CombatAI(void)
{
}

void CombatAI::Update(float fFrameTime)
{
    m_fUpdatetime += fFrameTime;
    if(m_fUpdatetime > COMBAT_AI_UPDATE_INTERVAL) {
        m_fUpdatetime -= COMBAT_AI_UPDATE_INTERVAL;
        //do your magic
        EvasiveManeuver();
    }
}

void CombatAI::EvasiveManeuver()
{
    DoubleLinkedList<Unit*>* units = AssetCollection::getAllUnits();
    ListNode<Unit*>* pNode = units->headNode();
    while(pNode)
    {
        if(pNode->item->getOwner() == m_pPlayer)
        {
            int size = Terrain::getInstance()->getSize();
            pNode->item->getMovingLogic()->clearTargets();
            pNode->item->getMovingLogic()->addTarget(new Target(rand() % size, rand() % size, false, 0));
        }
        pNode = pNode->next;
    }
}