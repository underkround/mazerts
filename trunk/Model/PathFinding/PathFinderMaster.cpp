#include "PathFinderMaster.h"
#include "../Common/Sleep.h"

PathFinderMaster* PathFinderMaster::pInstance = new PathFinderMaster();

PathFinderMaster::PathFinderMaster()
{
    m_Running = false;
    m_FinderNodes = 0;
    m_WaitingNodes = 0;

    //Create mutexes
    m_pNodeListMutex = new pthread_mutex_t;
    pthread_mutex_init(m_pNodeListMutex, NULL);    

    m_pWaitListMutex = new pthread_mutex_t;
    pthread_mutex_init(m_pWaitListMutex, NULL);

    m_pFinderNodeStart = NULL;
    m_pWaitingNodeStart = NULL;
    m_pWaitingNodeEnd = NULL;
    m_ppCanPath = NULL;
}


PathFinderMaster::~PathFinderMaster()
{
    clearLists();

    unsigned short size = Terrain::getInstance()->getSize();

    if(m_ppCanPath)
    {
        for(int i = 0; i < size; i++)
        {
                delete [] m_ppCanPath[i];
                m_ppCanPath[i] = NULL;
        }
        delete [] m_ppCanPath;
        m_ppCanPath = NULL;
    }

    pthread_mutex_destroy(m_pNodeListMutex);
    delete m_pNodeListMutex;
    m_pNodeListMutex = NULL;

    pthread_mutex_destroy(m_pWaitListMutex);
    delete m_pWaitListMutex;
    m_pWaitListMutex = NULL;
}


void PathFinderMaster::run()
{    
    int steps = 0;

    while(m_Running)
    {
        //Get new pathfinder from waiting list, if maximum amount isn't reached
        while(m_FinderNodes < MAX_FINDERS_RUNNING && m_WaitingNodes)
        {
            PathFinderNode* pNode = popWaitingFinderNode();
            
            if(pNode != NULL)
            {
                //Check that the search is still valid
                if(pNode->pFinder->getPathAgent() != NULL && pNode->pFinder->getPathAgent()->getState() == IPathFinder::NOT_FINISHED)
                {
                    //Prepare the pathfinder for the search
                    pNode->pFinder->prepareForExecution();
                    addPathFinderNode(pNode->pFinder);
                }
                else
                {
                    delete pNode->pFinder;
                }
                delete pNode;
            }
        }


        PathFinderNode* pCurrent = m_pFinderNodeStart;

        //Deadlock, deletePathFinderNode will cause another locking to occur
        //pthread_mutex_lock(m_pNodeListMutex);
        if(m_FinderNodes > 0)
        {
            while(pCurrent != NULL)
            {
                //Calculate the steps
                steps = STEPS_PER_LOOP / m_FinderNodes;
                
                //printf("Advancing PathFinderNode #%d towards %d %d\n", pCurrent->DEBUG_id, pCurrent->pFinder->getGoalX(), pCurrent->pFinder->getGoalY());

                IPathFinder::PathingState pState = pCurrent->pFinder->advance(steps);

                if(pState != IPathFinder::NOT_FINISHED)
                {
                    //Remove PathFinder
                    delete pCurrent->pFinder;
                    pCurrent->pFinder = NULL;

                    //Remove PathFinderNode
                    pCurrent = deletePathFinderNode(pCurrent);

                }

                //Advance in list, pCurrent could be NULL if the removePathFinderNoder
                //removed last node
                if(pCurrent)
                {
                    pCurrent = pCurrent->pNext;
                }
            }
        }
        //pthread_mutex_unlock(m_pNodeListMutex);
        msleep(1);
    }

    clearLists();
}

//Static
PathAgent* PathFinderMaster::findPath(unsigned short x, unsigned short y, unsigned short goalX, unsigned short goalY, unsigned char size)
{
    //CanPath-check  DEBUG, "real" canPath is still in the works
    /*if(!pInstance->m_ppCanPath[y / CANPATH_BLOCKS][x / CANPATH_BLOCKS])
    {
        return NULL;
    }*/

    PathFinder* pFinder = new PathFinder(x, y, goalX, goalY, size);
    if(pFinder->isInitialized())
    {        
        pInstance->pushWaitingFinderNode(pFinder);
        return pFinder->getPathAgent();
    }
    else
    {
        delete pFinder;
        return NULL;
    }
}

