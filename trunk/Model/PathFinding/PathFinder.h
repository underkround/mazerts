/**
 * PathFinder
 *
 * Stepping A* -pathfinder
 *
 * $Revision$
 * $Date$
 * $Id$
 */

//TODO: canPath + path between two arbitrary points to interface, unit-callback?,
// Master-thread, taking unit-size into account, bi-directional search?, reverse search?

#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include "IPathFinder.h"
#include "../Terrain/Terrain.h"
#include "../Unit/Unit.h"
#include "../Common/HeapTree.h"
#include "../Common/Vector3.h"

//Comment to use 4-way search
#define EIGHTWAYSEARCH

class PathFinder : public IPathFinder
{
public:

    //TODO: remove when not needed anymore
    int DEBUG_steps;

    /**
     * Constructor
     * @param pUnit Unit that needs the path
     * @param goalX X-coordinate of the goal tile
     * @param goalY Y-coordinate of the goal tile
     */
    PathFinder(Unit* pUnit, short goalX, short goalY);

    /**
     * Destructor
     */
    virtual ~PathFinder();

    /**
     * Advances the search a given number of steps
     * @param steps Steps to advance
     * @return PathingState, see enumeration
     */
    PathingState advance(short steps);

    /**
     * Cancels the search and prepares pathfinder for destruction
     */
    void cancel();

    inline PathNode* getPath()
    {
        return pStartNode;
    }

private:

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

public:  // <-- TODO: poista
    /**
     * Two-dimensional PathNode-array to use for closed list and cleaning up
     * unused PathNodes when the path is completed
     */
    PathNode*** m_pppNodeArray;

    /**
     * This is the node which will be delivered to unit if path is found
     */
    PathNode* pStartNode;

};

#endif //__PATHFINDER_H__