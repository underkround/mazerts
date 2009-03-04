/**
 * IPathFinder
 *
 * Interface for time-slicing pathfinders
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __IPATHFINDER_H__
#define __IPATHFINDER_H__

#ifndef NULL
    #define NULL 0
#endif

//FORWARD DECLARATIONS
class PathAgent;

class IPathFinder
{
public:

    /**
     * States for PathFinder, returned by advance()
     */
    enum PathingState
    {
        NOT_FINISHED = 0,
        NOT_FOUND,
        CANCELLED,
        FOUND,
        __END = 0xFF
    };

    /**
     * States for pathnodes
     */
    enum NodeState
    {             
        NODE_OPEN,
        NODE_CLOSED
    };

    /**
     * Nodes making up the path and stored into open- and closed list
     * during search
     */
    struct PathNode
    {

        PathNode(short x, short y, int G, NodeState state, PathNode* pParent)
        {
            this->x = x;
            this->y = y;            
            this->G = G;
            this->state = state;
            this->pParent = pParent;
            this->pChild = NULL;
        }

        /**
         * Node tile x-position
         */
        unsigned short x;

        /**
         * Node tile y-position
         */
        unsigned short y;

        /**
         * Node G-cost
         */
        unsigned int G;

        /**
         * Node state
         */
        NodeState state;

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
     * Default constructor
     */
    IPathFinder()
    {
        m_State = NOT_FINISHED;
    }

    /**
     * Destructor
     */
    virtual ~IPathFinder()
    {
    }

    /**
     * Called just prior to starting the search, prepares the pathfinder for running
     */
    virtual void prepareForExecution() = 0;

    /**
     * Advances the pathfinding by n steps and returns boolean to tell if the search
     * has ended or not
     * @param steps How many steps to advance
     * @return Current search state, see PathingState-enumeration
     */
    virtual PathingState advance(short steps) = 0;

    /**
     * Cancels the search
     */
    virtual void cancel() = 0;


protected:

    /**
     * Size of the path, ie. how many tiles must be passable in widht and height
     */
    unsigned char m_Size;

    /**
     * Starting tile-coordinates for the search
     */
    unsigned short m_StartX;
    unsigned short m_StartY;

    /**
     * Goal tile-coordinates for the search
     */
    unsigned short m_GoalX;
    unsigned short m_GoalY;

    /**
     * Tells the current state of the search
     */
    PathingState m_State;

};

#endif //__IPATHFINDER_H__