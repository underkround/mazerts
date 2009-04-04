/**
 * Projectile
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
#include "../Asset/IAsset.h"
#include "IWeapon.h"

class Projectile
{
public:

    static const int FLYHEIGHT_FACTOR = 200;

    /**
     * Concrete class types are declared here.
     * When adding new concrete implementing class, define enum for it here.
     * These need to be bit-values, so increase the shift value with 1
     * when adding.
     * Remember to keep the END -type last.
     */
    enum Type
    {
        BULLET      = 1 << 0,
        SHELL       = 1 << 1,
        BEAM        = 1 << 2,
        TYPE_END    = 1 << 3    // remember to advance the shift when adding new concrete type
    };

    /**
     * Shell-projectile flight speed
     */
    static const float SHELL_SPEED;

    Projectile(WeaponDef& def, unsigned short targetX, unsigned short targetY, IWeapon* launcher) 
        : m_ConcreteType((Projectile::Type)def.projectileConcreteType), m_pHost(launcher)
    {
        m_TargetX = targetX;
        m_TargetY = targetY;
        m_Damage = def.damage;
        //TODO: Radius for explosion

        if(m_ConcreteType == SHELL)
        {
            m_FlightTime = 0;
            m_Alive = true;

            //Get distance to target
            float dx = (float)(m_pHost->getHost()->getCenterGridX() - targetX);
            dx *= dx;
            float dy = (float)(m_pHost->getHost()->getCenterGridY() - targetY);
            dy *= dy;
            float dist = sqrt(dx + dy);
            m_TargetTime = dist / SHELL_SPEED;

            m_FlyHeight = ((float)FLYHEIGHT_FACTOR) / dist;
        }
    }

    virtual ~Projectile()
    {
    }

    /**
     * Normal per-frame update call from the hosting unit.
     * @param deltaT    time from last update
     * @return True if the projectile is still alive, otherwise false
     */
    virtual bool update(const float deltaT);

    /**
     * Returns how far along a projectile is in its flightpath
     * @return float between 0...1
     */
    inline const float getFlightT() const { return m_FlightTime / m_TargetTime; }

    //Target & origin getters
    inline const unsigned short getTargetX() const { return m_TargetX; }
    inline const unsigned short getTargetY() const { return m_TargetY; }
    inline const unsigned short getOriginX() const { return m_pHost->getHost()->getCenterGridX(); }
    inline const unsigned short getOriginY() const { return m_pHost->getHost()->getCenterGridY(); }
    
    /**
     * Returns the weapon which fired this projectile
     */
    inline IWeapon* getHost() { return m_pHost; }
    
    /**
     * Returns the alive state, used to signal UIProjectile to destroy itself
     */
    inline const bool isAlive() const { return m_Alive; }

    /**
     * Get the concrete type of the projectile
     */
    inline const Type getConcreteType() const { return m_ConcreteType; }

    /** 
     * Get the highest altitude of flight path
     */
    inline const float getFlyHeight() const { return m_FlyHeight; }

protected:

    IWeapon*        m_pHost; // the weapon from which this projectile was launched

    const Type      m_ConcreteType; // the type of the concrete class

    /**
     * Target location
     */
    unsigned short m_TargetX;
    unsigned short m_TargetY;

    //Damage
    int m_Damage;

    /**
     * Flight time for Shell-projectiles, used to advance the shell along the flightPath
     */
    float m_FlightTime;

    /**
     * Time in seconds after which the Shell hits the target
     */
    float m_TargetTime;

    /**
     * Highest altitude the shell will gain in its flight
     */
    float m_FlyHeight;

    /**
     * Used to signal the ui-side object to destroy itself, set to false one frame before
     * this object is destroyed
     */
    bool m_Alive;
};

#endif // __IPROJECTILE_H__
