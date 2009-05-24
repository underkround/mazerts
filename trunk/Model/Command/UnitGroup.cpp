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
    //TODO: testing, set assets to follow if the target is not static
    // also set some range to the flag so that the units don't prod each other so much
    if(!pTarget->isStatic())
    {
        pTarget->setFlag(Target::TGTFLAG_REPEATING_UNTIL_STATIC);
        pTarget->setRange(20.0f);
    }

    bool addToQueue = false;

    ListNode<Unit*>* n = m_Group.headNode();
    int step = 1;
    while (n)
    {
        if (n->item->getDef()->width > step) step = n->item->getDef()->width;
        n = n->next;
    }

    step += 2;
    const int square = (int)ceilf(sqrtf((float)m_Group.count()));
    int xoff = (-square >> 1) * step;
    int yoff = (-square >> 1) * step;

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
                //Only move to target
//                node->item->getMovingLogic()->addTarget(new Target(m_Target));
                node->item->getMovingLogic()->addTarget(new Target(pTarget->getTargetX() + xoff, pTarget->getTargetY() + yoff));
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
            node->item->getMovingLogic()->addTarget(new Target(pTarget->getTargetX() + xoff, pTarget->getTargetY() + yoff));
            xoff += step;
            if (xoff >= square >> 1) 
            {
                yoff += step;
                xoff = (-square >> 1) * step;
            }
        }

        node = node->next;
    }

    pTarget->clearFlags();
}

void UnitGroup::addUnit(Unit* pUnit)
{ 
    m_Group.pushTail(pUnit);
    pUnit->registerListener(this);
}

void UnitGroup::removeUnit(Unit* pUnit)
{
    handleAssetReleased(pUnit);
    pUnit->unregisterListener(this);
}
