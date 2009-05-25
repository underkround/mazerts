#include "CombatAI.h"

#include "../../Common/DoubleLinkedList.h"
#include "../../Asset/Unit.h"
#include "../../Asset/Building.h"
#include "../../Command/Target.h"
#include "../../Terrain/Terrain.h"
#include "../../Command/Target.h"
#include "../../Command/UnitGroup.h"
#include "../../Console.h"

CombatAI::CombatAI(Player* player)
{
    m_fUpdatetime = 0.0f;
    m_pPlayer = player;
    m_pDefenseGroup = new UnitGroup();
    m_pAttackGroup = new UnitGroup();
    m_pReserveGroup = new UnitGroup();
    m_Defending = false;
    m_Attacking = false;
    m_pAttackTarget = NULL;
    m_pDefenseTarget = NULL;
    m_DefensePointX = 0;
    m_DefensePointY = 0;
    m_RallyPointX = 0;
    m_RallyPointY = 0;
    m_UpdatesSinceAttackBegan = 0;
    m_PrintOutput = false;

    m_TargetFlags = 0;//Target::TGTFLAG_MAKEWAY;

    LoadConfigFromFile();

    AssetCollection::registerListener(this);
}

CombatAI::~CombatAI(void)
{
    if (m_pDefenseGroup) {
        delete m_pDefenseGroup;
        m_pDefenseGroup = NULL;
    }
    if (m_pAttackGroup) {
        delete m_pAttackGroup;
        m_pAttackGroup = NULL;
    }
    if (m_pReserveGroup) {
        delete m_pReserveGroup;
        m_pReserveGroup = NULL;
    }
}

//#include "../../../ViewController/App/Timer.h"

