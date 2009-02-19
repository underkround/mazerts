/**
 * PathFinderMaster
 *
 * Master-thread handling the updating of PathFinders
 * NOTE: The PathAgent returned by findPath is NOT destroyed by the PathFinderMaster
 * The responsibility of destroying the instance is left for the user of pathdata
 * (actual path-data is destroyed along with the PathAgent)
 * $Revision$
 * $Date$
 * $Id$
 */

//TODO: MAX_PATHFINDER_AMOUNT, Queuing

#ifndef __PATHFINDERMASTER_H__
#define __PATHFINDERMASTER_H__

#include <pthread.h>

#include "../Common/PThread.h"
#include "PathFinder.h"
#include "PathAgent.h"

class PathFinderMaster : public PThread
{
public:

    struct PathFinderNode
    {
        PathFinderNode()
        {
            pFinder = NULL;
            pPrev = NULL;
            pNext = NULL;            
        }

        /**
         * Pointer to the pathfinder in this node
         */
        PathFinder* pFinder;

        /**
         * Previous node
         */
        PathFinderNode* pPrev;

        /**
         * Next node
         */
        PathFinderNode* pNext;

        int DEBUG_id;
    };
    static int DEBUG_idCounter;

    //TODO: raise these, left low for testing
    /**
     * How many steps in total per loop all the pathfinders can take
     */
    static const int STEPS_PER_LOOP = 10000;

    /**
     * Minimum amount of steps the pathfinders take
     */         
    static const int MINIMUM_STEPS = 100;

    /**
     * Returns the singleton instance
     * @return Pointer to PathFinderMaster-instance
     */
    inline static PathFinderMaster* getInstance()
    {
        return pInstance;
    }

    /**
     * Destructor
     */
    virtual ~PathFinderMaster();

    /**
     * Starts a new PathFinder to search for path between the points.
     * @param x Start X-tilecoordinate
     * @param y Start Y-tilecoordinate
     * @param goalX Goal X-tilecoordinate
     * @param goalY Goal Y-tilecoordinate
     * @param size Size of the path (size * size tiles)
     * @return PathAgent-instance, that can be used to query for path searching state and 
               pathdata once it's found   NOTE: The caller of this method must take care 
               of destroying the PathAgent! Path data will be destroyed along with the agent
     */
    static PathAgent* findPath(unsigned short x, unsigned short y, unsigned short goalX, unsigned short goalY, unsigned char size);

    /**
     * Cancels all current PathFinders and destroys them
     * PathAgents will be left behind with their state indicator set to IPathFinder::CANCELLED
     */
    static void cancelAll();


    /**
     * Thread entry-point
     */
    void run();

    /**
     * Starts the thread
     */
    inline void start()
    { 
        if(!m_Running)
        {
            m_Running = true;
            PThread::start(pInstance);            
        }        
    }

    /**
     * Tells the thread to stop executing and terminate
     */
    inline void stop()
    {
        m_Running = false;        
    }

    /**
     * Tells if the thread is running
     */
    inline bool isRunning()
    {
        return m_Running;
    }

private:

    /**
     * Default constructor
     */
    PathFinderMaster();
    
    /**
     * Adds a new pathfinder
     * @param pFinder Pointer to PathFinder to add
     */
    void addPathFinderNode(PathFinder* pFinder);

    /**
     * Removes PathFinderNode from list and deletes it
     * @param pNode Node to remove
     * @return The next node after the removed one (so list-iteration
                doesn't go haywire)
     */
    PathFinderNode* deletePathFinderNode(PathFinderNode* pNode);

    /**
     * Static reference to the singleton instance
     */
    static PathFinderMaster* pInstance;
    
    /**
     * First node in the PathFinderNode-list
     */
    PathFinderNode* m_pNodeStart;

    /**
     * Number of nodes in list
     */
    int m_Nodes;

    /**
     * Mutex for the PathFinderNode-list
     */
    pthread_mutex_t* m_pNodeListMutex;

    /**
     * Tells if the thread is running or not
     */
    bool m_Running;
};


#endif // __PATHFINDERMASTER_H__