#include "PathFinder.h"

/*PathFinder::PathFinder(Unit* pUnit, unsigned short goalX, unsigned short goalY)
{
    m_Size = pUnit->getWidth();

    //Set start and goal positions
    m_StartX = (short)pUnit->getPosition()->x;
    m_StartY = (short)pUnit->getPosition()->y;

    initialize(goalX, goalY);
}*/

PathFinder::PathFinder(unsigned short x, unsigned short y, unsigned short goalX, unsigned short goalY, unsigned char size)
{
    m_StartX = x;
    m_StartY = y;
    m_Size = size;
    m_pStartNode = NULL;

    initialize(goalX, goalY);
}

void PathFinder::initialize(unsigned short goalX, unsigned short goalY)
{

    m_pPathAgent = NULL;

    m_Initialized = false;
    m_Prepared = false;

    //CHECKS FOR THINGS THAT PREVENT INITIALIZATION (Or just make it unnecessary)
    if(!Terrain::getInstance()->isPassable(goalX, goalY, m_Size))
    {
        m_State = NOT_FOUND;
        return;
    }

    if(goalX == m_StartX && goalY == m_StartY)
    {
        m_State = NOT_FOUND;
        return;
    }


    //TODO: Remove when not needed anymore
    //DEBUG_steps = 0;

    m_pStartNode = NULL;

    m_GoalX = goalX;
    m_GoalY = goalY;
    
   
    //Agent initialization
    m_pPathAgent = new PathAgent(this);

    m_Initialized = true;
}

PathFinder::~PathFinder()
{
    //If the pathfinder was prepared, destroy it
    if(m_Prepared)
    {
        //Delete heaptree
        delete m_pOpenList;

        unsigned short size = Terrain::getInstance()->getSize();

        //Remove pathnodes
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < size; j++)
            {
                if(m_pppNodeArray[i][j] != NULL)
                {
                    delete m_pppNodeArray[i][j];                
                }
            }
        }

        //Destroy arrays
        for(int i = 0; i < size; i++)
        {
            delete [] m_pppNodeArray[i];
            delete [] m_ppInOpenList[i];        
        }    
        delete [] m_pppNodeArray;
        delete [] m_ppInOpenList;
    }
}

void PathFinder::prepareForExecution()
{
    //Create needed arrays
    unsigned short size = Terrain::getInstance()->getSize();
        
    //TODO: Initial size?
    m_pOpenList = new CHeapTree<PathNode*, unsigned int>(3000);

    m_pppNodeArray = new PathNode**[size];
    m_ppInOpenList = new bool*[size];
    //m_pppOpenArray = new PathNode**[size];

    for(int i = 0; i < size; i++)
    {
        m_pppNodeArray[i] = new PathNode*[size];
        m_ppInOpenList[i] = new bool[size];
    }    
    
    //Clear lists
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            m_pppNodeArray[i][j] = NULL;
            m_ppInOpenList[i][j] = false;            
        }
    }

    //Push starting tile to open list
    addNode(m_StartX, m_StartY, 0, 0, NODE_OPEN, NULL);

    m_Prepared = true;
}