void CombatAI::Update(float fFrameTime)
{
    m_fUpdatetime += fFrameTime;
    if(m_fUpdatetime > m_CombatUpdateInterval) {
        m_fUpdatetime = 0;
/*
        CTimer t;
        t.Create();
        t.BeginTimer();
*/
//#ifdef _DEBUG
        if (m_pPlayer->getIndex() == 2)
        {
            m_PrintOutput = true;
        }
        else
        {
            m_PrintOutput = false;
        }
//#endif

        if (m_PrintOutput)
        {
            char* msg = new char[128];
            sprintf_s(msg, 128, "--- START COMBAT UPDATE SEQUENCE FOR PLAYER %d", m_pPlayer->getIndex());
            Console::debug(msg);
        }

        // TODO: This really could do some optimizing
        FindBaseCenterPoint(&m_DefensePointX, &m_DefensePointY);
        // set rally point
        unsigned int s = Terrain::getInstance()->getSize();
        int rallyoffx = m_RallyPointDistance;
        int rallyoffy = m_RallyPointDistance;
        if (m_DefensePointX > (s >> 1)) rallyoffx = -rallyoffx;
        if (m_DefensePointY > (s >> 1)) rallyoffy = -rallyoffy;
        m_RallyPointX = m_DefensePointX + rallyoffx;
        m_RallyPointY = m_DefensePointY + rallyoffy;

        // DEFENSE ============================================================
        // check if our defense team is dead
        if (m_Defending && m_pDefenseGroup->getUnits()->empty())
        {
            if (m_PrintOutput) Console::debug("Defense: group is dead");
            clearDefense();
        }

        if (m_Defending)
        {
            // We're already defending
            if (m_PrintOutput) Console::debug("Defense: already defending");

            // check if enemies are dead or ran away from defense perimeter
            if (m_pDefenseTarget)
            {
                // we have target

                if (m_pDefenseTarget->getTargetType() != Target::COORDINATE)
                {
                    // target hasn't been destroyed
                    float distance = unitDistance(m_pDefenseTarget->getTargetAsset()->getCenterGridX(),
                        m_pDefenseTarget->getTargetAsset()->getCenterGridY(), m_DefensePointX, m_DefensePointY);
                    if (distance > m_DefenseDistanceMax)
                    {
                        // target ran away
                        if (m_PrintOutput) Console::debug("Defense: target ran away");
                        clearDefense();
                    }
                    else
                    {
                        // same old target needs more ass-kicking to do
                        if (!m_pReserveGroup->getUnits()->empty())
                        {
                            // more units to assign
                        if (m_PrintOutput)
                        {
                            char* msg = new char[128];
                            sprintf_s(msg, 128, "Defense: still same target (%s @ %d, %d), assigning reserves", m_pDefenseTarget->getTargetAsset()->getDef()->name.c_str(), m_pDefenseTarget->getTargetX(), m_pDefenseTarget->getTargetY());
                            Console::debug(msg);
                        }
                        if (m_PrintOutput) Console::debug("Defense: ");
                            m_pReserveGroup->setTarget(m_pDefenseTarget);
                            assignReservesToGroup(m_pDefenseGroup);
                        }
                        else
                        {
                            // everybody is already doing something
                            if (m_PrintOutput)
                            {
                                char* msg = new char[128];
                                sprintf_s(msg, 128, "Defense: still same target (%s @ %d, %d), but no reserves to assign", m_pDefenseTarget->getTargetAsset()->getDef()->name.c_str(), m_pDefenseTarget->getTargetX(), m_pDefenseTarget->getTargetY());
                                Console::debug(msg);
                            }
                        }
                    }
                }
                else
                {
                    // target destroyed
                    if (m_PrintOutput) Console::debug("Defense: target destroyed");
                    clearDefense();
                }
            }
            else
            {
                // no target? you shouldn't get here
                if (m_PrintOutput) Console::debug("Defense: lol apua");
                clearDefense();
            }
        }

        if (!m_Defending)
        {
            if (m_pReserveGroup->getUnits()->empty())
            {
                // no need to check, we don't have anyone here to defend
                if (m_PrintOutput) Console::debug("Defense: no-one in reserve, can't defend");
            }
            else
            {
                if (m_pDefenseTarget)
                {
                    // you shouldn't get here, but just in case
                    if (m_PrintOutput) Console::debug("Defense: lol apua2");
                    clearDefense();
                }
                if (m_PrintOutput) Console::debug("Defense: checking if defense is needed");
                m_pDefenseTarget = needDefending();
                if (m_pDefenseTarget != NULL)
                {
                    if (!m_pReserveGroup->getUnits()->empty())
                    {
                        // defending is needed
                        if (m_PrintOutput)
                        {
                            char* msg = new char[128];
                            sprintf_s(msg, 128, "Defense: target acquired: %s @ %d, %d", m_pDefenseTarget->getTargetAsset()->getDef()->name.c_str(), m_pDefenseTarget->getTargetX(), m_pDefenseTarget->getTargetY());
                            Console::debug(msg);
                        }
                        m_pReserveGroup->setTarget(m_pDefenseTarget);
                        assignReservesToGroup(m_pDefenseGroup);
                        m_Defending = true;
                    }
                    else
                    {
                        if (m_PrintOutput) Console::debug("Defense: target aqcuired but no reserves to assign");
                    }
                }
                else
                {
                    if (m_PrintOutput) 
                        Console::debug("Defense: no defense needed");
                }
            }
        }
        // END DEFENSE ========================================================

        // OFFENSE ============================================================
        // check if our offense team is dead
        if (m_Attacking && m_pAttackGroup->getUnits()->empty())
        {
            if (m_PrintOutput) Console::debug("Offense: group is dead");
            clearAttacks();
        }

        if (!m_Defending && !m_Attacking || (m_Attacking && m_pAttackTarget->getTargetType() == Target::COORDINATE))
        {
            // not already defending or attacking, or target already destroyed
            if (m_pAttackTarget)
            {
                // target is destroyed
                if (m_PrintOutput) Console::debug("Offense: target destroyed");
                clearAttacks();
            }

            if (readyToAttack())
            {
                // ready to attack
                if (m_PrintOutput) Console::debug("Offense: ready to attack");


                // TODO: this could do some improving (if units are already attacking,
                // pick nearest(?) enemy.
                float threat = 0.0f;
                if (m_PrintOutput) Console::debug("Offense: looking for suitable targets");
                m_pAttackTarget = findAttackTarget(&threat);
                if (m_pAttackTarget)
                {
                    // new target found
                    if (m_PrintOutput)
                    {
                        char* msg = new char[128];
                        sprintf_s(msg, 128, "Offense: target acquired: %s @ %d, %d // t: %4.2f", m_pAttackTarget->getTargetAsset()->getDef()->name.c_str(), m_pAttackTarget->getTargetX(), m_pAttackTarget->getTargetY(), threat);
                        Console::debug(msg);
                    }
                    if (!m_pReserveGroup->getUnits()->empty() && assessFeasibility(threat))
                    {
                        // it's feasible to attack
                        if (m_PrintOutput) Console::debug("Offense: attack is feasible, attacking");
                        m_UpdatesSinceAttackBegan = 0;
                        assignReservesToGroup(m_pAttackGroup);
                        m_pAttackGroup->setTarget(m_pAttackTarget);
                        m_Attacking = true;
                    }
                    else
                    {
                        // attacking isn't feasible
                        if (m_PrintOutput) Console::debug("Offense: attack not feasible");
                        clearAttacks();
                    }
                }
                else
                {
                    // target not found
                    if (m_PrintOutput) Console::debug("Offense: no targets found");
                    clearAttacks();
                }
            }
            else
            {
                if (m_PrintOutput) Console::debug("Offense: not ready to attack");
            }
        }
        else
        {
            if (m_Attacking)
            {
                // already attacking
                if (m_PrintOutput)
                {
                    char* msg = new char[128];
                    sprintf_s(msg, 128, "Offense: already attacking %s @ %d, %d", m_pAttackTarget->getTargetAsset()->getDef()->name.c_str(), m_pAttackTarget->getTargetX(), m_pAttackTarget->getTargetY());
                    Console::debug(msg);
                }
                m_UpdatesSinceAttackBegan++;
                if (m_UpdatesSinceAttackBegan > m_AttackTimeout)
                {
                    // for some reason attacking has taken long time, reset attack
                    if (m_PrintOutput) Console::debug("Offense: attack timeout");
                    clearAttacks();
                }
            }
            else
            {
                // defending
                if (m_PrintOutput) Console::debug("Offense: cannot attack while defending");
            }
        }
        // END OFFENSE ========================================================

        //do your magic
//        EvasiveManeuver();
/*
        t.EndTimer();
        float f = t.GetElapsedSeconds();
        char* timex = new char[128];
        sprintf_s(timex, 128, "--- COMBAT UPDATE FOR PLAYER %d TOOK: %f ms", m_pPlayer->getIndex(), f * 1000);
        Console::debug(timex);
*/
    }
}

