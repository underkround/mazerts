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
        IDLE,
        ASKPATH,
        WAITPATH,
        FOLLOWPATH
    };

    /**
     * Constructor
     */
    GroundMovingLogic();

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
     * Set new target towards which to move to, clear old.
     * The ownership of the target object will transfer to this class
     */
    virtual void setTarget(Target* target);

    /**
     * Clear current target
     */
    virtual void clearTarget();

private:

    /**
     * Current target to move to
     */
    Target* m_pTarget;

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
     * Called no matter what the unit state is, handles actual moving (as in changing coordinates)
     * @param deltaTime Frametime in seconds
     */
    void move(const float deltaTime);

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

};

#endif // __GROUNDMOVINGLOGIC_H__

