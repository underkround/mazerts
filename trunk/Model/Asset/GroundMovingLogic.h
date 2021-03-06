/**
 *
 * Basic "default" moving logic for ground-based units
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __GROUNDMOVINGLOGIC_H__
#define __GROUNDMOVINGLOGIC_H__

#include "IMovingLogic.h"
#include "Unit.h"
#include "../PathFinding/PathAgent.h"
#include "../PathFinding/IPathFinder.h"
#include "../PathFinding/PathFinderMaster.h"
#include "../Command/Target.h"

class GroundMovingLogic : public IMovingLogic
{
public:
    /**
     * Enumeration of moving states
     */
    enum GroundMovingState
    {
        IDLE,           //Just sit around
        ASKPATH,        //Ask a new path from pathfinder-master
        WAITPATH,       //Wait for pathfinder to finish
        FOLLOWPATH,     //Follow current pathdata        
        JUSTMOVE        //Just move to target x,y
    };

    /**
     * Constructor
     */
    GroundMovingLogic(MovingDef& def);

    /**
     * Destructor
     */
    ~GroundMovingLogic();

    /**
     * NOTE: this is intented to be called only by unit, so do not
     * call this when attaching the movinglogic to the unit.
     * Use Unit::setMovingLogic(..) instead!
     *
     * When the movinglogic is set to the unit, the unit calls this
     * method, so call to this is indication to the logic that it is
     * now responsible of moving given unit (host), and starts to
     * receive update-calls from it.
     *
     * @see Unit::setMovingLogic()
     * @param pUnit  The pointer to Unit-instance in which this is being attached to
    */
    virtual void attach(Unit* pUnit);

    /**
     * Normal per-frame update, called from the hosting unit.
     * @param pUnit     the unit which update-call this is, IGNORED
     * @param deltaT    time from last update
     */
    virtual void update(Unit* pUnit, const float deltaT);

    /**
     * This gets called when the host-unit is to be destroyed.
     * Since the movinglogic is designed so that single instance can
     * be used with multiple units when needed, the return value of
     * release determines the ownership of the logic.
     *
     * If the concrete logic is unit-instance-specifig and it has no
     * use when the host is destroyed, this should return TRUE, and
     * the unit will delete this object when the method is returned.
     *
     * If this returns FALSE, the instance of this logic is left untouched
     * (will not be deleted). So if you use the same logic-instance with
     * multiple units, return FALSE and you are responsible of destroying
     * the logic-instance when it's done it's job (or, you could keep count
     * of attach() - release() -calls and return true in the last release()
     * when it's no longer attached to anything).
     *
     * @param pUnit  the unit in which this is attaced to, and from which this
     *              is being released from
     * @return      if TRUE, the unit will destroy this logic-instance after
     *              this call, if FALSE, the instance is not destroyed.
     */
    virtual bool release(Unit* pUnit);

// ===== Targets

    /**
     * @return current target to which to move, or NULL if none
     */
    virtual Target* getTarget();

    /**
     * Overrides the current target with given, pushing current as first of queue
     * The ownership of the target object will transfer to this class
     */
    virtual void priorityTarget(Target* target);

    /**
     * Pushes a new target to command queue
     * Note: The ownership of the target object will be transferred to this class
     * @param target Target-pointer to push into queue
     */
    virtual void addTarget(Target* target);

    /**
     * Clear command queue and current target
     */
    virtual void clearTargets();

private:

    /**
     * Current target to move to
     */
    Target* m_pTarget;

    /**
     * List of targets in queue
     */
    DoubleLinkedList<Target*> m_TargetList;

    /**
     * Idling when the unit has no orders
     * @param deltaTime Frametime in seconds
     */
    void idle(const float deltaTime);

    /**
     * Asks for a new path and moves into WAITPATH-state
     */
    void askPath();

    /**
     * Wait for path, poll agent until the state changes
     * Changes state to either MOVE (if path was found) or IDLE (no path or cancelled)
     */
    void waitPath();

    /**
     * Makes the unit follow the path, until path is walked through
     */
    void followPath();

    /** 
     * Make current unit steer aside from it's current movement direction
     * @param whichWay Which way to steer from current direction vector,
     *        0 for random(left/right), 1 for left, 2 for right (or the other way around)
     */
    void steerAway(char whichWay);

    /**
     * Checks if the square the unit is about to move in is available
     * also performs giving non-moving own units commands to move out of the way
     * @return true if squares are free, otherwise false
     */
    bool squareAvailability();

    /**
     * Called no matter what the unit state is, handles actual moving (as in changing coordinates)
     * @param deltaTime Frametime in seconds
     */
    void move(const float deltaTime);

    /** 
     * Destroys current target, or puts it in the end of the queue based on it's flags.
     * @param forceRelease  set to true if the current should be removed whatever
     *                      the flags say
     */
    void clearCurrentTarget(const bool forceRelease=false);

    /**
     * @return true, if the target has changed enough so we should get new path.
     */
    bool shouldWeRePath() const;

    /**
     * Unit this MovingLogic is owned by
     */
    Unit* m_pUnit;

    /**
     * Pointer to PathAgent mediating the data between PathFinderMaster and this object
     */
    PathAgent* m_pAgent;

    /**
     * State of moving
     */
    GroundMovingState m_State;

    /**
     * Next node in path, NEVER delete manually, deleting PathAgent removes all the nodes
     */
    IPathFinder::PathNode* m_pPathNode;

    /**
     * Target direction towards which the unit is turning
     */
    Vector3 m_TargetDir;

    /**
     * Current moving speed of the unit
     */
    float m_CurrentSpeed;

    /**
     * Temporary values used to determine if the target has reached since we reached it.
     */
    int m_CachedReachedTargetX;
    int m_CachedReachedTargetY;

    int m_CachedTargetOnLastPathX;
    int m_CachedTargetOnLastPathY;

    /** 
     * Half of unit width, used for offsets (unit "center")
     */
    float m_HalfSize;

    /** 
     * Counter used to cancel MAKEWAY-targets if the unit cannot move in certain number of seconds
     */
    float m_StuckCounter;

    /**
     * Treshold for how much the target can change before the path is recalculated.
     * Note that the check is in it's own method if you want to change the whole implementation.
     */
    int m_TargetChangeTreshold;

};

#endif // __GROUNDMOVINGLOGIC_H__

