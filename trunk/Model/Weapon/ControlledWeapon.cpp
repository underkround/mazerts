/**
 * Implementation of user controlled weapon
 *
 */

#include "ControlledWeapon.h"
#include "Projectile.h"
#include "../Common/DoubleLinkedList.h"
#include "../Asset/IAsset.h"
#include "../Asset/Unit.h"
#include "../Command/Target.h"

void ControlledWeapon::update(const float deltaT)
{   

    //Reloading if necessary
    if(m_Ammo == 0)
    {
        m_ReloadTimer += deltaT;
        if(m_ReloadTimer > m_Def.reloadTime)
        {
            m_Ammo = m_Def.clipSize;
            m_ReloadTimer = 0;
        }
    }

    //Rate of fire
    if(m_ROFTimer > 0)
    {
        m_ROFTimer -= deltaT;
        if(m_ROFTimer < 0)
        {
            m_ROFTimer = 0;
        }
    }

    if(m_pTarget)
    {
        fire();
        delete m_pTarget;
        m_pTarget = NULL;
    }

}


bool ControlledWeapon::release()
{
    return true;
}
