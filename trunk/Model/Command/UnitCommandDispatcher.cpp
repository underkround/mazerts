#include "UnitCommandDispatcher.h"


//
// NOTE!
// lines commented out with "TEMP:" are zemm's additions for what seems to
// avoid breaking pointers when selected assets are being destroyed, but
// it was laying around uncommited few weeks and i forgot if it worked or not..
// to:
// a) avoid breaking the game right now with tight schedule
// b) avoid rewriting the same thing again later
//
// I now add these to commit, but comment them out ;P sorry about the mess,
// you can find all stuff related with "//TEMP" prefix.
//


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

//TEMP void UnitCommandDispatcher::handleAssetStateChange(IAsset* pAsset, IAsset::State newState) { }

//TEMP void UnitCommandDispatcher::handleAssetReleased(IAsset* pAsset) { removeAsset(pAsset); }

void UnitCommandDispatcher::removeAsset(IAsset* asset)
{
//TEMP    asset->unregisterListener(this);
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
//TEMP    unit->registerListener(this);
}


void UnitCommandDispatcher::setUnit(Unit* unit)
{
    m_Units.release();
    m_Units.pushHead(unit);
//TEMP    unit->registerListener(this);
}


void UnitCommandDispatcher::clearUnits()
{
//TEMP    ListNode<Unit*>* node = m_Units.headNode();
//TEMP    while(node) {
//TEMP        node->item->unregisterListener(this);
//TEMP        node = node->next;
//TEMP    }
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
    //TODO: testing, set assets to follow if the target is not static
    // also set some range to the flag so that the units don't prod each other so much
    if(!m_Target.isStatic())
    {
        m_Target.setFlag(Target::TGTFLAG_REPEATING_UNTIL_STATIC);
        m_Target.setRange(20.0f);
    }

    ListNode<Unit*>* n = m_Units.headNode();
    int step = 1;
    while (n)
    {
        if (n->item->getDef()->width > step) step = n->item->getDef()->width;
        n = n->next;
    }

    step += 2;
    const int square = (int)ceilf(sqrtf((float)m_Units.count()));
    int xoff = (-square >> 1) * step;
    int yoff = (-square >> 1) * step;

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
//                node->item->getMovingLogic()->addTarget(new Target(m_Target));
                node->item->getMovingLogic()->addTarget(new Target(m_Target.getTargetX() + xoff, m_Target.getTargetY() + yoff));
                xoff += step;
                if (xoff >= square >> 1) 
                {
                    yoff += step;
                    xoff = (-square >> 1) * step;
                }
            }
        }
        else
        {
            //Only move to target
            node->item->getMovingLogic()->addTarget(new Target(m_Target.getTargetX() + xoff, m_Target.getTargetY() + yoff));
            xoff += step;
            if (xoff >= square >> 1) 
            {
                yoff += step;
                xoff = (-square >> 1) * step;
            }
        }

        node = node->next;
    }

    m_Target.clearFlags();

    return true;
}
