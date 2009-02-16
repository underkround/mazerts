/**
 * IComponent
 *
 * Base class for component that implements the logic for the unit.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __ICOMPONENT_H__
#define __ICOMPONENT_H__

#include "IUpdatable.h"

class Unit;

class IComponent : public IUpdatable
{
public:

    /**
     * @see IUpdatable
     */
    virtual char update(const float deltaT) = 0;

    /**
     * Attach
     *
     * !NOTE! only the host unit calls this! This should not be called by
     * any other class. The component registering should be done via the
     * unit's addComponent -method.
     *
     * Attach this component to the given host unit.
     * The component will start to receive update calls from the host unit.
     */
    virtual void attach(Unit* host) = 0;

    /**
     * Detach
     *
     * This get's called when the host unit is going to be destroyed.
     *
     * If this returns TRUE, the unit will delete the component after the
     * call, and the resource release stuff should be executed here.
     *
     * If this returns FALSE, the component will not be destroyed.
     *
     * This functionality is provided in case the component is a
     * singleton, or used with multiple units and hence should not be
     * destoyed.
     *
     * @param host  the host unit that is being destroyed. If this
     * @return      TRUE, if this component can be destroyed immediately
     */
    inline virtual bool detach(Unit* host)
    {
        return true;
    }

};

#endif // __ICOMPONENT_H__
