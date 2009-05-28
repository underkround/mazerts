/**
 * UnitGroup
 *
 * Pretty much same as UnitCommandDispatcher, but actually contains pointers to
 * units in it.
 *
 * Used in AI, could be suitable for groups for human player.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __ASSETGROUP_H__
#define __ASSETGROUP_H__

class Unit;
#include "../Common/DoubleLinkedList.h"
#include "../Asset/IAssetListener.h"
#include "Target.h"

class UnitGroup : public IAssetListener
{
public:
    UnitGroup(void);
    virtual ~UnitGroup(void);
    inline DoubleLinkedList<Unit*>* getUnits() { return &m_Group; }

    /**
     * Adds unit to group. Doesn't set target
     */
    void addUnit(Unit* pUnit);

    /**
     * Removes unit from group
     */
    void removeUnit(Unit* pUnit);

    /**
     * Does nothing
     */
    void handleAssetStateChange(IAsset* pAsset, IAsset::State newState);

    /**
     * Removes unit from group
     */
    void handleAssetReleased(IAsset* pAsset);

    /**
     * Sets the target of the group. Takes a copy.
     */
    void setTarget(Target* target);

private:
    DoubleLinkedList<Unit*> m_Group; // actual group of units
};

#endif
