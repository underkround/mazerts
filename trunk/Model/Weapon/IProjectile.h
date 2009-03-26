/**
 * IProjectile
 *
 * Projectile is the object that is launched by weapon, and travels to it's
 * target casting damage or creating explosioin-object to do the casting.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __IPROJECTILE_H__
#define __IPROJECTILE_H__

#include "../Defs/Defs.h"

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

    IProjectile(Type concreteType, IWeapon* launcher) : m_ConcreteType(concreteType), m_pHost(launcher)
    {
    }

    virtual ~IProjectile()
    {
    }

    /**
     * Normal per-frame update call from the hosting unit.
     * @param deltaT    time from last update
     */
    virtual void update(const float deltaT) = 0;

protected:

    IWeapon*        m_pHost; // the weapon from which this projectile was launched

    const Type      m_ConcreteType; // the type of the concrete class

};

#endif // __IPROJECTILE_H__
