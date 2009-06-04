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
    };

    /**
     * How many steps per loop a pathfinder can take
     */
    static const int STEPS_PER_LOOP = 10000;

    /**
     * Maximum amount of pathfinders running simultaneously (rest will be in the waiting list)
     */         
    static const int MAX_FINDERS_RUNNING = 25;

    /**
     * Blocks per side for canPath-array, the resulting array will be CANPATH_BLOCKS^4 bytes
     */
    static const int CANPATH_BLOCKS = 64;

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
     * @param priorize If set to true, the find-request will be placed first in queue
     * @return PathAgent-instance, that can be used to query for path searching state and 
               pathdata once it's found   NOTE: The caller of this method must take care 
               of destroying the PathAgent! Path data will be destroyed along with the agent
     */
    static PathAgent* findPath(unsigned short x, unsigned short y, unsigned short goalX, unsigned short goalY, unsigned char size, bool priorize = false);

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
     * Clears running- and waiting lists
     */
    void clearLists();

    /**
     * Tells if the thread is running
     */
    static inline bool isRunning()
    {
        return pInstance->m_Running;
    }

    /**
     * Returns the amount of running pathfinders
     * @return Amount of running pathfinders
     */
    static inline int getRunningAmount()
    {
        return pInstance->m_FinderNodes;
    }

    /**
     * Returns the amount of pathfinders waiting for execution
     * @return Amount of waiting pathfinders
     */
    static inline int getWaitingAmount()
    {
        return pInstance->m_WaitingNodes;
    }

    /**
     * Performs a flood filling from given point (usually some player
     * start location) to mark non-pathable positions in to CanPath-
     * array
     * @param x Start X-coordinate
     * @param y Start Y-coordinate
     */
    void buildCanPathArray(unsigned short x, unsigned short y);

private:

    /**
     * Default constructor
     */
    PathFinderMaster();
    
    /**
     * Pushes a new pathfinder to the end of waiting list
     * @param pFinder Pointer to PathFinder to add
     */
    void pushWaitingFinderNode(PathFinder* pFinder);
    
    /**
     * Pushes a new pathfinder to the start of waiting list
     * @param pFinder Pointer to PathFinder to add
     */
    void priorizeWaitingFinderNode(PathFinder* pFinder);

    /**
     * Pops a pathfindernode from the start of waiting list
     * @return PathFinderNode-pointer, or NULL if list is empty
     */
    PathFinderNode* popWaitingFinderNode();

    /**
     * Adds a new pathfinder to running list
     * @param pFinder Pointer to PathFinder to add
     */
    void addPathFinderNode(PathFinder* pFinder);

    /**
     * Removes PathFinderNode from running list and deletes it
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
     * First node in the running PathFinderNode-list
     */
    PathFinderNode* m_pFinderNodeStart;

    /**
     * Number of running nodes in list
     */
    int m_FinderNodes;

    /**
     * First node in the waiting PathFinderNode-list
     */
    PathFinderNode* m_pWaitingNodeStart;

   /**
    * Last node in the waiting PathFinderNode-list
    */
    PathFinderNode* m_pWaitingNodeEnd;

    /**
     * Number of nodes waiting for execution
     */
    int m_WaitingNodes;

    /**
     * Mutex for the running PathFinderNode-list
     */
    pthread_mutex_t* m_pNodeListMutex;

    /**
     * Mutex for the waiting PathFinderNode-list
     */
    pthread_mutex_t* m_pWaitListMutex;

    /**
     * Tells if the thread is running or not
     */
    bool m_Running;

    /**
     * Simple array for quick checks if there even can exist a path between two points
     */
    bool** m_ppCanPath;
    
};


#endif // __PATHFINDERMASTER_H__