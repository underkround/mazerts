#include "GroundMovingLogic.h"
#include "../Terrain/Terrain.h"
#include "../Common/Vector3.h"
#include <stdlib.h>  //TODO: REMOVE WHEN COMMANDS ARE IMPLEMENTED

#ifndef PI
#define PI 3.141592653589793238462f
#endif

//THE MODEL WON'T COMPILE WITH THIS, COMMENT OUT WHEN NOT TESTING!
//#define PATH_UI_DEBUG
#ifdef PATH_UI_DEBUG
#include "../../ViewController/3DDebug/UI3DDebug.h"
#include "../../ViewController/Terrain/UITerrain.h"
#endif

GroundMovingLogic::GroundMovingLogic()
{
    m_pUnit = NULL;
    m_pAgent = NULL;
    m_pPathNode = NULL;
    m_State = IDLE;
    m_pTarget = NULL;

    m_CurrentSpeed = 0.0f;
    m_TargetDir.x = 0.0f;
    m_TargetDir.y = 0.0f;
    m_TargetDir.z = 0.0f;

    m_CachedReachedTargetX = -1;
    m_CachedReachedTargetY = -1;
}

GroundMovingLogic::~GroundMovingLogic()
{
    if(m_pAgent)
    {
        delete m_pAgent;
        m_pAgent = NULL;
    }
    clearTarget();
}

