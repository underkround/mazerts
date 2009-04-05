/**
 * Implementation of general weapon
 *
 */

#include "Weapon.h"
#include "Projectile.h"
#include "../Common/DoubleLinkedList.h"
#include "../Asset/IAsset.h"
#include "../Asset/Unit.h"
#include "../Command/Target.h"


const float Weapon::TARGET_REJECTIONTIME = 4.0f;

void Weapon::update(const float deltaT)
{

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


    //NOTE: Target direction is in relation to the UNIT, not world, and 0 radians = straight forwards
    //      increases counterclockwise, value is between -PI...PI
    Vector3* dir = m_pHost->getDirection();

    //Used to control firing
    bool pointingInRightDirection = false;
    
    //If current target is not asset and does not contain forcing flag, delete it
    if(m_pTarget)
    {
        // remove the target, if:
        if(
            // target is not asset, and forceattack flag is not set
               ( m_pTarget->getTargetType() != Target::ASSET && !m_pTarget->isFlag(Target::TGTFLAG_FORCEATTACK) )
            // if target relection time exceeded
            || ( m_TargetRejectionTimer > TARGET_REJECTIONTIME )
            // if target is not an enemy
            || ( m_pTarget->getTargetType() == Target::ASSET && !m_pHost->getOwner()->isEnemy(m_pTarget->getTargetAsset()->getOwner()) )
        ) {
            delete m_pTarget;
            m_pTarget = NULL;
        }
    }


    //If there is no target, ask radar component for any
    if(!m_pTarget)
    {
        DoubleLinkedList<IAsset*>* list = m_pHost->getRadar()->getVisibleEnemyAssets();

        ListNode<IAsset*>* pNode = list->headNode();
        
        int ownX = m_pHost->getCenterGridX();
        int ownY = m_pHost->getCenterGridY();
        int shortestDist = 10000;
        IAsset* currentNearest = NULL;
        bool outOfRange = true;

        while(pNode)
        {
            int distance = abs(pNode->item->getCenterGridX() - ownX) + abs(pNode->item->getCenterGridY() - ownY);
            if(distance < shortestDist)
            {
                shortestDist = distance;
                currentNearest = pNode->item;

                if(distance < m_Def.range)
                {
                    outOfRange = false;
                }
            }
            pNode = pNode->next;
        }

        if(currentNearest != NULL)
        {
            m_pTarget = new Target(currentNearest);
            m_TargetRejectionTimer = 0;

            //If target is out of range, and there is no active moving
            //command, move closer to target (if m_pHost is an unit)
            if(outOfRange && m_pHost->getAssetType() == IAsset::UNIT)
            {
                Unit* pUnit = (Unit*)m_pHost;
                if(pUnit->getMovingLogic()->getTarget() == NULL)
                {
                    Target* pTarget = new Target(currentNearest);
                    pTarget->setRange(m_Def.range - 2.0f);
                    pUnit->getMovingLogic()->addTarget(pTarget);
                }
            }            
        }
    }

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

        //If the weapon fires SHELLS, turn the barrel
        if(m_Def.projectileConcreteType == Projectile::SHELL)
        {
            //Barrel pitching is just a graphical effect and has no effect on the actual firing
            //so just turn it gradually towards correct pitch
            float targetPitch = 1.0f - (targetDistSq / (m_Def.range * m_Def.range));

            if(targetPitch > 0.9f)
            {
                targetPitch = ((3.141592653589793238462f / 2.0f) * 0.9f);
            }
            else if(targetPitch < 0)
            {
                targetPitch = 0;
            }
            else
            {
                targetPitch *= (3.141592653589793238462f / 2.0f);
            }
            
            if(m_BarrelPitch < targetPitch)
            {
                m_BarrelPitch += 3.0f * deltaT;
            }
            else
            {
                m_BarrelPitch -= 3.0f * deltaT;
            }

            if(fabs(m_BarrelPitch - targetPitch) < 0.2f)
            {
                m_BarrelPitch = targetPitch;
            }
        }

        //Shooting only enemy assets or forced targets
        if((m_pTarget->getTargetType() == Target::ASSET && m_pTarget->getTargetAsset()->getOwner() != m_pHost->getOwner()) 
            || m_pTarget->isFlag(Target::TGTFLAG_FORCEATTACK))
        {
            //Ready to fire?
            if(m_Ammo > 0 && m_ROFTimer == 0)
            {
                //float targetDist = sqrt(targetDistSq);
                //If target is in range
                if(targetDistSq < (m_Def.range * m_Def.range))
                {
                    //...and turret points in right direction, fire
                    if(pointingInRightDirection)
                    {
                        fire();
                    }
                }
                else
                {
                    m_TargetRejectionTimer += deltaT;
                }
            }
        }

    }
}


bool Weapon::release()
{
    return true;
}
