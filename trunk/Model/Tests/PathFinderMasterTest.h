#include "../PathFinding/PathFinderMaster.h"
#include "../PathFinding/PathAgent.h"
#include <stdio.h>
#include "../Common/CConsole.h"
#include "../Terrain/Terrain.h"



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
        count = rand() % 1000;
    }

    PathAgent* pAgent;
    AgentNode* pNext;
    AgentNode* pPrev;
    int count;
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
    for(int i = 0; i < 4; i++)
    {
        strState[i] = new char[20];
    }

    strState[0] = "Not finished";
    strState[1] = "No path";
    strState[2] = "Cancelled";
    strState[3] = "FOUND";

    srand(100000);
    const int MAPSIZE = 1023;
    Terrain* terrain = Terrain::getInstance();
    terrain->initialize(MAPSIZE);

    for(int i = 0; i < 100; i++)
    {
        int len = rand() % 50;
        int x = rand() % MAPSIZE;
        int y = rand() % MAPSIZE;
        
        for(int j = 0; j < len; j++)
        {
            terrain->setTerrainVertexHeight(x++, y, j*30 % 255);
        }
    }

   

    PathFinderMaster* pMaster = PathFinderMaster::getInstance();

    pMaster->start();

    CConsole* pConsole = new CConsole();
    static INPUT_RECORD record;
    memset(&record, 0, sizeof(INPUT_RECORD));

    bool run = true;
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
            else if(key == 57 && record.Event.KeyEvent.bKeyDown == TRUE)
            {
                //for(int i = 0; i < 10; i++)
                {
                    addAgentNode(PathFinderMaster::findPath(0, 0, rand() % MAPSIZE, rand() % MAPSIZE, 2));
                }
            }

        }

        if(agents)
        {
            AgentNode* pCurrent = pNodeStart;

            char strMsg[100];
            
            pConsole->ClearBuffer();

            sprintf(strMsg, "Agents: %d" , agents);
            pConsole->writeMessage(50, 0, strMsg);
            int row = 0;
            
            while(pCurrent != NULL && row < 49)
            {
                sprintf_s(strMsg, 100, "Agent %d: State: %s ", pCurrent->id, strState[pCurrent->pAgent->getState()]);
                pConsole->writeMessage(10, ++row, strMsg);
                
                if(pCurrent->pAgent->getState() != IPathFinder::NOT_FINISHED)
                {
                    sprintf_s(strMsg, 100, "Preparing to delete:  %d", pCurrent->count--);
                    pConsole->writeMessage(50, row, strMsg);
                    if(pCurrent->count <= 0)
                    {
                        pCurrent = removeAgentNode(pCurrent);
                    }
                }
                if(pCurrent)
                {
                    pCurrent = pCurrent->pNext;
                }
            }
            if(!agents)
            {
                pConsole->ClearBuffer();
            }
            pConsole->DrawScreen();            
        }
        Sleep(5);
    }

    pMaster->stop();
    
    if(pMaster->wait())
    {
        printf("Something went wrong\n");
    }
    
    printf("DONE\n");
}