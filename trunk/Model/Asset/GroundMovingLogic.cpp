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
    if( fabs(pos->x  - m_pPathNode->x) < 0.5f && fabs(pos->y - m_pPathNode->y) < 0.5f)
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
        dir->x =  m_pPathNode->x - pos->x;
        dir->y =  m_pPathNode->y - pos->y;
        dir->z = 0.0f;
        
        //TODO: Speed from unit-data
        float currentSpeed = 5.0f * deltaTime;

        //TODO: better moving and turning
        //TODO: movecost affects movespeed?
        if(dir->x > 0.0f)
        {
            if(currentSpeed < dir->x)
            {
                pos->x += currentSpeed;
            }
            else
            {
                pos->x += dir->x;
            }
        }
        else if(dir->x < 0.0f)
        {
            if(currentSpeed > dir->x)
            {
                pos->x -= currentSpeed;
            }
            else
            {
                pos->x -= dir->x;
            }             
        }
        
        if(dir->y > 0.0f)
        {
            if(currentSpeed < dir->y)
            {
                pos->y += currentSpeed;
            }
            else
            {
                pos->y += dir->y;
            }
        }
        else if(dir->y < 0.0f)
        {
            if(currentSpeed > dir->y)
            {
                pos->y -= currentSpeed;
            }
            else
            {
                pos->y -= dir->y;
            }             
        }
        
    }
}

bool GroundMovingLogic::release(Unit* pUnit)
{
    return true;
}
