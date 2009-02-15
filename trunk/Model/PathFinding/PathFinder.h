/**
 * PathFinder
 *
 * Stepping A* -pathfinder
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include "IPathFinder.h"
#include "../Terrain/Terrain.h"
#include "../Unit/Unit.h"
#include "../Common/HeapTree.h"
#include "../Common/Vector3.h"

//Comment to use 4-way search
#define EIGHTWAYSEARCH

class PathFinder : IPathFinder
{
public:

    /**
     * Nodes stored into openlist
     */
    struct PathNode
    {
        PathNode(short x, short y, int F, int G, int H, PathNode* pParent)
        {
            this->x = x;
            this->y = y;
            this->F = F;
            this->G = G;
            this->H = H;
            this->pParent = pParent;
            this->pChild = NULL;
        }

        /**
         * Node tile x-position
         */
        short x;

        /**
         * Node tile y-position
         */
        short y;

        /**
         * Node F-cost
         */
        int F;
         
        /**
         * Node G-cost
         */
        int G;

        /**
         * Node H-cost
         */
        int H;

        /**
         * Node parent
         */
        PathNode* pParent;

        /**
         * Node child
         */
        PathNode* pChild;

    };


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
     * @return true, if search has finished, otherwise false
     */
    bool advance(short steps);

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
     * Use only the parametric constructor
     */
    PathFinder()
    {
        throw "DO NOT USE THE DEFAULT CONSTRUCTOR FOR PATHFINDER! Thank you";
    }

    /**
     * Builds the path after it is found
     * @param pEndNode PathNode containing the goal
     */
    void buildPath(PathNode* pEndNode);


    /**
     * Heaptree (binary heap)-pointer to use for openlist
     * ID is the PathNode-instance, data is F-cost
     */
    CHeapTree<PathNode*, int>* m_pOpenList;

    /**
     * Two-dimensional boolean array for quickly checking if some tile is 
     * in the openlist
     * First dimension is Y, second X
     */
    bool** m_ppInOpenList;

    /**
     * Two-dimensional PathNode-array to use for closed list
     */
    PathNode*** m_pppClosedArray;

    /**
     * This is the node that will be delivered to unit if path is found
     */
    PathNode* pStartNode;
};

#endif //__PATHFINDER_H__