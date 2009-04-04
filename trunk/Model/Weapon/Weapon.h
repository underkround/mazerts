/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "IWeapon.h"
#include "../Defs/Defs.h"

class Weapon : public IWeapon
{
public:

    /**
     * If target stays out of range this many seconds, the unit will delete the target
     * and check the radar for nearer targets
     */
    static const float TARGET_REJECTIONTIME;

    Weapon(WeaponDef& def) : IWeapon(DEFAULT, def)
    {
        m_TargetRejectionTimer = 0.0f;
    }

    virtual ~Weapon()
    {
    }

    /**
     * Normal per-frame update call from the hosting unit.
     * @param deltaT    time from last update
     */
    virtual void update(const float deltaT);

    /**
     * Return true, if the concrete weapon is spesific to hosting asset
     * (one weapon-instance per asset-instance)
     * When this returns true, the host will destroy this instance when
     * returned.
     * If the same weapon-instance is used with multiple asset-instances,
     * this should return false, and the host will not destroy this
     * weapon instance.
     */
    virtual bool release();
    
private:
    
    /**
     * Timer used to reject targets that stay out of range for longer than
     * TARGET_REJECTIONTIME seconds
     */
    float m_TargetRejectionTimer;

};

#endif // __WEAPON_H__
