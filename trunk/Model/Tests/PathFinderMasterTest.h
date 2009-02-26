#include "../PathFinding/PathFinderMaster.h"
#include "../PathFinding/PathAgent.h"
#include <stdio.h>
#include "../Common/CConsole.h"
#include "../Terrain/Terrain.h"


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
        count = 2100;//rand() % 1000;
        nodeCount = 0;
    }

    PathAgent* pAgent;
    AgentNode* pNext;
    AgentNode* pPrev;
    int count;
    int nodeCount;
    int id;
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
    const int MAPSIZE = 1023;
    Terrain* terrain = Terrain::getInstance();
    terrain->initialize(MAPSIZE);

    //Set to higher loops for longer searches
    for(int i = 0; i < 4000; i++)
    {
        int len = rand() % 60;
        int x = rand() % MAPSIZE;
        int y = rand() % MAPSIZE;
        
        for(int j = 0; j < len; j++)
        {
            terrain->setTerrainVertexHeight(x++, y, 0);
        }
    }

   

    PathFinderMaster* pMaster = PathFinderMaster::getInstance();

    pMaster->start();

    CConsole* pConsole = new CConsole();
    static INPUT_RECORD record;
    memset(&record, 0, sizeof(INPUT_RECORD));

    bool run = true;

    bool noNew = false;

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
                noNew = true;
            }
            else
            {
                noNew = false;
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

        if(agents)
        {
            AgentNode* pCurrent = pNodeStart;

            char strMsg[100];
            
            pConsole->ClearBuffer();

            int row = 0;                       

            while(pCurrent != NULL && row < 49)
            {
                

                if(pCurrent->pAgent->getState() != PathFinder::NOT_FINISHED)
                {
                    //TEST: get all the steps you can before dying
                    if(pCurrent->pAgent->getNextPathNode() != NULL)
                    {
                        pCurrent->nodeCount++;
                    }

                    sprintf_s(strMsg, 100, "Agent %d: State: %s NodeCount: %d", pCurrent->id, strState[pCurrent->pAgent->getState()], pCurrent->nodeCount);
                    pConsole->writeMessage(10, ++row, strMsg);
                    sprintf_s(strMsg, 100, "Preparing to delete:  %d", pCurrent->count--);
                    pConsole->writeMessage(50, row, strMsg);
                    if(pCurrent->count <= 0)
                    {
                        pCurrent = removeAgentNode(pCurrent);
                    }
                }
                else
                {
                    sprintf_s(strMsg, 100, "Agent %d: State: %s ", pCurrent->id, strState[pCurrent->pAgent->getState()]);
                    pConsole->writeMessage(10, ++row, strMsg);
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
            sprintf_s(strMsg, 100, "Running: %d  Alive: %d Total all time: %d", running, agents, agentID);
            pConsole->writeMessage(4, 0, strMsg);
            pConsole->DrawScreen();            
        }

        if(rand() % ((running * 10) + 1) == 0 && !noNew)
        {
            //for(i = 0; i < rand() % 20); i++)
            {
                addAgentNode(PathFinderMaster::findPath(0, 0, rand() % MAPSIZE, rand() % MAPSIZE, 4));
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