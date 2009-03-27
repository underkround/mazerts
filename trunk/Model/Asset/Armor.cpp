#include "Armor.h"

#include "IAsset.h"

Armor::Armor(IAsset* host)
{
    m_pHost = host;
    // TODO: load values from host's def-struct
    for(int i=0; i < Damage::DAMAGETYPE_AMOUNT; i++)
    {
        m_DamageFactor[i] = 1.0f;
    }
}


Armor::Armor(IAsset* host, const float damageFactors[], int count)
{
    m_pHost = host;
    for(int i=0; i<Damage::DAMAGETYPE_AMOUNT; i++)
        m_DamageFactor[i] = 1.0f;
    setResistance(damageFactors, count);
}
