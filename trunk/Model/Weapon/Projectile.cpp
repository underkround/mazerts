#include "Projectile.h"
#include "Explosion.h"
#include "ExplosionCollection.h"

bool Projectile::update(const float deltaT)
{
    Explosion* pExplosion = NULL;

    switch(m_ConcreteType)
    {
        case BULLET:
            pExplosion = new Explosion(m_TargetX, m_TargetY, 1, (float)m_Damage);
            ExplosionCollection::addExplosion(pExplosion);
            return false;
            break;

        case SHELL:
            m_FlightTime += deltaT;
            //TODO: check when the projectile has reached its target

            return true;
            break;

        case BEAM:

            return false;
            break;
    }
    //Unknown type
    return false;
}