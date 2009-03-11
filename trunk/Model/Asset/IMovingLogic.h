/**
 * Interface for unit's logic that modifies the position &
 * direction -vectors of the Unit, hence moving the unit.
 *
 * IMPLEMENTATION NOTES !!READ!!
 *
 * Concrete movinglogic (implementing this) is set to the unit in the
 * unitFactory, when the unit is created.
 *
 * The concrete logic can work with two basic ideas:
 *
 * A) Instance-spesific logic for each Unit, meaning one instance of
 * moving logic per Unit-instance. In this case, the hosting unit is
 * responsible of destroying the logic-instance when the hosting unit
 * is to be deleted.
 *
 * B) Shared logic for multiple Unit-instances, meaning one instance
 * of moving logic is used by many units. The logic could be singleton,
 * or just same normal instance given to many units in the UnitFactory.
 *
 * Each method takes pointer to the host-unit as parameter, in case
 * the logic is shared and hence needs to know which of many unit's
 * under it's control the call concerns.
 *
 * Both of these types are initialized and used the same way, for the
 * exception in the release (see notes for release() method)
 * @see release()
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __IMOVINGLOGIC_H__
#define __IMOVINGLOGIC_H__

class Unit;
class Target;

class IMovingLogic
{
public:

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
     * @param host  The Unit-instance in which this is being attached to
     */
    virtual void attach(Unit* host) = 0;

    /**
     * Normal per-frame update call from the hosting unit.
     * @param host      the unit which update-call this is
     * @param deltaT    time from last update
     */
    virtual void update(Unit* host, const float deltaT) = 0;

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
     * @param host  the unit in which this is attaced to, and from which this
     *              is being released from
     * @return      if TRUE, the unit will destroy this logic-instance after
     *              this call, if FALSE, the instance is not destroyed.
     */
    virtual bool release(Unit* host) = 0;

    /**
     * GetActions
     * @TODO: implement when the command-flags are more mature!
     *
     * The logic should "flag" given target for it's possible actions in it.
     * ..can the logic go in the target, is it passable?
     */
    //const int getActionFlags(Target* t);

    /**
     * @return current target to which to move, or NULL if none
     */
    virtual Target* getTarget() = 0;

    /**
     * Set new target towards which to move to, clear old.
     * The ownership of the target object will transfer to this class
     */
    virtual void setTarget(Target* target) = 0;

    /**
     * Clear current target
     */
    virtual void clearTarget() = 0;

};

#endif // __IMOVINGLOGIC_H__