void GroundMovingLogic::attach(Unit* pUnit)
{
    m_pUnit = pUnit;
    m_TargetDir.x = m_pUnit->getDirection()->x;
    m_TargetDir.y = m_pUnit->getDirection()->y;
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
    //TODO: Replace with reading targets from commands, after they are implemented
    // ... or, commands in unit, unit dispatching target from current command to this

    // do we have a target?
    if(m_pTarget) {
        // is the target reached?
        if( m_CachedReachedTargetX != m_pTarget->getTargetX() ||
            m_CachedReachedTargetY != m_pTarget->getTargetY()
        )
        {
            // target has changed since we reached it
            m_CachedReachedTargetX = -1;
            m_CachedReachedTargetY = -1;
            m_State = ASKPATH;
        }
    }
/*
    //DEBUG/TESTING: Wait some time before asking a new path
    static float counter = 0.0f;    
    counter += deltaT;
    if(counter > 1.0f)
    {    
        m_State = ASKPATH;
        counter = 0.0f;
    }
*/

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

//    unsigned short terraSize = Terrain::getInstance()->getSize();
    //TODO: Replace with reading targets from commands, after they are implemented
//    static unsigned short m_TargetX;
//    static unsigned short m_TargetY;
//    m_TargetX = rand() % terraSize;
//    m_TargetY = rand() % terraSize;
    if(!m_pTarget)
    {
        m_State = IDLE;
        return;
    }
    unsigned short m_TargetX = m_pTarget->getTargetX();
    unsigned short m_TargetY = m_pTarget->getTargetY();

    // is the target reached?
/*
    if( m_pUnit->getGridX() >= m_TargetX && (m_pUnit->getGridX() + m_pUnit->getWidth()) <= m_TargetY &&
        m_pUnit->getGridY() >= m_TargetY && (m_pUnit->getGridY() + m_pUnit->getHeight()) <= m_TargetY)
    {
*/
    if( m_pUnit->getGridX() == m_TargetX && m_pUnit->getGridY() == m_TargetY ) {
        // if the target was static coordinates - not tracking asset,
        // we can remove it when it's reached
        if(m_pTarget->isStatic())
        {
            m_CachedReachedTargetX = -1;
            m_CachedReachedTargetY = -1;
            clearTarget();
        }
        // if the target is locked to asset, do not delete it
        else
        {
            // cache current position of the target that is reached for
            // determing later if the target has moved
            m_CachedReachedTargetX = m_pTarget->getTargetX();
            m_CachedReachedTargetY = m_pTarget->getTargetY();
            return;
        }
    }

    m_pAgent = PathFinderMaster::getInstance()->findPath(   (unsigned short)m_pUnit->getPosition()->x,
                                                            (unsigned short)m_pUnit->getPosition()->y,
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
        m_pPathNode = m_pAgent->getNextPathNode();

        if(m_pPathNode == NULL)
        {
            m_State = IDLE;
            return;
        }




#ifdef PATH_UI_DEBUG

        /*IPathFinder::PathNode* pNode = m_pAgent->getPathData();        
        float halfX = m_pUnit->getWidth() * 0.5f;
        float halfY = m_pUnit->getHeight() * 0.5f;
        float aliveTime = 5.0f;

        while(pNode)
        {
            float x = pNode->x + halfX;
            float y = pNode->y + halfY;
            UI3DDebug::addSphere(x, y, (float)UITerrain::getInstance()->calculateTriangleHeightAt(x, y), 0.5f, aliveTime);
            if(pNode->pChild != NULL)
            {
                float cx = pNode->pChild->x + halfX;
                float cy = pNode->pChild->y + halfY;
                UI3DDebug::addLine(x, y, (float)UITerrain::getInstance()->calculateTriangleHeightAt(x, y) - 0.3f,
                        cx, cy, (float)UITerrain::getInstance()->calculateTriangleHeightAt(cx, cy) - 0.3f, 0.2f, aliveTime);
            }
            aliveTime += 0.1f;
            pNode = pNode->pChild;
        }*/


        float halfX = m_pUnit->getWidth() * 0.5f;
        float halfY = m_pUnit->getHeight() * 0.5f;
        float x = m_pPathNode->x + halfX;
        float y = m_pPathNode->y + halfY;        
        if(m_pPathNode->pChild != NULL)
        {
            float cx = m_pPathNode->pChild->x + halfX;
            float cy = m_pPathNode->pChild->y + halfY;
            UI3DDebug::addLine(x, y, (float)UITerrain::getInstance()->calculateTriangleHeightAt(x, y) - 0.3f,
                    cx, cy, (float)UITerrain::getInstance()->calculateTriangleHeightAt(cx, cy) - 0.3f, 0.2f, 5.0f);            
        }
#endif


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
    //TODO: fixes for offsets (unit halfwidth)
    float halfWidth = m_pUnit->getWidth() * 0.1f;
    float halfHeight = m_pUnit->getHeight() * 0.1f;
    if( fabs(m_pPathNode->x - pos->x) < halfWidth && fabs(m_pPathNode->y - pos->y) < halfHeight)
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
#ifdef PATH_UI_DEBUG
        float halfX = m_pUnit->getWidth() * 0.5f;
        float halfY = m_pUnit->getHeight() * 0.5f;
        float x = m_pPathNode->x + halfX;
        float y = m_pPathNode->y + halfY;
        if(m_pPathNode->pChild != NULL)
        {
            float cx = m_pPathNode->pChild->x + halfX;
            float cy = m_pPathNode->pChild->y + halfY;
            UI3DDebug::addLine(x, y, (float)UITerrain::getInstance()->calculateTriangleHeightAt(x, y) - 0.3f,
                    cx, cy, (float)UITerrain::getInstance()->calculateTriangleHeightAt(cx, cy) - 0.3f, 0.2f, 5.0f);
            UI3DDebug::addSphere(cx, cy, (float)UITerrain::getInstance()->calculateTriangleHeightAt(cx, cy), 0.5f, 5.0f);
        }
#endif

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
    static const float maxTurnSpeed = 1.0f * PI;

    //TODO: Maximum moving speed as units (grid squares) per second to unit TYPE data
    static const float maxMoveSpeed = 20.0f;

    //Static deceleration, due to friction etc.
    m_CurrentSpeed *= (0.99f - (0.99f * deltaTime));

    //Target direction
    float targetAngle = atan2(m_TargetDir.y, m_TargetDir.x);

    //Turning speed        
    float turnSpeed = maxTurnSpeed * deltaTime;

    //Difference between target and current angle
    float currentAngle = atan2(dir->y, dir->x);
    float turn = currentAngle - targetAngle;
    
    if(fabs(turn) > PI)
    {
        turn = targetAngle - currentAngle;    
    }
    
    //If current direction differs more than 45 degrees, slow down
    if(fabs(turn) > PI * 0.25f || m_State == IDLE)
    {
        //Braking factor to data?
        m_CurrentSpeed *= (0.95f - (0.95f * deltaTime));
    }


    /*if(turnSpeed > fabs(turn))
    {
        turnSpeed = -turn;
    }*/

    float turnTreshold = 0.1f; // below this, the dirs are set straight from target
    if( (turn <= turnTreshold && turn > 0.000001f) || (turn >= -turnTreshold && turn < -0.000001f) )
    {
        dir->x = m_TargetDir.x;
        dir->y = m_TargetDir.y;
    }
    else if(turn < -turnTreshold)
    {
        dir->x = cos(turnSpeed) * dir->x - sin(turnSpeed) * dir->y;
        dir->y = cos(turnSpeed) * dir->y + sin(turnSpeed) * dir->x;
    }
    else if(turn > turnTreshold)
    {
        dir->x = cos(-turnSpeed) * dir->x - sin(-turnSpeed) * dir->y;
        dir->y = cos(-turnSpeed) * dir->y + sin(-turnSpeed) * dir->x;
    }
    else
    {
        if(m_State == FOLLOWPATH)
        {
            //Heading (pretty much) toward correct direction, hit the pedal to the metal
            //TODO: Acceleration from data?
            m_CurrentSpeed += 10.0f * deltaTime;
            
            if(m_CurrentSpeed > maxMoveSpeed)
            {
                m_CurrentSpeed = maxMoveSpeed;
            }
        }
    }

    //Move the unit towards current direction by current speed
    pos->x += m_CurrentSpeed * dir->x * deltaTime;
    pos->y += m_CurrentSpeed * dir->y * deltaTime;

    unsigned short terraSize = Terrain::getInstance()->getSize();

    if(pos->x < 0)    
        pos->x = 0;
    if(pos->x + m_pUnit->getWidth() > terraSize)
        pos->x = (float)terraSize - m_pUnit->getWidth();
    if(pos->y < 0)    
        pos->y = 0;
    if(pos->y + m_pUnit->getHeight() > terraSize)
        pos->y = (float)terraSize - m_pUnit->getHeight();
    
}


bool GroundMovingLogic::release(Unit* pUnit)
{
    clearTarget();
    return true;
}


// ===== Target functionality


Target* GroundMovingLogic::getTarget()
{
    return m_pTarget;
}


void GroundMovingLogic::setTarget(Target* target)
{
    if(m_pTarget)
        clearTarget();
    m_pTarget = target;
//    m_State = ASKPATH;
}


void GroundMovingLogic::clearTarget()
{
    if(m_pTarget) {
        m_pTarget->release();
        delete m_pTarget;
        m_pTarget = NULL;
        // TODO: what? set state to idle?
        if(m_pAgent)
        {
            m_pAgent->cancel();
            delete m_pAgent;
            m_pAgent = NULL;
        }
        m_CachedReachedTargetX = -1;
        m_CachedReachedTargetY = -1;
        m_State = IDLE;
    }
}
