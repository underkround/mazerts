#include "../PathFinding/PathFinderMaster.h"
#include "../PathFinding/PathAgent.h"
#include <stdio.h>
#include "../Common/CConsole.h"
#include "../Terrain/Terrain.h"
//#include "CTimer.h"


#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif


int agents = 0;
int agentID = 0;

struct AgentNode
{
    AgentNode(PathAgent* _pAgent)
    {
        pAgent = _pAgent;
        pNext = NULL;
        pPrev = NULL;
        id = agentID++;
        count = 800;//rand() % 1000;
        //timer.Create();
        //timer.BeginTimer();
        time = 0.0f;
        //nodeCount = 0;
        finished = false;
    }

    PathAgent* pAgent;
    AgentNode* pNext;
    AgentNode* pPrev;
    int count;
    //int nodeCount;
    //CTimer timer;
    float time;
    int id;
    bool finished;
};

AgentNode* pNodeStart = NULL;

void addAgentNode(PathAgent* pAgent)
{
    if(pAgent == NULL)
    {
        return;
    }
    
    AgentNode* pNode = new AgentNode(pAgent);

    if(pNodeStart)
    {
        //Add to start
        pNode->pNext = pNodeStart;
        pNodeStart->pPrev = pNode;
        pNodeStart = pNode;

    }
    else
    {
        pNodeStart = pNode;
    }

    agents++;
}


AgentNode* removeAgentNode(AgentNode* pNode)
{
    AgentNode* pReturnPointer = pNode->pNext;

    //Fix starting pointer if the current start node is being removed        
    if(pNode == pNodeStart)
    {
        pNodeStart = pNodeStart->pNext;        
    }
    else
    {
        if(pNode->pNext)
        {
            pNode->pNext->pPrev = pNode->pPrev;
        }
        
        if(pNode->pPrev)
        {
            pNode->pPrev->pNext = pNode->pNext;            
        }
    }
    
    delete pNode->pAgent;
    pNode->pAgent = NULL;
    delete pNode;
    pNode = NULL;
    agents--;
    
    return pReturnPointer;
}

static char** strState = new char*[4];


void testPathFinderMaster()
{
    strState[0] = "Not finished";
    strState[1] = "NO PATH FOUND";
    strState[2] = "CANCELLED";
    strState[3] = "FOUND";

    srand(100000);
    const int MAPSIZE = 512;//256;
    Terrain* terrain = Terrain::getInstance();
    terrain->initialize(MAPSIZE); 

    PathFinderMaster* pMaster = PathFinderMaster::getInstance();

    pMaster->start();

    CConsole* pConsole = new CConsole();
    static INPUT_RECORD record;
    memset(&record, 0, sizeof(INPUT_RECORD));

    bool run = true;

    bool noNew = false;

   // CTimer* timer = new CTimer();
   // timer->Create();
    float time = 0.0f;

    while(run)
    {
        int i = pConsole->ReadInput(&record);
        
        if(record.EventType == KEY_EVENT && i)
        {
            int key = record.Event.KeyEvent.wVirtualScanCode;
            if(key == 1)
            {
                run = false;
            }
            if(key == 57)
            {
                PathFinderMaster::cancelAll();
            
            }
            
            if(key == 14 && record.Event.KeyEvent.bKeyDown == TRUE)
            {
                noNew = !noNew;
            }

            /*else if(key == 57 && record.Event.KeyEvent.bKeyDown == TRUE)
            {
                //for(int i = 0; i < 10; i++)
                {
                    addAgentNode(PathFinderMaster::findPath(0, 0, rand() % MAPSIZE, rand() % MAPSIZE, 2));
                }
            }*/

        }

        int running = 0;
        char strMsg[100];

        if(agents)
        {
            AgentNode* pCurrent = pNodeStart;
            
            pConsole->ClearBuffer();

            int row = 2;                       

            while(pCurrent != NULL)
            {
                if(row < 49)
                {
                    sprintf_s(strMsg, 100, "Agent %d: State: %s  Time: %.3fs", pCurrent->id, strState[pCurrent->pAgent->getState()], pCurrent->time);
                    pConsole->writeMessage(10, ++row, strMsg);
                }

                if(pCurrent->pAgent->getState() != PathFinder::NOT_FINISHED)
                {
                    if(pCurrent->finished == false)
                    {
//                      pCurrent->timer.EndTimer();
//                      pCurrent->time = pCurrent->timer.GetElapsedSeconds();
                        pCurrent->finished = true;
                    }

                    sprintf_s(strMsg, 100, "Preparing to delete:  %d", pCurrent->count);
                    pConsole->writeMessage(50, row, strMsg);

                    //TEST: get all the steps you can before dying
                    IPathFinder::PathNode* pNode = pCurrent->pAgent->getNextPathNode();
                    if(pNode != NULL)
                    {
                        if(pNode->x > terrain->getSize() || pNode->y > terrain->getSize())
                        {
                            pConsole->ClearBuffer();
                            pConsole->writeMessage(0, row, "INVALID DATA!!!!!");
                        }

                        //pCurrent->nodeCount++;
                    }

                    pCurrent->count--;

                    if(pCurrent->count <= 0)
                    {
                        pCurrent = removeAgentNode(pCurrent);
                    }
                }
                else
                {
                    running++;
                }

                if(pCurrent && rand() % 10000 == 1)
                {
                    pCurrent->pAgent->cancel();
                }


                if(pCurrent)
                {
                    pCurrent = pCurrent->pNext;
                }
            }
        }
        pConsole->writeMessage(2, 0, "Press BACKSPACE to toggle creating new, SPACE to cancel all current searches");
        sprintf_s(strMsg, 100, "Agents: Reported running: %d  Alive Agents: %d Total agents: %d", running, agents, agentID);
        pConsole->writeMessage(1, 1, strMsg);
        sprintf_s(strMsg, 100, "Master: Running: %d  Waiting: %d   ms Create new:%d", PathFinderMaster::getInstance()->getRunningAmount(), PathFinderMaster::getInstance()->getWaitingAmount(), !noNew);
        pConsole->writeMessage(1, 2, strMsg);
        pConsole->DrawScreen();
        pConsole->ClearBuffer();

        //if(rand() % ((running * 10) + 1) == 0 && !noNew)
        if(!noNew)
        {
            //for(i = 0; i < rand() % 40; i++)
            for(i = 0; i < 10; i++)
            {
                addAgentNode(PathFinderMaster::findPath(rand() % MAPSIZE,  rand() % MAPSIZE, rand() % MAPSIZE, rand() % MAPSIZE, (rand() % 5) + 1));
                //addAgentNode(PathFinderMaster::findPath(0, 0, MAPSIZE-1, MAPSIZE-1, 1));
            }
        }
        
        Sleep(1);
    }

    delete pConsole;

    PathFinderMaster::cancelAll();

    AgentNode* pCurrent = pNodeStart;
    while(pCurrent != NULL)
    {
        pCurrent = removeAgentNode(pCurrent);
    }

    pMaster->stop();
    
    if(pMaster->wait())
    {
        printf("Something went wrong\n");
    }    

    Terrain::getInstance()->initialize(1);

    delete [] strState;
    
    printf("DONE\n");
}