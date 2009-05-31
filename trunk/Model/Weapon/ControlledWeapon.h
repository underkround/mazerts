/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#pragma once

#include "IWeapon.h"
#include "../Defs/Defs.h"

class ControlledWeapon : public IWeapon
{
public:

    ControlledWeapon(WeaponDef& def) : IWeapon(DEFAULT, def)
    {
    }

    virtual ~ControlledWeapon()
    {
    }

    /**
     * Normal per-frame update call from the hosting unit.
     * @param deltaT    time from last update
     */
    virtual void update(const float deltaT);

    virtual bool release();

    virtual void setTarget(Target* pTarget);
    
private:
 
};