IPathFinder::PathingState PathFinder::advance(short steps)
{
    //If the search has been cancelled, return true so the 
    //pathfinding-master thread knows to destroy this instance,
    //but do not pass the pathdata to unit
    if(m_State != NOT_FINISHED)
    {        
        return m_State;
    }

    short currentY = 0;
    short currentX = 0;
    short mapSize = Terrain::getInstance()->getSize();

    while(steps)
    {
        //DEBUG_steps++;

        if(m_pOpenList->GetSize())
        {
            //Get pathnode with lowest F-score from openlist
            PathNode* current = NULL;
            m_pOpenList->GetTopID(&current);
            m_pOpenList->RemoveTop();

            currentY = current->y;
            currentX = current->x;

            //Mark as removed from open, insert it into closed
            m_ppInOpenList[currentY][currentX] = false;
            m_pppNodeArray[currentY][currentX]->state = NODE_CLOSED;

            //Push adjacent into openlist
            unsigned short adjaX = 0;
            unsigned short adjaY = 0;
            unsigned int F, G, H;
            
            //TODO OPT: Separate loop contents into method, call repeatedly ("loop unrolling")
            for(int i = -1; i < 2; i++)
            {
                for(int j = -1; j < 2; j++)
                {
#ifdef EIGHTWAYSEARCH
                    if(!(i == 0 && j == 0))
#else
                    if((i == 0 || j == 0) && (!(i == 0 && j == 0))) //Is this correct?
#endif
                    {
                        adjaX = currentX+i;
                        adjaY = currentY+j;

                        //Check if path was found
                        if(adjaX == m_GoalX && adjaY == m_GoalY)
                        {
                            //Path found! Add last node, package and send away to the agent (if it still
                            //exists, this is a special case, as a cancel-call might have come from the
                            //other thread during this advance, and the m_pPathAgent would be NULL then
                            if(m_pPathAgent)
                            {
                                //Lock the pathfinder while path is being built, this is to ensure
                                //that the pathagent isn't cancelled and destroyed while building
                                //path (this has happened once, and the chance of it happening is very,
                                //very improbable, but better be safe than sorry
                                m_pPathAgent->lock();
                                {
                                    current = addNode(adjaX, adjaY, 0, 0, NODE_OPEN, current);                                                        
                                    buildPath(current); 
                                }
                                m_pPathAgent->unlock();
                                m_pPathAgent->setPathData(m_pStartNode);
                                setState(FOUND);                            
                            }

                            return FOUND;
                        }

                        //The node must not be in the closed list and it has to be passable
                        if( Terrain::getInstance()->isPassable(adjaX, adjaY, m_Size) && (!m_pppNodeArray[adjaY][adjaX] || m_pppNodeArray[adjaY][adjaX]->state != NODE_CLOSED) )
                        {
                            //TODO OPT: getMoveCost? (Maybe not)
                            //Calculate costs                         
                            short cost = Terrain::getInstance()->getMoveCost(currentX, currentY, i, j);
                            G = current->G + cost;
                            H = heuristic(adjaX, adjaY);

                            //If the node hasn't been in the open list yet, add it there
                            if(!m_ppInOpenList[adjaY][adjaX])                            
                            {                                
                                F = G + H;

                                //Add to list and mark InOpenList
                                addNode(adjaX, adjaY, F, G, NODE_OPEN, current);
                                m_ppInOpenList[adjaY][adjaX] = true;                                
                            }
                            else if(m_pppNodeArray[adjaY][adjaX]->G > G)
                            {
                                //Node is in the openlist, but has higher G-cost, 
                                //switch parent, recalculate costs and reorder open list
                                m_pppNodeArray[adjaY][adjaX]->pParent = current;
                                m_pppNodeArray[adjaY][adjaX]->G = G;
                                m_pOpenList->ResetData(m_pppNodeArray[adjaY][adjaX], G+H);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            //Openlist is empty, no path
            setState(NOT_FOUND);
            return NOT_FOUND;
        }

        --steps;
    }

    //Search isn't done yet    
    return NOT_FINISHED;
}

//TODO: If path is searched in reverse (goal and start swapped), this becomes much simpler
void PathFinder::buildPath(PathNode* pCurrent)
{    

#define sgn(a) (a > 0) ? 1 : (a < 0) ? -1 : 0

    
    PathNode* pRealPath = new PathNode(pCurrent->x, pCurrent->y, pCurrent->G, NODE_CLOSED, NULL);    
    pRealPath->pChild = NULL;

    //Keep track of direction
    bool dirChange = true;
    char oldxDir = sgn(pCurrent->pParent->x -  pCurrent->x);
    short oldyDir = sgn(pCurrent->pParent->y -  pCurrent->y);   

    while(pCurrent->pParent != NULL)
    {
        char xDir = sgn(pCurrent->pParent->x -  pCurrent->x);
        char yDir = sgn(pCurrent->pParent->y -  pCurrent->y);
   
        //Only store points where the x or y direction changes
        if(oldxDir != xDir || oldyDir != yDir)
        {           
            pRealPath->pParent = new PathNode(pCurrent->x, pCurrent->y, pCurrent->G, NODE_CLOSED, NULL);
            pRealPath->pParent->pChild = pRealPath;
            pRealPath = pRealPath->pParent;

            oldxDir = xDir;
            oldyDir = yDir;
        }
         
        pCurrent->pParent->pChild = pCurrent;        
        pCurrent = pCurrent->pParent;
    }    

    //Now set the REAL start node
    m_pStartNode = pRealPath;

    /*while(1)
    {        
        //Remove pointer from m_pppNodeArray, so the actual path nodes won't
        //get deleted along with the array (the ones used by the unit)
        m_pppNodeArray[pCurrent->y][pCurrent->x] = NULL;
        
        if(pCurrent->pParent != NULL)
        {
            pCurrent->pParent->pChild = pCurrent;        
            pCurrent = pCurrent->pParent;
        }
        else
        {            
            m_pStartNode = pCurrent;
            return;
        }        
    }*/
}


IPathFinder::PathNode* PathFinder::addNode(unsigned short x, unsigned short y, int F, int G, NodeState state, PathNode* pParent)
{
    PathNode* pNode = new PathNode(x, y, G, state, pParent);
    m_pOpenList->Insert(pNode, F);
    m_pppNodeArray[y][x] = pNode;
    return pNode;
}

int PathFinder::heuristic(unsigned short x, unsigned short y)
{
    int value = 0;
    
#define HEURISTIC_FACTOR 11

    //TODO: Better heuristics?
    short distX = abs(x - m_GoalX);
    short distY = abs(y - m_GoalY);
    //value = (distX + distY) << HEURISTIC_FACTOR;
    value = ((unsigned int)sqrt((float)distX*distX + distY*distY)) << HEURISTIC_FACTOR;

    /*int diag = (distX < distY) ? distX : distY;
    int straight = (distX + distY);
    value = (int)(Terrain::MOVECOST_MIN * 0.95f) * diag + (int)((Terrain::MOVECOST_MIN * 0.95f) / 1.4f) * straight - 2 * diag;*/

    /*                                  
    short xDist = abs(adjaX-m_GoalX);
    short yDist = abs(adjaY-m_GoalY);
    H = ((xDist > yDist) ? xDist : yDist) << HEURISTIC_FACTOR;
    */

    return value;
}