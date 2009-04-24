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

void UnitCommandDispatcher::removeAsset(IAsset* asset)
{
    switch(asset->getAssetType())
    {
    case IAsset::UNIT:
        m_Units.remove((Unit*)asset);
        break;
    case IAsset::BUILDING:
        // m_Buildings.remove((Building*)asset); // TODO
        break;
    }
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

    //TODO: testing, set assets to follow if the target is not static
    // also set some range to the flag so that the units don't prod each other so much
    if(!m_Target.isStatic())
    {
        m_Target.setFlag(Target::TGTFLAG_REPEATING_UNTIL_STATIC);
        m_Target.setRange(20.0f);
    }

    ListNode<Unit*>* node = m_Units.headNode();
    while(node)
    {
        if(!addToQueue)
        {
            node->item->getMovingLogic()->clearTargets();
        }
                
        if(m_Target.getTargetAsset())
        {
            //TODO: Force attack
            if(node->item->getWeapon() && m_Target.getTargetAsset()->getOwner() != node->item->getOwner())
            {
                //Enemy unit, set target range based on weapon range
                m_Target.setRange(node->item->getWeapon()->getDef()->range - 2.0f);                
                //Also gives the target to moving logic
                node->item->getWeapon()->setTarget(new Target(m_Target));
            }
            else
            {
                //Only move to target
                node->item->getMovingLogic()->addTarget(new Target(m_Target));
            }
        }
        else
        {
            //Only move to target
            node->item->getMovingLogic()->addTarget(new Target(m_Target));
        }

        node = node->next;
    }

    m_Target.clearFlags();

    return true;
}
