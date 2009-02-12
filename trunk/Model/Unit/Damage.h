/**
 * Damage
 *
 * Damage is a data class holding different damage types that can reduce unit
 * hitpoints.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __DAMAGE_H__
#define __DAMAGE_H__

#include <string.h>

class Damage 
{

public:
    /**
     * Number of total damagetypes
     */
    static const int DAMAGETYPE_AMOUNT = 3;

    /**
     * Enumeration for different damagetypes, used as indices in arrays 
     * (first type must have value 0)
     */    
    enum DamageType 
    {
        KINETIC = 0,
        EXPLOSIVE,
        ENERGY
    };

    /**
     * Constructor
     * Sets every type of damage to zero
     */
    Damage()
    {
        ::memset(m_Damage, 0, DAMAGETYPE_AMOUNT * sizeof(int));
    }

    /**
     * Get damage amout of given damage type
     * @param  type the type indicator for damage value to return
     * @return the damage value for type
     */
    int getDamage(const int type)
    {
        return m_Damage[type];
    }

    /**
     * Set damage for given damage type
     * @param type DamageType to set, see enumeration
     * @param value Damage-value
     */
    void setDamage(const int type, const int value)
    {
        m_Damage[type] = value;
    }

    /**
     * Return the total amount of damage, sum from all types
     */
    int getTotalDamage(void)
    {
        int i;
        int sum = 0;
        for(i=0; i<DAMAGETYPE_AMOUNT; i++)
            sum += m_Damage[i];
        return sum;
    }

private:

    /**
     * Array for damage-values per type (see DamageType-enumeration)
     */
    int m_Damage[DAMAGETYPE_AMOUNT];
};

#endif // __DAMAGE_H__
