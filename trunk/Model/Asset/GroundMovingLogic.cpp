#include "GroundMovingLogic.h"
#include "../Terrain/Terrain.h"
#include "../Common/Vector3.h"
#include <stdlib.h>  //TODO: REMOVE WHEN COMMANDS ARE IMPLEMENTED

GroundMovingLogic::GroundMovingLogic()
{
    m_pUnit = NULL;
    m_pAgent = NULL;
    m_pPathNode = NULL;
    m_State = IDLE;
}

GroundMovingLogic::~GroundMovingLogic()
{
    if(m_pAgent)
    {
        delete m_pAgent;
        m_pAgent = NULL;
    }
}

void GroundMovingLogic::attach(Unit* pUnit)
{
    m_pUnit = pUnit;
}

void GroundMovingLogic::update(Unit* pUnit, const float deltaT)
{   
    switch(m_State)
    {
    case IDLE:
        {
            idle(deltaT);
            break;
        }
    case ASKPATH:
        {
            askPath();
            break;
        }
    case WAITPATH:
        {
            waitPath();
            break;
        }
    case MOVE:
        {
            move(deltaT);
            break;
        }
    }

}

void GroundMovingLogic::idle(const float deltaT)
{
    //TODO: Maybe turning around or something while idling?
    //TODO: Replace with reading targets from commands, after they are implemented
    m_State = ASKPATH;
}

void GroundMovingLogic::askPath()
{
    //Destroy possible old agent (the path data will be discarded along with it)
    if(m_pAgent)
    {
        m_pAgent->cancel();
        delete m_pAgent;
        m_pAgent = NULL;                
    }
    

    unsigned short terraSize = Terrain::getInstance()->getSize();

    //TODO: Replace with reading targets from commands, after they are implemented
    static unsigned short m_TargetX;
    static unsigned short m_TargetY;
    m_TargetX = rand() % terraSize;
    m_TargetY = rand() % terraSize;
    m_pAgent = PathFinderMaster::getInstance()->findPath((unsigned short)m_pUnit->getPosition()->x, (unsigned short)m_pUnit->getPosition()->y,
                                                          m_TargetX, m_TargetY,
                                                          m_pUnit->getWidth()); //TODO: NOTE: Remember to set the widht & height of unit to same value!
    
    //If no agent is found, there is no path (goal is same as start location or goal is unpassable)
    if(m_pAgent != NULL)
    {
        m_State = WAITPATH;
    }
}

void GroundMovingLogic::waitPath()
{
    if(m_pAgent->getState() == IPathFinder::FOUND)
    {
        m_State = MOVE;
        m_pPathNode = m_pAgent->getPathData();

        if(m_pPathNode == NULL)
        {
            m_State = ASKPATH;
        }
    }
    else if(m_pAgent->getState() != IPathFinder::NOT_FINISHED)
    {
        //Cancelled or no path, destroy agent and idle
        delete m_pAgent;
        m_pAgent = NULL;
        m_State = IDLE;
    }
}

void GroundMovingLogic::move(const float deltaTime)
{
    Vector3* dir = m_pUnit->getDirection();
    Vector3* pos = m_pUnit->getPosition();

    //If close enough to current, get next target square
    if( fabs(pos->x  - m_pPathNode->x) < m_pUnit->getWidth() && fabs(pos->y - m_pPathNode->y) < m_pUnit->getHeight())
    {
        m_pPathNode = m_pAgent->getNextPathNode();
    }

    //Did we finish the path?
    if(m_pPathNode == NULL)
    {
        m_State = IDLE;
    }
    else
    {
        Vector3 targetDir;
        targetDir.x = m_pPathNode->x - pos->x;
        targetDir.y = m_pPathNode->y - pos->y;
        targetDir.z = 0.0f;
        targetDir.normalize();
        
        //TODO: Maximum turning speed as radians per second to unit TYPE data
        static const float maxTurnSpeed = 1.0f * 3.1415f;
        
        //TODO: Maximum moving speed as units (grid squares) per second to unit TYPE data
        static const float maxMoveSpeed = 20.0f;

        //TODO: CURRENT moving speed to unit data (not TYPE data)
        static float currentSpeed = 0.0f;

        //Deceleration
        currentSpeed *= 0.999f;


        //Target direction
        float targetAngle = atan2(targetDir.y, targetDir.x);

        //Turning speed        
        float turnSpeed = maxTurnSpeed * deltaTime;

        //Difference between target and current angle
        float turn = targetAngle - atan2(dir->y, dir->x);
        
        
        //If current direction differs more than 35 degrees, slow down
        if(fabs(turn) > 0.61f)
        {
            //Braking factor to data?
            currentSpeed *= 0.95f;
        }


        if(turnSpeed > fabs(turn))
        {
            turnSpeed = -turn;
        }
        if(turn > 0.01f)
        {
            dir->x = cos(turnSpeed) * dir->x - sin(turnSpeed) * dir->y;
            dir->y = cos(turnSpeed) * dir->y + sin(turnSpeed) * dir->x;
        }
        else if(turn < -0.01f)
        {
            dir->x = cos(-turnSpeed) * dir->x - sin(-turnSpeed) * dir->y;
            dir->y = cos(-turnSpeed) * dir->y + sin(-turnSpeed) * dir->x;
        }
        else
        {
            //Heading (pretty much) toward correct direction, hit the pedal to the metal
            //TODO: Acceleration from data?
            currentSpeed += 0.5f * deltaTime;
            
            if(currentSpeed > maxMoveSpeed)
            {
                currentSpeed = maxMoveSpeed;
            }
        }

        //Move the unit towards current direction by current speed
        pos->x += currentSpeed * dir->x;
        pos->y += currentSpeed * dir->y;


        
    }
}

bool GroundMovingLogic::release(Unit* pUnit)
{
    return true;
}
