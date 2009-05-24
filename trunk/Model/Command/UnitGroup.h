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

    void addUnit(Unit* pUnit);
    void removeUnit(Unit* pUnit);

    void handleAssetStateChange(IAsset* pAsset, IAsset::State newState);
    void handleAssetReleased(IAsset* pAsset);

    void setTarget(Target* target);

private:
    DoubleLinkedList<Unit*> m_Group;
};

#endif
