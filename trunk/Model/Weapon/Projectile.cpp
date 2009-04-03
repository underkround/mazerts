#include "Projectile.h"
#include "Explosion.h"
#include "ExplosionCollection.h"

const float Projectile::SHELL_SPEED = 20.0f;

bool Projectile::update(const float deltaT)
{
    Explosion* pExplosion = NULL;

    switch(m_ConcreteType)
    {
        case BULLET:
            pExplosion = new Explosion(m_TargetX, m_TargetY, 0, (float)m_Damage);
            ExplosionCollection::addExplosion(pExplosion);
            return false;
            break;

        case SHELL:
            m_FlightTime += deltaT;
            
            //If m_Alive was set to false on last frame, destroy it know
            //(1 frame delayed destruction to take care of UIProjectile first)
            if(!m_Alive)
            {
                return false;
            }

            if(m_FlightTime >= m_TargetTime)
            {
                //TODO: Explosion radius
                pExplosion = new Explosion(m_TargetX, m_TargetY, 1, (float)m_Damage);
                ExplosionCollection::addExplosion(pExplosion);
                m_Alive = false;
            }
            return true;
            break;

        case BEAM:

            return false;
            break;
    }
    //Unknown type
    return false;
}