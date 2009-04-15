#include "Resourcer.h"
#include "Unit.h"
#include "Building.h"
#include "AssetCollection.h"
#include "../Command/Target.h"
#include "../Terrain/Terrain.h"
#include "../PathFinding/PathFinderMaster.h"
#include "../PathFinding/PathAgent.h"

void Resourcer::release()
{
    if(m_pTargetList)
    {
        delete m_pTargetList;
        m_pTargetList = NULL;
    }
    if(m_pTarget)
    {
        //TODO: How is the resourcer informed if GroundMovingLogic destroyed the target?
        //delete m_pTarget;
        m_pTarget = NULL;
    }
    if(m_pAgent)
    {
        delete m_pAgent;
        m_pAgent = NULL;
    }
}


void Resourcer::update(const float deltaT)
{
    Terrain* pTerrain = Terrain::getInstance();
    unsigned short size = pTerrain->getSize();
    DoubleLinkedList<Building*> list;

    switch(m_State)
    {
    case RES_IDLE:
        {
            //Clear the target list
            m_pTargetList->RemoveAll();

            int tagToLookFor = 0;
            int playerToLookFor = 0;

            //Look for target
            if(m_Ore == 0)
            {
                //TODO: hardcoded tag for ore mine, better way to distinguish?
                tagToLookFor = 51;
            }
            else
            {
                //TODO: Hard coded tag for resource yard, better way to distinguish
                tagToLookFor = 53;
                playerToLookFor = m_pUnit->getOwner()->getIndex();
            }

            
            //Find mine or resource yard
            AssetCollection::getPlayerBuildingsAt(&list, playerToLookFor, 0, 0, size, size);

            ListNode<Building*>* pNode = list.headNode();

            //Look up ore mines/resource yards
            while(pNode)
            {
                Building* pBuilding = pNode->item;            
                if(pBuilding->getDef()->tag == tagToLookFor)
                {
                    //Calculate squared distance
                    int distXSq = abs(m_pUnit->getGridX() - pBuilding->getGridX());
                    distXSq *= distXSq;
                    int distYSq = abs(m_pUnit->getGridY() - pBuilding->getGridY());
                    distYSq *= distYSq;

                    //Push to viable targets tree
                    m_pTargetList->Insert(pBuilding, distXSq + distYSq);

                    //On the next update, the resourcer starts to look for paths
                    m_State = RES_WAITCANPATH;
                }

                pNode = pNode->next;
            }
            
                
            break;
        }
    case RES_WAITCANPATH:
        {   
            //TODO: Do targets leak memory?

            //Check for targets if no pathfinding is underway
            if(!m_pAgent)
            {
                if(!m_pTargetList->IsEmpty())
                {
                    Building* pBuilding = NULL;
                    m_pTargetList->GetTopID(&pBuilding);
                    m_pTarget = new Target(pBuilding, Target::TGTFLAG_INSIDE_BUILDING);
                    m_pTargetList->RemoveTop();
                    //TODO: No "canPath"-checks in pathfinding yet, using real pathfinding-logic
                    m_pAgent = PathFinderMaster::findPath(m_pUnit->getCenterGridX(), m_pUnit->getCenterGridY(), m_pTarget->getTargetX(), m_pTarget->getTargetY(), m_pUnit->getWidth());
                }
                else
                {
                    //All viable targets have been gone through, and the state is still RES_WAITCANPATH... return to IDLE
                    m_State = RES_IDLE;
                }
            }

            if(m_pAgent)
            {
                //Was path found?
                switch(m_pAgent->getState())
                {
                case IPathFinder::FOUND:
                    {
                        //Give target to moving logic
                        m_pUnit->getMovingLogic()->addTarget(m_pTarget);
                        m_State = RES_MOVING;
                        delete m_pAgent;
                        m_pAgent = NULL;

                        break;
                    }
                case IPathFinder::NOT_FOUND:    //NOT_FOUND means there is no path at all, NOT_FINISHED means search hasn't been ended yet
                    {
                        delete m_pAgent;
                        m_pAgent = NULL;
                        break;
                    }

                }
            }
            
            break;
        }

    case RES_MOVING:
        //TODO: NEEDS better system to keep track of reaching target (nonreachable/target might get destroyed => crash)
        if((abs(m_pTarget->getTargetX() - m_pUnit->getCenterGridX()) + abs(m_pTarget->getTargetY() - m_pUnit->getCenterGridY())) < 2)
        {
            if(m_Ore == 0)
            {
                m_State = RES_LOADING;
            }
            else
            {
                m_State = RES_UNLOADING;
            }
        }
        break;


    case RES_LOADING:
        
        m_LoadTimer += deltaT;
        if(m_LoadTimer > m_Def.loadTime)
        {
            m_LoadTimer = 0;
            m_Ore = m_Def.oreCapacity;        
            m_State = RES_IDLE;
        }
        
        break;


    case RES_UNLOADING:
        m_LoadTimer += deltaT;
        if(m_LoadTimer > m_Def.loadTime)
        {
            m_LoadTimer = 0;
            m_Ore = 0;
            //TODO: Add ore to player
        
            m_State = RES_IDLE;
        }
        break;
    }

}
