/**
 * DamageHandler
 *
 * DamageHandler-implementation, processes Damage-objects and returns 
 * the actual damage received to Unit owning the handler
 *
 * $Revision$
 * $Date$
 * $Id$
 */
 
#ifndef __DAMAGEHANDLER_H__
#define __DAMAGEHANDLER_H__

//For memcpy
#include <string.h>
#include "Damage.h"

class DamageHandler
{
public:
    
    /**
     * Default constructor 
     * Leaves all the resistance values to one
     */
    DamageHandler()
    {
        int i;
        for(i=0; i < Damage::DAMAGETYPE_AMOUNT; i++)
        {
            m_DamageFactor[i] = 1.0f;
        }
    }

/**
     * Constructor
     * @param damageFactors Array of floats containing value between 0.0f and 
     *        1.0f for each Damage::DamageType -enumeration
     * @param count Amount of values contained within the damageFactors
     */
    DamageHandler(const float damageFactors[], int count)
    {
        int i;
        for(i=0; i<Damage::DAMAGETYPE_AMOUNT; i++)
            m_DamageFactor[i] = 1.0f;
        setResistance(damageFactors, count);
    }

    /**
     * Destructor
     */
    virtual ~DamageHandler()
    {
    }

    /**
     * Sets resistance value agains given damage type
     * @param type damagetype index
     * @param percentage float containing value between 0.0f and 1.0f
     *        0.0f means 100% resistance, 1.0f passes full damage
     */
    inline void setResistance(Damage::DamageType type, float percentage)
    {
        m_DamageFactor[type] = percentage;
    }

    /**
     * Sets resistance values from the array
     * @param damageFactors Array of floats containing value between 0.0f and 
     *        1.0f for each Damage::DamageType -enumeration
     * @param count Amount of values contained within the damageFactors
     */
    inline void setResistance(const float damageFactors[], int count)
    {
        // sanity check
        if(count > Damage::DAMAGETYPE_AMOUNT)
        {
             count = Damage::DAMAGETYPE_AMOUNT;
        }
        ::memcpy(m_DamageFactor, damageFactors, count * sizeof(float));
    }

    inline const float* getResistance()
    {
        return m_DamageFactor;
    }

    /**
     * Handles the Damage-object and alters damage values based on resistances
     * @param pDamage Pointer to Damage-object to handle
     */
    inline void handleDamage(Damage* pDamage)
    {
        int totalDamage = 0;
        for(int i = 0; i < Damage::DAMAGETYPE_AMOUNT; i++)
        {
            pDamage->setDamage(i, (int) (pDamage->getDamage(i) * m_DamageFactor[i]) );
        }       
    }

private:
    /**
     * Array of damage factors, size is determined by constant in Damage.h
     */
    float m_DamageFactor[Damage::DAMAGETYPE_AMOUNT];
};

#endif // endif __DAMAGEHANDLER_H__
