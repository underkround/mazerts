/**
 *
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __ARMOR_H__
#define __ARMOR_H__

#include "../Weapon/Damage.h"

class IAsset;

class Armor
{
public:

    /**
     * @param host  IAsset that this armor is for - tries to load the
     *              filter values from host's def
     */
    Armor(IAsset* host);

    /**
     * @param damageFactors Array of floats containing value between 0.0f and 
     *        1.0f for each Damage::DamageType -enumeration
     * @param count Amount of values contained within the damageFactors
     */
    Armor(IAsset* host, const float damageFactors[], int count);

    ~Armor()
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

    /**
     * Get the dynamic array holding the factor values for damage
     * handling
     */
    inline const float* getResistance()
    {
        return m_DamageFactor;
    }

    /**
     * Modify the given Damage-object by multiplying with our factor-values
     * per damage type.
     * @param pDamage Pointer to Damage-object to modify
     */
    inline void filterDamage(Damage* pDamage)
    {
        for(int i = 0; i < Damage::DAMAGETYPE_AMOUNT; i++)
        {
            pDamage->setDamage(i, (pDamage->getDamage(i) * m_DamageFactor[i]) );
        }
    }

private:

    // factorial values for multiplying damage values by type
    float           m_DamageFactor[Damage::DAMAGETYPE_AMOUNT];

    IAsset*         m_pHost;

};

#endif // __ARMOR_H__