//Static
void PathFinderMaster::cancelAll()
{
    pthread_mutex_lock(pInstance->m_pNodeListMutex);
    {
        PathFinderNode* pCurrent = pInstance->m_pFinderNodeStart;
        while(pCurrent != NULL)
        {
            pCurrent->pFinder->cancel();
            pCurrent = pCurrent->pNext;            
        }
    }
    pthread_mutex_unlock(pInstance->m_pNodeListMutex);


    pthread_mutex_lock(pInstance->m_pWaitListMutex);
    {
        PathFinderNode* pCurrent = pInstance->m_pWaitingNodeStart;
        while(pCurrent != NULL)
        {
            //Cancel but don't destroy, the waiting nodes will be switched
            //to running, and proper cancel-actions will take place
            pCurrent->pFinder->cancel();
            pCurrent = pCurrent->pNext;
        }
    }
    pthread_mutex_unlock(pInstance->m_pWaitListMutex);
}



void PathFinderMaster::pushWaitingFinderNode(PathFinder* pFinder)
{
    pthread_mutex_lock(m_pWaitListMutex);
    //The block is here just to show that this part is inside mutex
    {
        if(m_pWaitingNodeStart && m_pWaitingNodeEnd)
        {
            //Add the new node in back of the list
            PathFinderNode* pNode = new PathFinderNode();
            pNode->pFinder = pFinder;

            m_pWaitingNodeEnd->pNext = pNode;
            pNode->pPrev = m_pWaitingNodeEnd;
            m_pWaitingNodeEnd = pNode;
        }
        else
        {
            //This one becomes the start and end of the list
            m_pWaitingNodeStart = new PathFinderNode();
            m_pWaitingNodeStart->pFinder = pFinder;
            m_pWaitingNodeEnd = m_pWaitingNodeStart;
        }

        m_WaitingNodes++;
    }
    pthread_mutex_unlock(m_pWaitListMutex);
}

PathFinderMaster::PathFinderNode* PathFinderMaster::popWaitingFinderNode()
{
    PathFinderNode* pReturnNode = NULL;

    pthread_mutex_lock(m_pWaitListMutex);
    //The block is here just to show that this part is inside mutex
    {
        if(m_WaitingNodes > 0 && m_pWaitingNodeStart)
        {
            pReturnNode = m_pWaitingNodeStart;
            m_pWaitingNodeStart = m_pWaitingNodeStart->pNext;
            
            if(m_pWaitingNodeStart)
            {
                m_pWaitingNodeStart->pPrev = NULL;
            }

            m_WaitingNodes--;
        }
    }
    pthread_mutex_unlock(m_pWaitListMutex);

    return pReturnNode;
}


void PathFinderMaster::addPathFinderNode(PathFinder* pFinder)
{
    pthread_mutex_lock(m_pNodeListMutex);
    //The block is here just to show that this part is inside mutex
    {
        if(m_pFinderNodeStart)
        {
            //Add the new node in front of the list
            PathFinderNode* pNode = new PathFinderNode();
            pNode->pFinder = pFinder;
            
            pNode->pNext = m_pFinderNodeStart;
            m_pFinderNodeStart->pPrev = pNode;

            //Set new one as first
            m_pFinderNodeStart = pNode;                                    
        }
        else
        {
            //This one becomes the start of the list
            m_pFinderNodeStart = new PathFinderNode();
            m_pFinderNodeStart->pFinder = pFinder;
        }

        m_FinderNodes++;
    }
    pthread_mutex_unlock(m_pNodeListMutex);
}

