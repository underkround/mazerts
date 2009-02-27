/**
 * IWeapon
 *
 * Weapon handles set of projectiles which it can shoot.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __IWEAPON_H__
#define __IWEAPON_H__

#include "../Common/Vector3.h"
#include "ProjectileDefinition.h"
//#include "../Asset/IAsset.h"
class IAsset;

class IWeapon
{
public:

    IWeapon();
    virtual ~IWeapon();

    virtual void create(IAsset* host, ProjectileDefinition def);

    virtual void release();

    /**
     * Load capability to shoot projectiles of given definition
     */
//    void load(ProjectileDefinition* def);

    /**
     * Remove capability to launch projectiles of given type
     */
//    void unload(int projectileId);

    inline const Vector3* const getDirection() { return &m_Direction; }

    inline const int getKillCount() { return m_KillCount; }

    inline const void addKill() { m_KillCount++; }

private:

    IAsset*     m_Host;
    Vector3     m_Direction;
    int         m_KillCount;

};

#endif // __IWEAPON_H__
