/**
 * PathAgent
 *
 * A mediator that handles path-data transfer between PathFinders in 
 * separate thread and the component that asked for path
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __PATHAGENT_H__
#define __PATHAGENT_H__

#include <pthread.h>
#include "IPathFinder.h"

class PathAgent
{
public:
    
    /**
     * Constructor
     * @param pFinder Pointer to IPathFinder-instance this agent is associated with
     */
    PathAgent(IPathFinder* pFinder)
    {
        m_pFinder = pFinder;
        m_pNode = NULL;
        m_State = IPathFinder::NOT_FINISHED;
        m_pMutex = new pthread_mutex_t;
        pthread_mutex_init(m_pMutex, NULL);
    }

    /**
     * Destructor
     */
    virtual ~PathAgent()
    {
        cancel();
        destroyPath();
        pthread_mutex_destroy(m_pMutex);
        delete m_pMutex;
        m_pMutex = NULL;
    }

    /**
     * Returns the state of the pathfinding, see enumeration in IPathFinder
     * @return The current state of the pathfinding
     */
    IPathFinder::PathingState getState()
    {
        //pthread_mutex_lock(m_pMutex);
        {
            
        }
        //pthread_mutex_unlock(m_pMutex); 
        return m_State;
    }

    /**
     * Returns the pathdata, if any
     * @return Pointer to starting node of the path, or NULL if path is not
     *          (yet) found.
     */
    IPathFinder::PathNode* getPathData()
    {
        IPathFinder::PathNode* pNode = NULL;
        pthread_mutex_lock(m_pMutex);
        {
            if(m_pNode)
            {
                pNode = m_pNode;
            }
        }
        pthread_mutex_unlock(m_pMutex);
        
        return pNode;
    }

    /**
     * Sets the current state of the pathfinding
     * @param state State to set, see enumeration in IPathFinder
     */
    inline void setState(IPathFinder::PathingState state)
    {
        pthread_mutex_lock(m_pMutex);
        {
            m_State = state;
        }
        pthread_mutex_unlock(m_pMutex);
    }

    /** 
     * Returns the next pathnode, deleting the old one
     * @return PathNode-pointer
     */
    inline IPathFinder::PathNode* getNextPathNode()
    {
        IPathFinder::PathNode* pNode = NULL;
        pthread_mutex_lock(m_pMutex);
        {
            if(m_pNode)
            {                
                pNode = m_pNode;

                if(m_pNode->pParent != NULL)
                {
                    delete m_pNode->pParent;
                    m_pNode->pParent = NULL;
                }

                if(m_pNode->pChild != NULL)
                {
                    m_pNode = m_pNode->pChild;
                }
                else
                {
                    delete m_pNode;
                    m_pNode = NULL;
                }
            }
        }
        pthread_mutex_unlock(m_pMutex);
        
        return pNode;
    }

    /**
     * Sets the pathdata-pointer safely
     * @param pNode Pointer to PathNode in the beginning of the path
     */
    inline void setPathData(IPathFinder::PathNode* pNode)
    {
        pthread_mutex_lock(m_pMutex);
        {
            m_pNode = pNode;
        }
        pthread_mutex_unlock(m_pMutex);
    }

    inline void cancel()
    {        
        {        
            if(m_State == IPathFinder::NOT_FINISHED)
            {
                if(m_pFinder)
                {
                    m_pFinder->cancel();
                }
            }
        }     
    }
       
    /**
     * Locks the mutex from outside of class
     */
    inline void lock()
    {
        pthread_mutex_lock(m_pMutex);
    }

    /**
     * Unlocks the mutex from outside of class
     */
    inline void unlock()
    {
        pthread_mutex_unlock(m_pMutex);
    }

private:

    /**
     * Destroys the pathdata (if any)
     */
    void destroyPath()
    {
        if(m_pNode)
        {
            IPathFinder::PathNode* pCurrent = m_pNode;
            while(pCurrent != NULL)
            {
                if(pCurrent->pParent)
                {
                    delete pCurrent->pParent;
                }
                
                if(pCurrent->pChild)
                {
                    pCurrent = pCurrent->pChild;
                }
                else
                {
                    delete pCurrent;
                    break;
                }
            }
        }
    }

    /**
     * Mutual exclusion for pathdata
     */
    pthread_mutex_t* m_pMutex;

    /**
     * Current state of the pathfinding
     */
    IPathFinder::PathingState m_State;
    
    /**
     * The starting node of the path, or NULL if not (yet) found
     */
    IPathFinder::PathNode* m_pNode;

    /**
     * IPathFinder-instance this agent is associated with
     */
    IPathFinder* m_pFinder;
};

#endif //__PATHAGENT_H