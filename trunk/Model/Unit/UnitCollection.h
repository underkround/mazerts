/**
 * UnitCollection is a singleton that keeps track of the units int the game.
 *
 * Note: this could also be static class, but.. would there be any benefits
 * from it?
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __UNITCOLLECTION_H__
#define __UNITCOLLECTION_H__

//#include "IUpdatable.h"

class Unit;

class UnitCollection
{
public:

    struct UNITNODE {
        Unit*       unit;
        UNITNODE*   next;
        UNITNODE*   prev;
    };

    static inline UnitCollection* getInstance()
    {
        static UnitCollection instance;
        return &instance;
    }

// ===== ACCESSING UNITS

    Unit* getUnitAt(unsigned short x, unsigned short y);

// ===== MAINTAINANCE

    void registerUnit(Unit* unit);

    bool releaseUnit(Unit* unit);

    /**
     * Update call that iterates through all units in the game
     * and call's their update methods.
     */
    char updateAll(const float deltaT);

    /**
     * Delete all units in this container
     */
    void releaseAll(void);

    /**
     * Unit calls this if it's position is changed in the map
     */
    void handlePositionChanged(Unit* unit, unsigned short oldPosX, unsigned short oldPosY);

// ===== OBSERVERS

    /**
     * Needed notifications:
     *  unitPositionChanged(Unit u)
     *  unitReleased(Unit u)
     *  unitCreated(Unit u)
     */
//    void addUnitChangeListener(UnitChangeListener* listener);

//    void removeUnitChangeListener(UnitChangeListener* listener);

private:

    UnitCollection();
    ~UnitCollection();

    UNITNODE*           m_Head;
    UNITNODE*           m_Tail;

};

#endif // __UNITCOLLECTION_H__
