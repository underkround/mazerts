#include "Resourcer.h"
#include "Unit.h"
#include "Building.h"
#include "AssetCollection.h"
#include "../Command/Target.h"
#include "../Common/DoubleLinkedList.h"
#include "../Terrain/Terrain.h"

void Resourcer::update(const float deltaT)
{
    Terrain* pTerrain = Terrain::getInstance();
    unsigned short size = pTerrain->getSize();
    DoubleLinkedList<Building*> list;

    switch(m_State)
    {
    case RES_IDLE:
        {
            int tagToLookFor = 0;
            int playerToLookFor = 0;

            //Look for target
            if(m_Ore == 0)
            {
                //TODO: hardcoded value for ore mine, better way to distinguish?
                tagToLookFor = 51;
            }
            else
            {
                //TODO: Tag for resource yard? For testing using war factory
                tagToLookFor = 52;
                playerToLookFor = m_pUnit->getOwner()->getIndex();
            }

            
            //Find mine or resource yard
            AssetCollection::getPlayerBuildingsAt(&list, playerToLookFor, 0, 0, size, size);

            ListNode<Building*>* pNode = list.headNode();
            int shortestDistSq = 100000000;
            Building* pMine = NULL;

            //Look up ore mines/resource yards
            while(pNode)
            {
                Building* pBuilding = pNode->item;            
                if(pBuilding->getDef()->tag == tagToLookFor)
                {
                    //Ore mine, check distance
                    int distXSq = abs(m_pUnit->getGridX() - pBuilding->getGridX());
                    distXSq *= distXSq;
                    int distYSq = abs(m_pUnit->getGridY() - pBuilding->getGridY());
                    distYSq *= distYSq;

                    if(distXSq + distYSq < shortestDistSq)
                    {
                        pMine = pNode->item;
                        shortestDistSq = distXSq + distYSq;
                    }
                }

                pNode = pNode->next;
            }
          
            //Did we find a mine/resource yard?
            if(pMine)
            {
                //Get entrance location
                unsigned short x = pMine->getGridX() + pMine->getDef()->gridEntrancePointX;
                unsigned short y = pMine->getGridY() + pMine->getDef()->gridEntrancePointY;
                
                //Create target and give to moving logic
                m_pTarget = new Target(x, y, false);
                m_pUnit->getMovingLogic()->addTarget(m_pTarget);
                m_State = RES_MOVING;
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
