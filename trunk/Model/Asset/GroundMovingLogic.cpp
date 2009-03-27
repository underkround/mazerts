#include "GroundMovingLogic.h"
#include "../Terrain/Terrain.h"
#include "../Common/Vector3.h"
#include <math.h>

#ifndef PI
#define PI 3.141592653589793238462f
#endif

#ifdef DIRECT3D_VERSION // 3d debug is possible only on dx side
//#define PATH_UI_DEBUG   // comment this out when done testing
#ifdef PATH_UI_DEBUG
#include "../../ViewController/3DDebug/UI3DDebug.h"
#include "../../ViewController/Terrain/UITerrain.h"
#endif
#endif

//After how many seconds a stuck unit cancels its MAKEWAY-target
#define STUCKCOUNTER_CANCEL 1.0f

GroundMovingLogic::GroundMovingLogic(MovingDef& def) : IMovingLogic(GROUNDMOVING, def)
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
    case JUSTMOVE:
        {            
            Vector3* pos = m_pUnit->getPosition();

            //If close enough to target, return to idle
            if( fabs(m_pTarget->getTargetX() - pos->x) < 1.0f && fabs(m_pTarget->getTargetY() - pos->y) < 1.0f)
            {
                clearCurrentTarget();
            }
        }
    }

    move(deltaT);
}

void GroundMovingLogic::idle(const float deltaT)
{
    //Destroy possible old agent (the path data will be discarded along with it)
    if(m_pAgent)
    {
        m_pAgent->cancel();
        delete m_pAgent;
        m_pAgent = NULL;                
    }

    // do we have a target?
    if(m_pTarget) 
    {
        // if the target is unit itself, remove the target
        if(m_pTarget->getTargetType() == Target::ASSET && m_pTarget->getTargetAsset() == m_pUnit)
        {
            clearCurrentTarget();
        }
        else if(m_pTarget->isFlag(Target::TGTFLAG_MAKEWAY))
        {
            m_TargetDir.x = m_pTarget->getTargetX() - m_pUnit->getPosition()->x;
            m_TargetDir.y = m_pTarget->getTargetY() - m_pUnit->getPosition()->y;
            m_TargetDir.normalize();
            m_State = JUSTMOVE;
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

        IPathFinder::PathNode* pNode = m_pAgent->getPathData();        
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
        }

        //float halfX = m_pUnit->getWidth() * 0.5f;
        //float halfY = m_pUnit->getHeight() * 0.5f;
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

    //If close enough to current, get next target square (Is this correct? should the distance be based on unit size?)
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

    //Static deceleration, due to friction etc.
    m_CurrentSpeed *= (0.95f - (0.95f * deltaTime));

    //Target direction
    float targetAngle = atan2(m_TargetDir.y, m_TargetDir.x);

    //Difference between target and current angle
    float currentAngle = atan2(dir->y, dir->x);
    float turn = currentAngle - targetAngle;
    
    //Pick shorter turn route if necessary
    if(fabs(turn) > PI)
    {
        turn = -turn;
    }
    
    //Turning speed        
    float turnSpeed = m_Def.turningSpeed * deltaTime;

    //If current direction differs more than 45 degrees, slow down
    if(fabs(turn) > PI * 0.25f || m_State == IDLE)
    {
        //Braking factor to data?
        m_CurrentSpeed *= (0.90f - (0.90f * deltaTime));
    }

#define sgn(a) ((a > 0) ? 1 : (a < 0) ? -1 : 0)

    //0.01 radians = 0.57 degrees
    if(fabs(turn) > 0.01f)
    {
        if(fabs(turn) > turnSpeed)
        {
            turn = sgn(turn) * turnSpeed;
        }
        dir->x = cos(currentAngle - turn);
        dir->y = sin(currentAngle - turn);
    }
    else
    {
        dir->x = m_TargetDir.x;
        dir->y = m_TargetDir.y;

        if(m_State == FOLLOWPATH || m_State == JUSTMOVE)
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
                            Vector3 posDiff = *pNode->item->getPosition() - *pos;
                            posDiff.normalize();

                            float minDist = (float)(pNode->item->getWidth() + m_pUnit->getWidth());

                            //Check if the target already has a MAKEWAY-priority target
                            Target* unitTarget = pNode->item->getMovingLogic()->getTarget();
                            if(unitTarget == NULL || !unitTarget->isFlag(Target::TGTFLAG_MAKEWAY))
                            {
                                Target* makeWayTarget = NULL;
                                
                                int targetX = 0;
                                int targetY = 0;

                                //Take vector from unit position towards target moving out of way, select normal from either side randomly
                                if(rand() > (RAND_MAX / 2))
                                {
                                    targetX = (int)(pNode->item->getPosition()->x - posDiff.y * minDist);
                                    targetY = (int)(pNode->item->getPosition()->y + posDiff.x * minDist);                                    
                                }
                                else
                                {
                                    targetX = (int)(pNode->item->getPosition()->x + posDiff.y * minDist);
                                    targetY = (int)(pNode->item->getPosition()->y - posDiff.x * minDist);
                                }

                                //Bounds checks
                                unsigned short mapSize = Terrain::getInstance()->getSize();
                                if(targetX < 0)
                                {
                                    targetX = 0;
                                }
                                else if(targetX > mapSize)
                                {
                                    targetX = mapSize;
                                }
                                
                                if(targetY < 0)
                                {
                                    targetY = 0;
                                }
                                else if(targetY > mapSize)
                                {
                                    targetY = mapSize;
                                }

                                makeWayTarget = new Target((unsigned short)targetX, (unsigned short)targetY, false);

                                makeWayTarget->setFlag(Target::TGTFLAG_MAKEWAY);

                                pNode->item->getMovingLogic()->priorityTarget(makeWayTarget);

//show line for where the unit is told to move
#ifdef PATH_UI_DEBUG
        float halfX = m_pUnit->getWidth() * 0.5f;
        float halfY = m_pUnit->getHeight() * 0.5f;
        float x1 = pNode->item->getPosition()->x;
        float y1 = pNode->item->getPosition()->y;
        //float x2 = pNode->item->getPosition()->x + posDiff.x * minDist;
        //float y2 = pNode->item->getPosition()->y + posDiff.y * minDist;
        float x2 = (float)makeWayTarget->getTargetX();
        float y2 = (float)makeWayTarget->getTargetY();
        UI3DDebug::addLine( x1, y1, (float)UITerrain::getInstance()->calculateTriangleHeightAt(x1, y1) - 0.5f,
                            x2, y2, (float)UITerrain::getInstance()->calculateTriangleHeightAt(x2, y2) - 0.5f, 0.5f, 5.0f);
#endif
                                
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
                m_CurrentSpeed = factor * m_Def.maxSpeed;
            }
            else
            {
                //Slow down to halt
                m_CurrentSpeed *= (0.90f - (0.90f * deltaTime));

                //Used to clear "stuck" MAKEWAY-flags (häröpallo)
                if(m_pTarget && m_pTarget->isFlag(Target::TGTFLAG_MAKEWAY))
                {
                    m_StuckCounter += deltaTime;
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