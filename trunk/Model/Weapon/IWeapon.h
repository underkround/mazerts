/**
 * IWeapon
 *
 * Weapon handles set of projectiles which it can launch.
 *
 * Like MovingLogic, this also could be shared with many instances.
 * @see MovingLogic comments for more info.
 * This should be concerned when implementing the release() method
 * in the concrete weapon.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __IWEAPON_H__
#define __IWEAPON_H__

#include "../Common/Vector3.h"
//#include "../Asset/IAsset.h"
#include "../Defs/Defs.h"

class IAsset;

class IWeapon
{
public:

    IWeapon(ProjectileDef def) : m_Def(def)
    {
        m_Direction.x = 0;
        m_Direction.y = 1;
        m_Direction.z = 0;
        m_KillCount = 0;
        m_pHost = 0;
    }

    virtual ~IWeapon()
    {
    }

    /**
     * This gets called by the asset when the weapon is attached to
     * a asset.
     * DO NOT call this from outside!
     * @see IAsset::setWeapon(..)
     */
    inline virtual void attach(IAsset* host)
    {
        m_pHost = host;
    }

    /**
     * Normal per-frame update call from the hosting unit.
     * @param deltaT    time from last update
     */
    virtual void update(const float deltaT) = 0;

    /**
     * Return true, if the concrete weapon is spesific to hosting asset
     * (one weapon-instance per asset-instance)
     * When this returns true, the host will destroy this instance when
     * returned.
     * If the same weapon-instance is used with multiple asset-instances,
     * this should return false, and the host will not destroy this
     * weapon instance.
     */
    virtual bool release() = 0;

    /**
     * Get the direction in which the weapon is facing, relative to the
     * hosting asset (used by ui to render the weapon in right direction)
     */
    inline const Vector3* const getDirection() { return &m_Direction; }

    /**
     * Get the killcount of this weapon
     * @TODO: can be moved to the asset, if wanted ?
     */
    inline const int getKillCount() { return m_KillCount; }

    /**
     * If the projectile launched from this weapon kills it's target, it
     * should notify this weapon to advance it's kill count (used by AI)
     * by calling this.
     */
    virtual inline const void addKill() { m_KillCount++; }

    /**
     * @return  The def struct that defines what this weapon shoots
     */
    virtual inline ProjectileDef* getDef() { return &m_Def; }

private:

    IAsset*         m_pHost;
    Vector3         m_Direction;
    int             m_KillCount;

    ProjectileDef   m_Def;

};

#endif // __IWEAPON_H__