/*
void CombatAI::EvasiveManeuver()
{
    DoubleLinkedList<Unit*>* units = AssetCollection::getAllUnits();
    ListNode<Unit*>* pNode = units->headNode();
    while(pNode)
    {
        if(pNode->item->getOwner() == m_pPlayer)
        {
            if(rand() < (RAND_MAX / 3))
            {
                int size = Terrain::getInstance()->getSize();
                pNode->item->getMovingLogic()->clearTargets();
                pNode->item->getMovingLogic()->addTarget(new Target(rand() % size, rand() % size, false, 0));
            }
        }
        pNode = pNode->next;
    }
}
*/

Target* CombatAI::needDefending()
{
    DoubleLinkedList<Unit*> enemiesInRange;

    // TODO: this should probably be fixed to use AssetCollection::getUnitsAt()
    ListNode<Unit*>* pNode = AssetCollection::getAllUnits()->headNode();
    while (pNode)
    {
        Unit* pUnit = pNode->item;
        if (m_pPlayer->isEnemy(pUnit->getOwner()))
        {
            if (unitDistance(m_DefensePointX, m_DefensePointY, pUnit->getCenterGridX(), pUnit->getCenterGridY()) < m_DefenseDistanceMin)
            {
                enemiesInRange.pushHead(pUnit);
            }
        }
        pNode = pNode->next;
    }

    if (!enemiesInRange.empty())
    {
        ListNode<Unit*>* pNode = enemiesInRange.headNode();
        IAsset* pTarget = NULL;
        float threat = 0;

        while (pNode)
        {
            IAsset* pAsset = pNode->item;
            if (m_pPlayer->isEnemy(pAsset->getOwner()))
            {
                float f = calculateThreat(pAsset);
                if (f > threat)
                {
                    pTarget = pAsset;
                    threat = f;
                }
            }

            pNode = pNode->next;
        }
        Target* t = new Target(pTarget, m_TargetFlags);
        return t;
    }
    return NULL;
}

