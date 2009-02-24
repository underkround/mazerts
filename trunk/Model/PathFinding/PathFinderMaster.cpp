#include "PathFinderMaster.h"
#include "../Common/Sleep.h"

PathFinderMaster* PathFinderMaster::pInstance = new PathFinderMaster();

//DEBUG:
int PathFinderMaster::DEBUG_idCounter = 0;

PathFinderMaster::PathFinderMaster()
{
    m_Running = false;
    m_Nodes = 0;

    //Create mutexes
    m_pNodeListMutex = new pthread_mutex_t;
    pthread_mutex_init(m_pNodeListMutex, NULL);    

    m_pNodeStart = NULL;
}


PathFinderMaster::~PathFinderMaster()
{
    PathFinderNode* pCurrent = m_pNodeStart;
    while(pCurrent != NULL)
    {
        //Remove PathFinder
        delete pCurrent->pFinder;
        pCurrent->pFinder = NULL;

        //Remove PathFinderNode
        pCurrent = deletePathFinderNode(pCurrent);
    }

    pthread_mutex_destroy(m_pNodeListMutex);
    delete m_pNodeListMutex;
    m_pNodeListMutex = NULL;
}


void PathFinderMaster::run()
{    
    int steps = 0;

    while(m_Running)
    {
        PathFinderNode* pCurrent = m_pNodeStart;

        //Deadlock, deletePathFinderNode will cause another locking to occur
        //pthread_mutex_lock(m_pNodeListMutex);
        {
            while(pCurrent != NULL)
            {
                //Calculate the steps
                steps = STEPS_PER_LOOP / (m_Nodes+1);
                if(steps < MINIMUM_STEPS)
                {
                    steps = MINIMUM_STEPS;
                }

                //printf("Advancing PathFinderNode #%d towards %d %d\n", pCurrent->DEBUG_id, pCurrent->pFinder->getGoalX(), pCurrent->pFinder->getGoalY());

                IPathFinder::PathingState pState = pCurrent->pFinder->advance(steps);

                if(pState != IPathFinder::NOT_FINISHED)
                {
                    /*printf("PathFinderNode #%d FINISHED\n", pCurrent->DEBUG_id);

                    switch(pState)
                    {
                    case IPathFinder::NOT_FOUND:
                        printf("There seems to be no path...\n");
                        break;
                    case IPathFinder::CANCELLED:
                        printf("The search was cancelled by request\n");
                        break;
                    case IPathFinder::FOUND:
                        printf("Path found!\n");
                        break;
                    }*/
                    
                    //Remove PathFinder
                    delete pCurrent->pFinder;
                    pCurrent->pFinder = NULL;

                    //Remove PathFinderNode
                    pCurrent = deletePathFinderNode(pCurrent);

                    //printf("Nodes left: %d\n", m_Nodes);
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

    PathFinderMaster::~PathFinderMaster();
}

//Static
PathAgent* PathFinderMaster::findPath(unsigned short x, unsigned short y, unsigned short goalX, unsigned short goalY, unsigned char size)
{
    PathFinder* pFinder = new PathFinder(x, y, goalX, goalY, size);
    if(pFinder->isInitialized())
    {
        pInstance->addPathFinderNode(pFinder);
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
        PathFinderNode* pCurrent = pInstance->m_pNodeStart;
        while(pCurrent != NULL)
        {
            pCurrent->pFinder->cancel();
            pCurrent = pCurrent->pNext;            
        }
    }
    pthread_mutex_unlock(pInstance->m_pNodeListMutex);
}


void PathFinderMaster::addPathFinderNode(PathFinder* pFinder)
{
    pthread_mutex_lock(m_pNodeListMutex);
    //The block is here just to show that this part is inside mutex
    {
        if(m_pNodeStart)
        {
            //Add the new node in front of the list
            PathFinderNode* pNode = new PathFinderNode();
            pNode->pFinder = pFinder;
            
            pNode->pNext = m_pNodeStart;
            m_pNodeStart->pPrev = pNode;

            //Set new one as first
            m_pNodeStart = pNode;                                    
        }
        else
        {
            //This one becomes the start of the list
            m_pNodeStart = new PathFinderNode();
            m_pNodeStart->pFinder = pFinder;
        }

        //DEBUG
        m_pNodeStart->DEBUG_id = DEBUG_idCounter++;

        m_Nodes++;
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
        if(pNode == m_pNodeStart)
        {
            //TODO:: Palauttaa tämän? Onko ok?
            m_pNodeStart = m_pNodeStart->pNext;
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
        m_Nodes--;
    }
    pthread_mutex_unlock(m_pNodeListMutex);

    return pReturnNode;
}