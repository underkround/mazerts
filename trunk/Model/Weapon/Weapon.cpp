/**
 * Implementation of general weapon
 *
 */

#include "Weapon.h"

void Weapon::update(const float deltaT)
{
    //NOTE: Target direction is in relation to the UNIT, not world, and 0 radians = straight forwards
    //      increases counterclockwise, value is between -PI...PI

    //TODO: Testing, randomly selects new target direction, remove when not needed
    static float counter = 0;
    counter += deltaT;
    if(counter > 4.0f)
    {
        counter = 0;
        float x = ((rand() % 100) - 50)/50.0f;
        float y = ((rand() % 100) - 50)/50.0f;
        m_TargetDirection.x = x;
        m_TargetDirection.y = y;
        m_TargetDirection.normalize();
    }
    //End of testing

    //Threshold-value, below this no turning takes place
    static const float THRESHOLD = 0.001f;
    
    //Turn towards target direction
    if(fabs(m_TargetDirection.x - m_Direction.x) > THRESHOLD || fabs(m_TargetDirection.y - m_Direction.y) > THRESHOLD)
    {
        float targetAngle = atan2(m_TargetDirection.y, m_TargetDirection.x);
        float currentAngle = atan2(m_Direction.y, m_Direction.x);
        float turn = targetAngle - currentAngle;
        
        //Select "shorter route"
        if(fabs(turn) > 3.141592653589793238462f)
        {
            turn = -turn;
        }

        //TODO: Use m_Def.turnSpeed (when it actually has the value) instead of 2.0f
        float turnSpeed = 2.0f * deltaT;
        
#define sgn(a) ((a > 0) ? 1 : (a < 0) ? -1 : 0)

        //0.01 radians = 0.57 degrees
        if(fabs(turn) > 0.01f)
        {
            if(fabs(turn) > turnSpeed)
            {
                turn = sgn(turn) * turnSpeed;
            }
            m_Direction.x = cos(currentAngle + turn);
            m_Direction.y = sin(currentAngle + turn);
        }
        else
        {
            m_Direction.x = m_TargetDirection.x;
            m_Direction.y = m_TargetDirection.y;
        }
                
    }
}

bool Weapon::release()
{
    return true;
}