PathFinderMaster::PathFinderNode* PathFinderMaster::deletePathFinderNode(PathFinderNode* pNode)
{
    //Return the following node (might be NULL)
    PathFinderNode* pReturnNode = pNode->pNext;

    pthread_mutex_lock(m_pNodeListMutex);
    //The block is here just to show that this part is inside mutex
    {
        //Fix starting pointer if the current start node is being removed        
        if(pNode == m_pFinderNodeStart)
        {
            m_pFinderNodeStart = m_pFinderNodeStart->pNext;
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
        delete pNode;
        pNode = NULL;
        m_FinderNodes--;
    }
    pthread_mutex_unlock(m_pNodeListMutex);

    return pReturnNode;
}

void PathFinderMaster::clearLists()
{
    //Clear running list
    PathFinderNode* pCurrent = m_pFinderNodeStart;
    while(pCurrent != NULL)
    {
        pCurrent->pFinder->cancel();
        //Remove PathFinder
        delete pCurrent->pFinder;
        pCurrent->pFinder = NULL;

        //Remove PathFinderNode
        pCurrent = deletePathFinderNode(pCurrent);
    }

    //Clear waiting list
    pCurrent = popWaitingFinderNode();
    while(pCurrent != NULL)
    {
        pCurrent->pFinder->cancel();
        delete pCurrent->pFinder;
        delete pCurrent;
        
        pCurrent = popWaitingFinderNode();
    }
}

void PathFinderMaster::buildCanPathArray(unsigned short x, unsigned short y)
{
    unsigned short size = Terrain::getInstance()->getSize();
    unsigned short blockSize = size / CANPATH_BLOCKS;

    //Clear old canPath-array, if exists
    if(m_ppCanPath != NULL)
    {
        for(int i = 0; i < CANPATH_BLOCKS; i++)
        {
            delete [] m_ppCanPath[i];
            m_ppCanPath[i] = NULL;
        }
        delete [] m_ppCanPath;
        m_ppCanPath = NULL;
    }

    //Build canPath-array
    //TODO: CANPATH_BLOCKS * CANPATH_BLOCKS
    m_ppCanPath = new bool*[CANPATH_BLOCKS];
    for(int i = 0; i < CANPATH_BLOCKS; i++)
    {
        m_ppCanPath[i] = new bool[CANPATH_BLOCKS];
        memset(m_ppCanPath[i], 0, CANPATH_BLOCKS);
    }

    //Build map-array
    bool** pp_MapArray = new bool*[size];
    for(int i = 0; i < size; i++)
    {
        pp_MapArray[i] = new bool[size];
    }
    

    //Perform floodfilling from given point  NOTE: Goal must be passable
    PathFinder* pFinder = new PathFinder(x, y, x+1, y, 1);

    pFinder->prepareForExecution();
    pFinder->floodFill(x, y);

    IPathFinder::PathNode*** ppp_NodeArray = pFinder->getNodeArray();

    //Go through data, even a single floodfilled square means that
    //block is marked pathable. Smaller blocksize means that more
    //unnecessary searches can be culled, but the memory requirement
    //grows exponentially  (mapsize / canpath-blocksize)^4
    for(int i = 0; i < CANPATH_BLOCKS; i++)
    {
        for(int j = 0; j < CANPATH_BLOCKS; j++)
        {
            unsigned short startX = j * blockSize;
            unsigned short startY = i * blockSize;
            //Scan block for floodfilled data
            for(int y = 0; y < blockSize; y++)
            {
                for(int x = 0; x < blockSize; x++)
                {
                    if(ppp_NodeArray[startY + y][startX + x] != NULL)
                    {
                        //SHOULD BE NODE_CLOSED
                        if(ppp_NodeArray[startY + y][startX + x]->state != IPathFinder::NODE_CLOSED)
                        {
                            int k = 1; //BREAKPOINT
                        }
                        m_ppCanPath[i][j] = true;
                    }
                    else
                    {
                        int k = 1;
                    }
                }
            }
        }
    }

    //Clean up
    if(pFinder->getPathAgent())
    {
        delete pFinder->getPathAgent();
    }
    delete pFinder;

    for(int i = 0; i < size; i++)
    {
        delete [] pp_MapArray[i];
    }    
    delete [] pp_MapArray;

    /*for(int i = 0; i < CANPATH_BLOCKS; i++)
    {
        for(int j = 0; j < CANPATH_BLOCKS; j++)
        {
            char msg[10];
            sprintf(msg, "%d", m_ppCanPath[i][j]);
            ::OutputDebugStringA(msg);
        }
        ::OutputDebugStringA("\r\n");
    }*/
    
}