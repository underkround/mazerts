#include "UnitGroup.h"
#include "../Asset/IAsset.h"
#include "../Asset/Unit.h"
#include "../Asset/AssetCollection.h"

UnitGroup::UnitGroup(void)
{
}

UnitGroup::~UnitGroup(void)
{
}

void UnitGroup::handleAssetStateChange(IAsset* pAsset, IAsset::State newState)
{
    
}

void UnitGroup::handleAssetReleased(IAsset* pAsset)
{
    ListNode<Unit*>* pNode = m_Group.headNode();
    while (pNode)
    {
        if (pNode->item == pAsset)
        {
            m_Group.remove(pNode);
            return;
        }
        pNode = pNode->next;
    }
}

void UnitGroup::setTarget(Target* pTarget)
{
    // this is stolen straight from unitcommanddispatcher.dispatch
    if(m_Group.empty())
        return;
    // TODO: if target is coordinate, and there are multiple units selected,
    // alter the target so the unit's wont all try to go in the same coordinate

    //TODO: testing, set assets to follow if the target is not static
    // also set some range to the flag so that the units don't prod each other so much
    if(!pTarget->isStatic())
    {
        pTarget->setFlag(Target::TGTFLAG_REPEATING_UNTIL_STATIC);
        pTarget->setRange(20.0f);
    }

    bool addToQueue = false;

    ListNode<Unit*>* node = m_Group.headNode();
    while(node)
    {
        if(!addToQueue)
        {
            node->item->getMovingLogic()->clearTargets();
        }

        if(pTarget->getTargetAsset())
        {
            //TODO: Force attack
            if(node->item->getWeapon() && pTarget->getTargetAsset()->getOwner() != node->item->getOwner())
            {
                Target* t = new Target(*pTarget);
                //Enemy unit, set target range based on weapon range
                t->setRange(node->item->getWeapon()->getDef()->range - 2.0f);                
                //Also gives the target to moving logic
                node->item->getWeapon()->setTarget(t);
            }
            else
            {
                //((Unit*)node->item)->getMovingLogic()->clearTargets(); //TODO: test
                ((Unit*)node->item)->getMovingLogic()->addTarget(new Target(*pTarget));
            }
        }
        else
        {
            //((Unit*)node->item)->getMovingLogic()->clearTargets(); //TODO: test
            ((Unit*)node->item)->getMovingLogic()->addTarget(new Target(*pTarget));
        }

        node = node->next;
    }

    pTarget->clearFlags();
}

void UnitGroup::addUnit(Unit* pUnit)
{ 
    m_Group.pushHead(pUnit);
    pUnit->registerListener(this);
}

void UnitGroup::removeUnit(Unit* pUnit)
{
    handleAssetReleased(pUnit);
    pUnit->unregisterListener(this);
}
