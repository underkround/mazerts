#include "UnitCommandDispatcher.h"


UnitCommandDispatcher::UnitCommandDispatcher(Player* owner) : m_Target(0, 0, false)
{
    // TODO?
}


UnitCommandDispatcher::~UnitCommandDispatcher()
{
    // TODO?
}

void UnitCommandDispatcher::release()
{
    m_Target.release();
}


Target* UnitCommandDispatcher::getTarget() {
    return &m_Target;
/*
    {
        if(m_pCommand)
            return m_pCommand->getTarget();
        return NULL;
    }
*/
}


void UnitCommandDispatcher::addUnit(Unit* unit)
{
    m_Units.pushHead(unit);
}


void UnitCommandDispatcher::setUnit(Unit* unit)
{
    m_Units.release();
    m_Units.pushHead(unit);
}


void UnitCommandDispatcher::clearUnits()
{
    m_Units.release();
}


int UnitCommandDispatcher::getCommandFlagsForTarget()
{
    // TODO!
    return 0;
}

bool UnitCommandDispatcher::dispatch(bool addToQueue)
{
    if(m_Units.empty())
        return false;
    // TODO: if target is coordinate, and there are multiple units selected,
    // alter the target so the unit's wont all try to go in the same coordinate
    ListNode<Unit*>* node = m_Units.headNode();
    while(node)
    {
        if(!addToQueue)
        {
            node->item->getMovingLogic()->clearTargets();
        }
        node->item->getMovingLogic()->addTarget(new Target(m_Target));

        //TODO: Testing, revise: Only set weapon-target when target is asset? (or some key is down or something)
        //Take care of removing the old target before setting new
        Target* pWeaponTarget = node->item->getWeapon()->getTarget();
        if(pWeaponTarget)
        {
            delete pWeaponTarget;
        }
        node->item->getWeapon()->setTarget(new Target(m_Target));

        node = node->next;
    }
    return true;
}