Target* CombatAI::findAttackTarget(float* threatasdf)
{
    ListNode<Unit*>* pNode = AssetCollection::getAllUnits()->headNode();
    IAsset* pTarget = NULL;
    float threat = 0;

    while (pNode)
    {
        IAsset* pAsset = pNode->item;
        if (m_pPlayer->isEnemy(pAsset->getOwner()))
        {
            float f = calculateThreat(pAsset);
            if (f > threat)
            {
                pTarget = pAsset;
                threat = f;
            }
        }

        pNode = pNode->next;
    }

    ListNode<Building*>* pNode2 = AssetCollection::getAllBuildings()->headNode();
    while (pNode2)
    {
        IAsset* pAsset = pNode2->item;
        if (m_pPlayer->isEnemy(pAsset->getOwner()))
        {
            float f = calculateThreat(pAsset);
            if (f > threat)
            {
                pTarget = pAsset;
                threat = f;
            }
        }

        pNode2 = pNode2->next;
    }

    *threatasdf = threat;
    Target* t = NULL;

    if (pTarget)
    {
        t = new Target(pTarget, m_TargetFlags);
    }
    return t;
}

void CombatAI::assignReservesToGroup(UnitGroup* g)
{
    if (!m_pReserveGroup->getUnits()->empty())
    {
        ListNode<Unit*>* pNode = m_pReserveGroup->getUnits()->headNode();
        while(pNode && !(g == m_pAttackGroup && m_pReserveGroup->getUnits()->count() <= m_MinUnitsInReserve))
        {
            Unit* pUnit = pNode->item;

            if(pUnit->getWeapon()) g->addUnit(pUnit);

            pUnit->unregisterListener(m_pReserveGroup);
            pNode = m_pReserveGroup->getUnits()->removeGetNext(pNode);
        }
    }
}

void CombatAI::releaseToReserve(UnitGroup* g)
{
    if (!g->getUnits()->empty())
    {
        ListNode<Unit*>* pNode = g->getUnits()->headNode();
        while(pNode)
        {
            Unit* pUnit = pNode->item;

            m_pReserveGroup->addUnit(pUnit);

            pUnit->unregisterListener(g);
            pNode = g->getUnits()->removeGetNext(pNode);
        }
    }
    Target t(m_RallyPointX, m_RallyPointY, false, m_TargetFlags);
    m_pReserveGroup->setTarget(&t);
}

const bool CombatAI::readyToAttack()
{
    bool b = m_pReserveGroup->getUnits()->count() >= m_MinUnitsInReserve + m_MinAttackGroupSize; // enough free units
    return b;
}

const bool CombatAI::assessFeasibility(const float threat)
{
    // TODO: Find out some kind of mechanism to solve whether m_pAttackTarget
    // is attackable
//    bool b = m_pReserveGroup->getUnits()->count() > 3; // enough free units
    bool b = (threat > m_ThreatThreshold); // threat is great enough
//    b = b && ((rand() % 2) == 0); // dirty häx :P
    return b;
}

/*
const unsigned int CombatAI::manhattanDistance(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2) const
{
    int ix1 = x1;
    int ix2 = x2;
    int iy1 = y1;
    int iy2 = y2;
    return abs(ix1 - ix2) + abs(iy1 - iy2);
}
*/

const float CombatAI::unitDistance(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2) const
{
    const int x = x1 - x2;
    const int y = y1 - y2;
    const float fx = (float)(x * x);
    const float fy = (float)(y * y);
    return sqrt(fx + fy);

}

void CombatAI::FindBaseCenterPoint(unsigned int *xCenter, unsigned int *yCenter)
{
    unsigned int count = 0;
    unsigned int xTotal = 0;
    unsigned int yTotal = 0;

    DoubleLinkedList<Building*>* buildings = AssetCollection::getAllBuildings();
    ListNode<Building*>* pNode = buildings->headNode();
    while(pNode)
    {
        if(pNode->item->getOwner() == m_pPlayer)
        {
            ++count;
            xTotal += pNode->item->getCenterGridX();
            yTotal += pNode->item->getCenterGridY();
        }
        pNode = pNode->next;
    }

    if(count > 0)
    {
        *xCenter = xTotal/count;
        *yCenter = yTotal/count;
    }
    else
    { //zero buildings! game over man!
        Terrain* pTerrain = Terrain::getInstance();
        *xCenter = (rand() % pTerrain->getSize() / 2)+pTerrain->getSize()/4;
        *yCenter = (rand() % pTerrain->getSize() / 2)+pTerrain->getSize()/4;
    }
}

