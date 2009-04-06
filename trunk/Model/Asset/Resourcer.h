/**
 * Resourcer-component takes care of ore transporting and telling the unit where to go
 *
 * $Revision: 281 $
 * $Date: 2009-04-03 20:16:06 +0300 (pe, 03 huhti 2009) $
 * $Id: Unit.h 281 2009-04-03 17:16:06Z zem $
 */

#ifndef __RESOURCER_H__
#define __RESOURCER_H__

#include "../Defs/Defs.h"

//Forward declarations
class Unit;
class Target;

class Resourcer
{
public:

    enum Type
    {
        DEFAULT     = 1 << 0,   // the default Resourcer (This class)
        TYPE_END    = 1 << 1    // remember to advance the shift when adding new concrete type
    };

    enum ResourcerState
    {
        RES_IDLE,       //Resources is doing nothing (automatically scans for nearby resources)
        RES_MOVING,     //Resourcer is between locations
        RES_LOADING,    //Resourcer is loading in mine
        RES_UNLOADING   //Resourcer is unloading at refinery
    };

    /** 
     * Constructor
     * @param pUnit Pointer to owning unit
     * @param def ResourceDef to use
     */
    Resourcer(Unit* pUnit, ResourcerDef& def) : m_Def(def)
    {
        m_pUnit = pUnit;
        m_State = RES_IDLE;
        m_Ore = 0;
        m_LoadTimer = 0;
    }

    virtual ~Resourcer()
    {
    }

    /**
     * Updates the resourcer
     * @param deltaT Time passed in seconds
     */
    virtual void update(float deltaT);

protected:
    
    //Unit this resourcer belongs to
    Unit* m_pUnit;

    //Resourcer values (max capacity, load time...)
    const ResourcerDef& m_Def;

    //Current loading timer value
    float m_LoadTimer;

    //How much ore the unit is currently carrying
    int m_Ore;

    //Current state
    ResourcerState m_State;

    //Target, shared with moving logic so this object knows when to change state
    //(moving logic still owns the target, and takes care of destruction)
    Target* m_pTarget;
};

#endif //__RESOURCER_H__
