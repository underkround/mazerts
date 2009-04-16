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
#include "../Common/HeapTree.h"

//Forward declarations
class Unit;
class Target;
class PathAgent;
class Building;

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
        RES_IDLE,           //Resources is doing nothing (automatically scans for nearby resources)
        RES_WAITCANPATH,    //Resourcer is waiting for pathing confirmation
        RES_MOVING,         //Resourcer is between locations
        RES_LOADING,        //Resourcer is loading in mine
        RES_UNLOADING       //Resourcer is unloading at resource yard
    };

    /** 
     * Constructor
     * @param pUnit Pointer to owning unit
     * @param def ResourceDef to use
     */
    Resourcer(Unit* pUnit, ResourcerDef& def) : m_Def(def)
    {
        m_pUnit = pUnit;

        m_pAgent = NULL;
        m_pTargetList = new CHeapTree<Building*, int>();
        m_pTarget = NULL;

        m_State = RES_IDLE;
        m_Ore = 0;
        m_LoadTimer = 0;
    }

    virtual ~Resourcer()
    {
        release();
    }

    /**
     * Updates the resourcer
     * @param deltaT Time passed in seconds
     */
    virtual void update(float deltaT);

    /**
     * Releases resources allocated by this resourcer
     */
    virtual void release();

protected:
    
    /**
     * Updates the idle state
     */
    void idle();

    /**
     * Searches viable targets for closest with existing path
     */
    void waitCanPath();

    /**
     * Checks when loading/unloading area is reached
     */
    void moving();

    /** 
     * Loads the ore
     * @param deltaT Time passed since last call
     */
    void loading(const float deltaT);

    /**
     * Unloads the ore
     * @param deltaT Time passed since last call
     */
    void unloading(const float deltaT);


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

    //PathAgent-pointer used in pathing checks
    PathAgent* m_pAgent;

    //Binary heap of viable targets, sorted by distance
    CHeapTree<Building*, int>* m_pTargetList;
};

#endif //__RESOURCER_H__
