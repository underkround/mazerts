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
}


PathFinderMaster::~PathFinderMaster()
{
    //Clear running list
    PathFinderNode* pCurrent = m_pFinderNodeStart;
    while(pCurrent != NULL)
    {
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
        delete pCurrent->pFinder;
        delete pCurrent;
        
        pCurrent = popWaitingFinderNode();
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
        if(m_FinderNodes < MAX_FINDERS_RUNNING && m_WaitingNodes)
        {
            PathFinderNode* pNode = popWaitingFinderNode();
            
            if(pNode != NULL)
            {
                addPathFinderNode(pNode->pFinder);
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
        msleep(15);
    }

    PathFinderMaster::~PathFinderMaster();
}

//Static
PathAgent* PathFinderMaster::findPath(unsigned short x, unsigned short y, unsigned short goalX, unsigned short goalY, unsigned char size)
{
    
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