/**
 * IProjectile
 *
 * Projectile is the object that is launched by weapon, and travels to it's
 * target casting damage or creating explosioin-object to do the casting.
 *
 * $Revision:$
 * $Date:$
 * $Id:$
 */

#ifndef __IPROJECTILE_H__
#define __IPROJECTILE_H__

#include "../Defs/Defs.h"

//Forward declaration
class IWeapon;

class IProjectile
{
public:

    /**
     * Concrete class types are declared here.
     * When adding new concrete implementing class, define enum for it here.
     * These need to be bit-values, so increase the shift value with 1
     * when adding.
     * Remember to keep the END -type last.
     */
    enum Type
    {
        BULLET      = 1 << 1,
        SHELL       = 1 << 2,
        BEAM        = 1 << 3,
        TYPE_END    = 1 << 4    // remember to advance the shift when adding new concrete type
    };

    IProjectile(WeaponDef& def, unsigned short targetX, unsigned short targetY, IWeapon* launcher) 
        : m_ConcreteType((IProjectile::Type)def.projectileConcreteType), m_pHost(launcher)
    {
        m_TargetX = targetX;
        m_TargetY = targetY;
        m_Damage = def.damage;
        m_FlightTime = 0;
    }

    virtual ~IProjectile()
    {
    }

    /**
     * Normal per-frame update call from the hosting unit.
     * @param deltaT    time from last update
     * @return True if the projectile is still alive, otherwise false
     */
    virtual bool update(const float deltaT);

protected:

    IWeapon*        m_pHost; // the weapon from which this projectile was launched

    const Type      m_ConcreteType; // the type of the concrete class

    //Target location
    unsigned short m_TargetX;
    unsigned short m_TargetY;

    //Damage
    int m_Damage;

    //Flight time for Shell-projectiles, used to advance the shell along the flightPath
    float m_FlightTime;
};

#endif // __IPROJECTILE_H__
