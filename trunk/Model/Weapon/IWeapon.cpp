#include "IWeapon.h"
#include "../Command/Target.h"
#include "Projectile.h"
#include "ProjectileCollection.h"
#include "../Asset/Unit.h"

void IWeapon::setTarget(Target* pTarget)
{
    if(m_pTarget)
    {
        delete m_pTarget;
        m_pTarget = NULL;
    }

    if(pTarget->getTargetType() == Target::ASSET || pTarget->isFlag(Target::TGTFLAG_FORCEATTACK))
    {
        pTarget->setRange(m_Def.range - 2.0f);
        m_pTarget = pTarget;
        if(m_pHost->getAssetType() == IAsset::UNIT)
        {
            ((Unit*)m_pHost)->getMovingLogic()->addTarget(new Target(*pTarget));
        }
    }
    else
    {
        delete pTarget;
        pTarget = NULL;
    }
}

void IWeapon::fire()
{
    Projectile* pProjectile = NULL;
    
    //Fix offsets for ASSET-targets
    if(m_pTarget->getTargetType() == Target::ASSET)
    {
        pProjectile = new Projectile(m_Def, m_pTarget->getTargetAsset()->getCenterGridX(),
                               m_pTarget->getTargetAsset()->getCenterGridY(), this);
    }
//    else if(m_pTarget->isFlag(Target::TGTFLAG_FORCEATTACK))
    else
    {
        pProjectile = new Projectile(m_Def, m_pTarget->getTargetX(), m_pTarget->getTargetY(), this);
    }
    /*
    else
    {
        delete m_pTarget;
        m_pTarget = NULL;
    }
    */
    
    ProjectileCollection::addProjectile(pProjectile);

    //Callback for effects
    if(m_pFiringCallBack)
    {
        m_pFiringCallBack->callBack();
    }

    --m_Ammo;
    m_ROFTimer = m_Def.rof;
}