const float CombatAI::calculateThreat(IAsset* pAsset)
{
    DoubleLinkedList<IAsset*> list;
    AssetCollection::getAssetsAt(&list, pAsset->getCenterGridX() - (m_ThreatRange >> 1), pAsset->getCenterGridY() - (m_ThreatRange >> 1), m_ThreatRange, m_ThreatRange);

    ListNode<IAsset*>* pNode = list.headNode();
    int threat = 0;
    int ownUnitsInAreaMultiplier = 1;
    while (pNode)
    {
        if (m_pPlayer->isEnemy(pNode->item->getOwner()))
            threat += pNode->item->getDef()->threat;
        else if (pNode->item->getOwner() == m_pPlayer)
            ++ownUnitsInAreaMultiplier;

        pNode = pNode->next;
    }

    // adds own value into threat (so own value is little more weighed)
    threat += pAsset->getDef()->threat;

    // count own units in the area, if there are many make it more suitable for attacking
    threat *= ownUnitsInAreaMultiplier;

    float dist = unitDistance(pAsset->getCenterGridX(), pAsset->getCenterGridY(), m_DefensePointX, m_DefensePointY);
    if (dist < 1.0f) dist = 1.0f;
    return ((Terrain::getInstance()->getSize() << 1) / dist) * threat;
}

void CombatAI::clearDefense()
{
    if (!m_pDefenseGroup->getUnits()->empty())
    {
        releaseToReserve(m_pDefenseGroup);
    }

    if (m_pDefenseTarget)
    {
        delete m_pDefenseTarget;
        m_pDefenseTarget = NULL;
    }

    m_Defending = false;
}

void CombatAI::handleCreatedAsset(IAsset* instance)
{
    if (instance->getAssetType() == IAsset::UNIT && instance->getOwner() == m_pPlayer)
    {
        if (instance->getDef()->concreteType != UNIT_TYPE_MINER)
        {
            m_pReserveGroup->addUnit((Unit*)instance);
            if (m_RallyPointX != 0)
            {
                Target t(m_RallyPointX, m_RallyPointY, false, m_TargetFlags);
                m_pReserveGroup->setTarget(&t);
            }
        }
    }
}

void CombatAI::handleReleasedAsset(IAsset* instance)
{

}

void CombatAI::clearAttacks()
{
    if (!m_pAttackGroup->getUnits()->empty())
    {
        releaseToReserve(m_pAttackGroup);
    }

    if (m_pAttackTarget)
    {
        delete m_pAttackTarget;
        m_pAttackTarget = NULL;
    }

    m_Attacking = false;
}

void CombatAI::LoadConfigFromFile(void)
{
    Config & c = * Config::getInstance();
    c.setFilename("../Data/defs/", "AI.ini");
    c.readFile();

    /*
    c.updateFloat("AICombat", "combat update interval", m_CombatUpdateInterval); // 5.0
    c.updateFloat("AICombat", "defense distance min", m_DefenseDistanceMin); // 125.0
    c.updateFloat("AICombat", "defense distance max", m_DefenseDistanceMax); // 150.0
    c.updateInt("AICombat", "threat range", m_ThreatRange); // 50
    c.updateFloat("AICombat", "threat threshold", m_ThreatThreshold); // 1.0
    c.updateInt("AICombat", "min units in reserve", m_MinUnitsInReserve); // 3
    c.updateInt("AICombat", "min attack group size", m_MinAttackGroupSize); // 3
    c.updateInt("AICombat", "attack timeout", m_AttackTimeout); // 25
    c.updateInt("AICombat", "rally point distance", m_RallyPointDistance); // 30
    */
    m_CombatUpdateInterval = c.getValueAsFloat("AICombat", "combat update interval", 5.0f);
    m_DefenseDistanceMin = c.getValueAsFloat("AICombat", "defense distance min", 125.0f);
    m_DefenseDistanceMax = c.getValueAsFloat("AICombat", "defense distance max", 150.0f);
    m_ThreatRange = c.getValueAsInt("AICombat", "threat range", 50);
    m_ThreatThreshold = c.getValueAsFloat("AICombat", "threat threshold", 1.0f);
    m_MinUnitsInReserve = c.getValueAsInt("AICombat", "min units in reserve", 3);
    m_MinAttackGroupSize = c.getValueAsInt("AICombat", "min attack group size", 3);
    m_AttackTimeout = c.getValueAsInt("AICombat", "attack timeout", 25);
    m_RallyPointDistance = c.getValueAsInt("AICombat", "rally point distance", 30);
}