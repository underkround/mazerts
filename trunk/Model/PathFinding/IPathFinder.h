/**
 * IPathFinder
 *
 * Interface for time-slicing pathfinders
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __IPATHFINDER_H__
#define __IPATHFINDER_H__

//Forward declarations
class Unit;

class IPathFinder
{
public:
    /**
     * Default constructor
     */
    IPathFinder()
    {
        m_Cancelled = false;
    }

    /**
     * Destructor
     */
    virtual ~IPathFinder()
    {
    }

    /**
     * Advances the pathfinding by n steps and returns boolean to tell if the search
     * has ended or not
     * @param steps How many steps to advance
     * @return True, if the search has ended, otherwise false
     */
    virtual bool advance(short steps) = 0;

    /**
     * Cancels the search
     */
    inline void cancel()
    {

    }

    /**
     * Terrain to use for data in the search
     * @param pTerrain Terrain to use
     */
    /*inline void setTerrain(Terrain* pTerrain)
    {
        m_pTerrain = pTerrain;
    }*/

    /**
     * Sets the unit that will get the path once its' done (or told that there is
     * no path
     * @param pUnit Pointer to the Unit
     */
    inline void setUnit(Unit* pUnit)
    {
        m_pUnit = pUnit;
    }

protected:

    /**
     * Pointer to terrain to use in the search
     */
    //Terrain* m_pTerrain;

    /**
     * Pointer to Unit that is asking for the path
     */
    Unit* m_pUnit;

    /**
     * Starting tile-coordinates for the search
     */
    short m_StartX;
    short m_StartY;

    /**
     * Goal tile-coordinates for the search
     */
    short m_GoalX;
    short m_GoalY;

    /**
     * Tells if the search has been cancelled
     */
    bool m_Cancelled;
};

#endif //__IPATHFINDER_H__