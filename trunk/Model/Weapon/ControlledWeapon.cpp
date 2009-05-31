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

void ControlledWeapon::setTarget(Target* pTarget)
{
    if(m_pTarget)
    {
        delete m_pTarget;
        m_pTarget = NULL;
    }

    pTarget->setRange(m_Def.range - 2.0f);
    m_pTarget = pTarget;
    if(m_pHost->getAssetType() == IAsset::UNIT)
    {
        ((Unit*)m_pHost)->getMovingLogic()->addTarget(new Target(*pTarget));
    }
}