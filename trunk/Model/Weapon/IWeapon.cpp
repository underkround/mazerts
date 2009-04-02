#include "IWeapon.h"
#include "../Command/Target.h"
#include "Projectile.h"

void IWeapon::setTarget(Target* pTarget)
{
    if(m_pTarget)
    {
        delete m_pTarget;
    }
    m_pTarget = pTarget;
}

void IWeapon::fire()
{
    Projectile* pProjectile = NULL;
    
    //Fix offsets for ASSET-targets
    if(m_pTarget->getTargetType() == Target::ASSET)
    {
        pProjectile = new Projectile(m_Def, m_pTarget->getTargetX() + (m_pTarget->getTargetAsset()->getWidth() >> 1),
                               m_pTarget->getTargetY() + (m_pTarget->getTargetAsset()->getHeight() >> 1), this);
    }
    else if(m_pTarget->isFlag(Target::TGTFLAG_FORCEATTACK))
    {
        pProjectile = new Projectile(m_Def, m_pTarget->getTargetX(), m_pTarget->getTargetY(), this);
    }
    else
    {
        delete m_pTarget;
        m_pTarget = NULL;
    }
    
    ProjectileCollection::addProjectile(pProjectile);

    //Callback for effects
    if(m_pFiringCallBack)
    {
        m_pFiringCallBack->callBack();
    }

    --m_Ammo;
    m_ROFTimer = m_Def.rof;
}