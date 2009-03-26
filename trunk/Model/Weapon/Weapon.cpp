/**
 * Implementation of general weapon
 *
 */

#include "Weapon.h"
#include "../Asset/IAsset.h"
#include "../Command/Target.h"

void Weapon::update(const float deltaT)
{
    //NOTE: Target direction is in relation to the UNIT, not world, and 0 radians = straight forwards
    //      increases counterclockwise, value is between -PI...PI
    Vector3* dir = m_pHost->getDirection();

    if(m_pTarget)
    {
        Vector3* pos = m_pHost->getPosition();
        m_TargetDirection.x = m_pTarget->getTargetX() - pos->x;
        m_TargetDirection.y = m_pTarget->getTargetY() - pos->y;
        m_TargetDirection.normalize();
    }

    //Threshold-value, below this no turning takes place
    static const float THRESHOLD = 0.001f;
    
    //Turn towards target direction
    if(fabs(m_TargetDirection.x - m_Direction.x) > THRESHOLD || fabs(m_TargetDirection.y - m_Direction.y) > THRESHOLD)
    {
        float unitAngle = atan2(dir->y, dir->x);
        float targetAngle = atan2(m_TargetDirection.y, m_TargetDirection.x);
        float currentAngle = atan2(m_Direction.y, m_Direction.x);
        float turn = fmod(targetAngle - currentAngle - unitAngle, (2.0f * 3.141592653589793238462f) );
        
        //Select "shorter route"
        if(fabs(turn) > 3.141592653589793238462f)        
        {
            turn = -turn;
        }

        //TODO: Use m_Def.turnSpeed (when it actually has the value) instead of 4.0f
        float turnSpeed = 1.0f * deltaT;
        
#define sgn(a) ((a > 0) ? 1 : (a < 0) ? -1 : 0)
        
        if(fabs(turn) > 0.0001f)
        {
            if(fabs(turn) > turnSpeed)
            {
                turn = sgn(turn) * turnSpeed;
            }
            m_Direction.x = cos(currentAngle + turn);
            m_Direction.y = sin(currentAngle + turn);
        }        
    }
}

bool Weapon::release()
{
    return true;
}
