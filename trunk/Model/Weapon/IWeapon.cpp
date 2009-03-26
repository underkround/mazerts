#include "IWeapon.h"
#include "../Command/Target.h"

void IWeapon::setTarget(Target* pTarget)
{
    if(m_pTarget)
    {
        delete m_pTarget;
    }
    m_pTarget = pTarget;
}