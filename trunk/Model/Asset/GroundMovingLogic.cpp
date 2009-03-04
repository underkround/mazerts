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

    m_CurrentSpeed = 0.0f;
    m_TargetDir.x = 0.0f;
    m_TargetDir.y = 0.0f;
    m_TargetDir.z = 0.0f;
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
    case FOLLOWPATH:
        {
            followPath();
            break;
        }
    }

    move(deltaT);
}

void GroundMovingLogic::idle(const float deltaT)
{
    //TODO: Maybe turning around or something while idling?
    //TODO: Replace with reading targets from commands, after they are implemented

    //DEBUG/TESTING: make the unit turn towards map center when idling
    static bool entered = true;
    if(entered)
    {
        m_TargetDir.x = (Terrain::getInstance()->getSize() >> 1) - m_pUnit->getPosition()->x;;
        m_TargetDir.y = (Terrain::getInstance()->getSize() >> 1) - m_pUnit->getPosition()->y;;
        m_TargetDir.z = 0;
        m_TargetDir.normalize();
        entered = false;
    }


    //DEBUG/TESTING: Wait some time before asking a new path
    static float counter = 0.0f;    
    counter += deltaT;
    if(counter > (1.0f + (float(rand() % 10))))
    {    
        m_State = ASKPATH;
        counter = 0.0f;
        entered = true;
    }
        
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
    else
    {
        m_State = IDLE;
    }
}

void GroundMovingLogic::waitPath()
{
    if(m_pAgent->getState() == IPathFinder::FOUND)
    {
        m_State = FOLLOWPATH;
        m_pPathNode = m_pAgent->getPathData();

        if(m_pPathNode == NULL)
        {
            m_State = IDLE;
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

void GroundMovingLogic::followPath()
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
        m_TargetDir.x = m_pPathNode->x - pos->x;
        m_TargetDir.y = m_pPathNode->y - pos->y;
        m_TargetDir.z = 0.0f;
        m_TargetDir.normalize();
    }
        
}

void GroundMovingLogic::move(float deltaTime)
{
    Vector3* dir = m_pUnit->getDirection();
    Vector3* pos = m_pUnit->getPosition();

    //TODO: Maximum turning speed as radians per second to unit TYPE data
    static const float maxTurnSpeed = 1.0f * 3.1415f;

    //TODO: Maximum moving speed as units (grid squares) per second to unit TYPE data
    static const float maxMoveSpeed = 6.0f;

    //Static deceleration, due to friction etc.
    m_CurrentSpeed *= (0.99f - (0.99f * deltaTime));

    //Target direction
    float targetAngle = atan2(m_TargetDir.y, m_TargetDir.x);

    //Turning speed        
    float turnSpeed = maxTurnSpeed * deltaTime;

    //Difference between target and current angle
    float currentAngle = atan2(dir->y, dir->x);
    float turn = currentAngle - targetAngle;
    
    if(fabs(turn) > 3.14159265f)
    {
        turn = targetAngle - currentAngle;    
    }
    
    //If current direction differs more than 35 degrees, slow down
    if(fabs(turn) > 0.61f)
    {
        //Braking factor to data?
        m_CurrentSpeed *= (0.95f - (0.95f * deltaTime));
    }


    if(turnSpeed > fabs(turn))
    {
        turnSpeed = -turn;
    }
    if(turn < -0.01f)
    {
        dir->x = cos(turnSpeed) * dir->x - sin(turnSpeed) * dir->y;
        dir->y = cos(turnSpeed) * dir->y + sin(turnSpeed) * dir->x;
    }
    else if(turn > 0.01f)
    {
        dir->x = cos(-turnSpeed) * dir->x - sin(-turnSpeed) * dir->y;
        dir->y = cos(-turnSpeed) * dir->y + sin(-turnSpeed) * dir->x;
    }
    else
    {
        //Heading (pretty much) toward correct direction, hit the pedal to the metal
        //TODO: Acceleration from data?
        m_CurrentSpeed += 1.0f * deltaTime;
        
        if(m_CurrentSpeed > maxMoveSpeed)
        {
            m_CurrentSpeed = maxMoveSpeed;
        }
    }

    //Move the unit towards current direction by current speed
    pos->x += m_CurrentSpeed * dir->x;
    pos->y += m_CurrentSpeed * dir->y;

    if(pos->x < 0 || pos->y < 0)
    {
        int k = 1;
    }

}


bool GroundMovingLogic::release(Unit* pUnit)
{
    return true;
}
