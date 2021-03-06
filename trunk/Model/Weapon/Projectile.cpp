#include "Projectile.h"
#include "Explosion.h"
#include "ExplosionCollection.h"

const float Projectile::SHELL_SPEED = 40.0f;
const float Projectile::ROCKET_SPEED = 20.0f;
const float Projectile::ICBM_SPEED = 10.0f;

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
        case ROCKET:
        case ICBM:
            m_FlightTime += deltaT;
            
            //If m_Alive was set to false on last frame, destroy it know
            //(1 frame delayed destruction to take care of UIProjectile first)
            if(!m_Alive)
            {
                return false;
            }

            if(m_FlightTime >= m_TargetTime)
            {
                pExplosion = new Explosion(m_TargetX, m_TargetY, m_DamageRange, (float)m_Damage);
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