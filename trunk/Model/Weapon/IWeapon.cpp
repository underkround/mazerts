#include "IWeapon.h"
#include "../Command/Target.h"
#include "IProjectile.h"

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
    IProjectile* pProjectile = new IProjectile(m_Def, m_pTarget->getTargetX(), m_pTarget->getTargetY(), this);
    ProjectileCollection::addProjectile(pProjectile);
}