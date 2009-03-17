#include "GroundMovingLogic.h"
#include "../Terrain/Terrain.h"
#include "../Common/Vector3.h"
#include <math.h>

#ifndef PI
#define PI 3.141592653589793238462f
#endif

//THE MODEL WON'T COMPILE WITH THIS, COMMENT OUT WHEN NOT TESTING!
//#define PATH_UI_DEBUG
#ifdef PATH_UI_DEBUG
#include "../../ViewController/3DDebug/UI3DDebug.h"
#include "../../ViewController/Terrain/UITerrain.h"
#endif

//After how many frames a stuck unit cancels its MAKEWAY-target
#define STUCKCOUNTER_CANCEL 300

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

    m_StuckCounter = 0;
}

GroundMovingLogic::~GroundMovingLogic()
{
    if(m_pAgent)
    {
        delete m_pAgent;
        m_pAgent = NULL;
    }
    clearTargets();
}

void GroundMovingLogic::attach(Unit* pUnit)
{
    m_pUnit = pUnit;
    m_TargetDir.x = m_pUnit->getDirection()->x;
    m_TargetDir.y = m_pUnit->getDirection()->y;

    m_HalfSize = m_pUnit->getWidth() * 0.5f;
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

    // do we have a target?
    if(m_pTarget) 
    {
        // if the target is unit itself, remove the target
        if(m_pTarget->getTargetType() == Target::ASSET && m_pTarget->getTargetAsset() == m_pUnit)
        {
            clearCurrentTarget();
        }
        // is the target reached?
        else if( m_CachedReachedTargetX != m_pTarget->getTargetX() ||
            m_CachedReachedTargetY != m_pTarget->getTargetY()   )
        {
            // target has changed since we reached it
            m_CachedReachedTargetX = -1;
            m_CachedReachedTargetY = -1;
            m_State = ASKPATH;
        }
    }
    else
    {
        //No target, try to pull new one from queue
        if(!m_TargetList.empty())
        {
            m_pTarget = m_TargetList.popHead();
        }
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

    if(!m_pTarget)
    {
        m_State = IDLE;
        return;
    }
    unsigned short m_TargetX = m_pTarget->getTargetX();
    unsigned short m_TargetY = m_pTarget->getTargetY();

    // is the target reached? (Can differ by 1 square)
    if( abs(m_pUnit->getGridX() - m_TargetX) < 2 && abs(m_pUnit->getGridY() == m_TargetY) < 2)
    {
        // if the target was static coordinates - not tracking asset,
        // we can remove it when it's reached
        if(m_pTarget->isStatic())
        {
            m_CachedReachedTargetX = -1;
            m_CachedReachedTargetY = -1;
            clearCurrentTarget();
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
        clearCurrentTarget();
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
            clearCurrentTarget();
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
        //Cancelled or no path, destroy target and idle
        clearCurrentTarget();
    }
}

void GroundMovingLogic::followPath()
{
    Vector3* dir = m_pUnit->getDirection();
    Vector3* pos = m_pUnit->getPosition();

    //If close enough to current, get next target square
    if( fabs(m_pPathNode->x - pos->x) < 1.0f && fabs(m_pPathNode->y - pos->y) < 1.0f)
    {
        m_pPathNode = m_pAgent->getNextPathNode();        
    }

    //Did we finish the path?
    if(m_pPathNode == NULL)
    {
        clearCurrentTarget();
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
    static const float maxMoveSpeed = 10.0f;

    //Static deceleration, due to friction etc.
    m_CurrentSpeed *= (0.95f - (0.95f * deltaTime));

    //Target direction
    float targetAngle = atan2(m_TargetDir.y, m_TargetDir.x);

    //Turning speed        
    float turnSpeed = maxTurnSpeed * deltaTime;

    //Difference between target and current angle
    float currentAngle = atan2(dir->y, dir->x);
    float turn = currentAngle - targetAngle;
    
    //Pick shorter turn route if necessary
    if(fabs(turn) > PI)
    {
        turn = targetAngle - currentAngle;    
    }
    
    //If current direction differs more than 45 degrees, slow down
    if(fabs(turn) > PI * 0.25f || m_State == IDLE)
    {
        //Braking factor to data?
        m_CurrentSpeed *= (0.90f - (0.90f * deltaTime));
    }


    /*if(turnSpeed > fabs(turn))
    {
        turnSpeed = -turn;
    }*/

    float turnThreshold = 0.03f; // below this, the dirs are set straight from target
    if( (turn <= turnThreshold && turn > 0.000001f) || (turn >= -turnThreshold && turn < -0.000001f) )
    {
        dir->x = m_TargetDir.x;
        dir->y = m_TargetDir.y;
    }
    else if(turn < -turnThreshold)
    {
        dir->x = cos(turnSpeed) * dir->x - sin(turnSpeed) * dir->y;
        dir->y = cos(turnSpeed) * dir->y + sin(turnSpeed) * dir->x;
    }
    else if(turn > turnThreshold)
    {
        dir->x = cos(-turnSpeed) * dir->x - sin(-turnSpeed) * dir->y;
        dir->y = cos(-turnSpeed) * dir->y + sin(-turnSpeed) * dir->x;
    }
    else
    {
        if(m_State == FOLLOWPATH)
        {
             //Current moving directions
            signed char dirX = (char)floor(dir->x + 0.5f);
            signed char dirY = (char)floor(dir->y + 0.5f);            

            //Check the squares for availability
            bool squaresAvailable = true;
            static DoubleLinkedList<Unit*> unitsAtSquares;  //Reusing same list all the time
    
            //Ask for units in the way
            if(AssetCollection::getUnitsAt(&unitsAtSquares, (unsigned short)(floor(pos->x + 0.5f) + dirX), (unsigned short)(floor(pos->y  + 0.5f) + dirY), m_pUnit->getWidth(), m_pUnit->getHeight()) != 0)
            {
                //Check found units
                ListNode<Unit*>* pNode = unitsAtSquares.headNode();
                while(pNode)
                {
                    //Check that the unit isn't this one
                    if(pNode->item != m_pUnit)
                    {
                        //Only own units can be told to move
                        if(pNode->item->getOwner() == m_pUnit->getOwner())
                        {
                            float half = pNode->item->getWidth() * 0.5f;
                            Vector3 posDiff = *pNode->item->getPosition() - *pos;
                            //float dxsq = posDiff.x * posDiff.x;                        
                            //float dysq = posDiff.y * posDiff.y;
                            float minDist = half + m_HalfSize;
                            //minDist *= minDist;
                            //if(sqrt(dxsq + dysq) < minDist)                           
                            {
                                posDiff.normalize();
                                //Don't mind me, I'll just push you away a bit (testing)
                                pNode->item->getPosition()->x += posDiff.x * deltaTime;
                                pNode->item->getPosition()->y += posDiff.y * deltaTime;

                                //Check if the target already has a MAKEWAY-priority flag
                                Target* unitTarget = pNode->item->getMovingLogic()->getTarget();
                                if(unitTarget == NULL || !unitTarget->isFlag(Target::TGTFLAG_MAKEWAY))
                                {
                                    Target* makeWayTarget = new Target((unsigned short)(pNode->item->getPosition()->x + posDiff.x * minDist), 
                                                                                         (unsigned short)(pNode->item->getPosition()->y + posDiff.y * minDist),
                                                                                         false);
                                    makeWayTarget->setFlag(Target::TGTFLAG_MAKEWAY);

                                    pNode->item->getMovingLogic()->priorityTarget(makeWayTarget);
                                }
                            }
                        }                        

                        squaresAvailable = false;
                    }
                    
                    pNode = pNode->next;
                }
            }

            //Clear the list
            unitsAtSquares.release();

            if(squaresAvailable)
            {

                //Heading (pretty much) toward correct direction and the road is clear, hit the pedal to the metal    
                //Offsets (m_HalfSize) are needed because the speed is calculated from "center" of unit
                float factor = Terrain::getInstance()->getUnitMoveSpeed( (short)(pos->x + m_HalfSize), (short)(pos->y + m_HalfSize), dirX, dirY);            
                m_CurrentSpeed = factor * maxMoveSpeed;
                /*TCHAR msg[256];            
                _stprintf_s(msg, _T("FACTOR: %.3f   SPEED: %.3f\n"), factor, m_CurrentSpeed);
                ::OutputDebugStr(msg);*/
            }
            else
            {
                //Slow down to halt
                m_CurrentSpeed *= (0.90f - (0.90f * deltaTime));

                //Used to clear "stuck" MAKEWAY-flags (häröpallo)
                if(m_pTarget && m_pTarget->isFlag(Target::TGTFLAG_MAKEWAY))
                {
                    m_StuckCounter++;
                    if(m_StuckCounter > STUCKCOUNTER_CANCEL)
                    {
                        clearCurrentTarget();
                        m_StuckCounter = 0;
                        return;
                    }
                }
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
    clearTargets();
    return true;
}


// ===== Target functionality


Target* GroundMovingLogic::getTarget()
{
    return m_pTarget;
}


void GroundMovingLogic::addTarget(Target* target)
{
    m_TargetList.pushTail(target);
}


void GroundMovingLogic::clearTargets()
{
    //Release current
    if(m_pTarget) 
    {
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

    //Release queued
    if(!m_TargetList.empty())
    {
        ListNode<Target*>* pNode = m_TargetList.headNode();

        while(pNode)
        {
            pNode->item->release();
            delete pNode->item;
            pNode = m_TargetList.removeGetNext(pNode);
        }
    }
}

void GroundMovingLogic::clearCurrentTarget()
{
    if(m_pTarget)
    {
        delete m_pTarget;
        m_pTarget = NULL;
    }
    m_State = IDLE;
}

void GroundMovingLogic::priorityTarget(Target* target)
{
    if(m_pTarget)
    {
        m_TargetList.pushHead(m_pTarget);
    }
    m_pTarget = target;
    m_State = IDLE;
    m_StuckCounter = 0; //For MAKEWAY-targets
}