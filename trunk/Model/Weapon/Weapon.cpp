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

    //Used to control firing
    bool pointingInRightDirection = false;

    if(m_pTarget)
    {
        Vector3* pos = m_pHost->getPosition();
        m_TargetDirection.x = m_pTarget->getTargetX() - pos->x;
        m_TargetDirection.y = m_pTarget->getTargetY() - pos->y;

        //Calculate squared distance before normalizing
        float targetDistSq = m_TargetDirection.x *  m_TargetDirection.x +  m_TargetDirection.y *  m_TargetDirection.y;

        m_TargetDirection.normalize();

        float unitAngle = atan2(dir->y, dir->x);
        float targetAngle = atan2(m_TargetDirection.y, m_TargetDirection.x) - unitAngle;
        float currentAngle = atan2(m_Direction.y, m_Direction.x);
        float turn = fmod(targetAngle - currentAngle, (2.0f * 3.141592653589793238462f) );
        
        //Select "shorter route"
        if(fabs(turn) > 3.141592653589793238462f)        
        {
            turn = -turn;
        }

        //Turnspeed from unit data
        float turnSpeed = m_Def.turnSpeed * deltaT;
        
#define sgn(a) ((a > 0) ? 1 : (a < 0) ? -1 : 0)
        
        if(fabs(turn) > 0.01f)
        {
            float turnAmount = 0.0f;

            if(fabs(turn) > turnSpeed)
            {
                turnAmount = sgn(turn) * turnSpeed;
            }
            else
            {
                turnAmount = turn;
            }
            m_Direction.x = cos(currentAngle + turnAmount);
            m_Direction.y = sin(currentAngle + turnAmount);
        }
        else
        {
            pointingInRightDirection = true;
        }

        //Reloading if necessary
        if(m_Ammo == 0)
        {
            m_ReloadTimer += deltaT;
            if(m_ReloadTimer > m_Def.reloadTime)
            {
                m_Ammo = m_Def.clipSize;
                m_ReloadTimer = 0;
            }
        }

        //Rate of fire
        if(m_ROFTimer > 0)
        {
            m_ROFTimer -= deltaT;
            if(m_ROFTimer < 0)
            {
                m_ROFTimer = 0;
            }
        }

        //Shooting only enemy assets or forced targets
        if((m_pTarget->getTargetType() == Target::ASSET && m_pTarget->getTargetAsset()->getOwner() != m_pHost->getOwner()) 
            || m_pTarget->isFlag(Target::TGTFLAG_FORCEATTACK))
        {
            //Ready to fire?
            if(m_Ammo > 0 && m_ROFTimer == 0)
            {
                float targetDist = sqrt(targetDistSq);
                //If target is in range
                if(targetDist < m_Def.range)
                {
                    //...and turret points in right direction, fire
                    if(pointingInRightDirection)
                    {
                        fire();
                    }
                }
            }
        }

    }
}


bool Weapon::release()
{
    return true;
}
