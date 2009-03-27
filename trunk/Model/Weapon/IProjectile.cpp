#include "IProjectile.h"

bool IProjectile::update(const float deltaT)
{
    switch(m_ConcreteType)
    {
        case BULLET:
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