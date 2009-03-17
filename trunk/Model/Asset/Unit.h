/**
 * !!NOTE!!
 * Read the documentation of the IAsset-class!
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __UNIT_H__
#define __UNIT_H__

#include "IAsset.h"
#include "IMovingLogic.h"
#include "../Command/Target.h"

class Unit : public IAsset
{
public:

    Unit(int unitType = 0);
    virtual ~Unit();

// ===== Initialization

    inline void setMovingLogic(IMovingLogic* movingLogic)
    {
        // if old logic exists, release it first
        if(m_pMovingLogic)
            releaseMovingLogic();
        m_pMovingLogic = movingLogic;
        // tell the logic that it is now in control of the moving of
        // this unit
        m_pMovingLogic->attach(this);
    }

    /**
     * @return the moving logic of this unit, or NULL if none attached
     */
    inline IMovingLogic* getMovingLogic() { return m_pMovingLogic; }

    /**
     * When this get's called, the unit is registered to the collection
     * and the ownership of the unit is transferred to the collection.
     * After this, you can safely lost the pointer to the unit.
     * 
     */
    virtual void create();

    /**
     * Only AssetCollection should call this!
     * Units are deletet via the collection
     */
    virtual void release();

// =====

    /**
     * Normal update call, updates the unit's components
     */
    virtual char update(const float deltaT);

// ===== Targets

    bool hasTarget();

    void addTarget(Target* target);

    void setTarget(Target* target);

    Target* getCurrentTarget();

    void clearCurrentTarget();

    void clearAllTargets();

    DoubleLinkedList<Target*> getTargetQueue();

private:

    /**
     * Release the movinglogic, and delete it if the implementation
     * requests it.
     * @see IMovingLogic::release()
     */
    void releaseMovingLogic();

// ===== MEMBERS

    // the type-flag of this unit, used by resource-loading etc
    int                         m_UnitType;

    // the logic responsible of moving this unit, altering the
    // position and direction -vectors of this unit
    IMovingLogic*               m_pMovingLogic;

    // the weapon & radar are in the IAsset

    // flag for weather this unit is created to prevent registration
    // to the collection multiple times
    bool                        m_Created;

    // Targets queued for this unit
    DoubleLinkedList<Target*>   m_TargetQueue;

};

#endif // __UNIT_H__
