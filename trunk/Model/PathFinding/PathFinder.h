/**
 * PathFinder
 *
 * Stepping A* -pathfinder. NOTE: The PathAgent constructed is _NOT_ 
 * destroyed along with the PathFinder, same goes for found path.
 * The destroying of the PathAgent is left for the user,
 * Agent will take care of destroying the pathdata
 * $Revision$
 * $Date$
 * $Id$
 */

//TODO: bi-directional search?, reverse search?

#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include "IPathFinder.h"
#include "PathAgent.h"
#include "../Terrain/Terrain.h"
#include "../Common/HeapTree.h"

//Comment to use 4-way search
#define EIGHTWAYSEARCH

class PathFinder : public IPathFinder
{
public:

    /**
     * Constructor
     * @param x X-coordinate of the start tile
     * @param y Y-coordinate of the start tile
     * @param goalX X-coordinate of the goal tile
     * @param goalY Y-coordinate of the goal tile
     * @param size Size of the path (size * size tiles)
     */
    PathFinder(unsigned short x, unsigned short y, unsigned short goalX, unsigned short goalY, unsigned char size);

    /**
     * Destructor
     */
    virtual ~PathFinder();

    /**
     * Prepares the pathfinder to execution (the memory allocation
     * of big arrays is left until here, last moment before the
     * actual path searching starts)
     */
    virtual void prepareForExecution();

    /**
     * Advances the search a given number of steps
     * @param steps Steps to advance
     * @return PathingState, see enumeration
     */
    PathingState advance(short steps);

    /**
     * Returns the X-coordinate of the goal tile
     * @param X-coordinate of the goal tile
     */
    inline unsigned short getGoalX()
    {
        return m_GoalX;
    }

    /**
     * Returns the Y-coordinate of the goal tile
     * @param Y-coordinate of the goal tile
     */
    inline unsigned short getGoalY()
    {
        return m_GoalY;
    }

    /**
     * Returns the path-agent that mediates the data
     * from this PathFinder to outside
     * @return Pointer to PathAgent
     */
    inline PathAgent* getPathAgent()
    {
        return m_pPathAgent;
    }

    /**
     * Tells if the constructor call initialized the internal data or not.
     * Usually if the object wasn't initialized, it's because there's no
     * path to search (goal tile is unpassable)
     */
    inline bool isInitialized()
    {
        return m_Initialized;
    }

    /**
     * Sets the PathingState for both agent and this object
     * @param state PathingState to set
     */
    inline void setState(PathingState state)
    {
        m_State = state;
        
        if(m_pPathAgent)
        {
            m_pPathAgent->setState(m_State);
        }
    }

    /**
     * Cancels the search
     */
    inline void cancel()
    {
        setState(IPathFinder::CANCELLED);
        m_pPathAgent = NULL;
    }

    /**
     * Returns the nodearray, for floodfill and testing purposes
     * @return two-dimensional array of PathNode-pointers
     */
    PathNode*** getNodeArray() { return m_pppNodeArray; }

    /**
     * Performs a "floodfill"-search from given map position
     * @param x X-coordinate to start from
     * @param y Y-coordinate to start from
     */
    void floodFill(unsigned short x, unsigned short y);

private:

    /**
     * Default constructor
     */
    PathFinder()
    {
    }

    /**
     * Internal initialize, does common stuff to all public constructos
     * @param goalX X-coordinate of the goal tile
     * @param goalY Y-coordinate of the goal tile
     */
    void initialize(unsigned short goalX, unsigned short goalY);

    /**
     * Heuristic value of the tile
     * @param x X-coordinate of the tile to calculate heuristic for
     * @param y Y-coordinate of the tile to calculate heuristic for
     * @return Heuristic value
     */
    int heuristic(unsigned short x, unsigned short y);

    /**
     * Builds the path after it is found
     * @param pEndNode PathNode containing the goal
     */
    void buildPath(PathNode* pEndNode);

    /**
     * Creates a new pathnode and adds it to array and openlist
     * @param x Tile X-coordinate
     * @param y Tile Y-coordinate
     * @param F F-cost
     * @param G G-cost
     * @param state NodeState the node is in (see enumeration)
     * @param pParent Pointer to parent-node
     * @return Pointer to added PathNode
     */
    PathNode* addNode(unsigned short x, unsigned short y, int F, int G, NodeState state, PathNode* pParent);

    /**
     * Heaptree (binary heap)-pointer to use for openlist
     * ID is the PathNode-instance, data is F-cost
     */
    CHeapTree<PathNode*, unsigned int>* m_pOpenList;

    /**
     * Two-dimensional boolean array for quickly checking if some tile is 
     * in the openlist
     * First dimension is Y, second X
     */
    bool** m_ppInOpenList;

    /**
     * Two-dimensional PathNode-array to use for closed list and cleaning up
     * unused PathNodes when the path is completed
     */
    PathNode*** m_pppNodeArray;

    /**
     * This is the node which will be delivered to unit if path is found
     */
    PathNode* m_pStartNode;

    /**
     * Mediator-agent for pathdata
     */
    PathAgent* m_pPathAgent;

    /**
     * Tells if the pathfinder is initialized
     * This is used to control the destruction
     */
    bool m_Initialized;

    /**
     * Tells if the pathfinder is ready for execution (are the arrays allocated)
     */
    bool m_Prepared;
};

#endif //__PATHFINDER_H__