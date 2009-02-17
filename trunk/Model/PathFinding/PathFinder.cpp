#include "PathFinder.h"

PathFinder::PathFinder(Unit* pUnit, short goalX, short goalY)
{
    //TODO: Remove when not needed anymore
    DEBUG_steps = 0;


    m_pStartNode = NULL;

    m_pUnit = pUnit;

    m_Size = pUnit->getWidth();

    //Set start and goal positions
    m_StartX = (short)pUnit->getPosition()->x;
    m_StartY = (short)pUnit->getPosition()->y;

    m_GoalX = goalX;
    m_GoalY = goalY;

    //If goal is unpassable, cancel the search right away
    if(!Terrain::getInstance()->isPassable(goalX, goalY, m_Size))
    {
        m_Cancelled = true;
    }
    
    //Create needed arrays
    unsigned short size = Terrain::getInstance()->getSize();
        
    //TODO: Initial size?
    m_pOpenList = new CHeapTree<PathNode*, unsigned int>(10000);

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
}

PathFinder::~PathFinder()
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


IPathFinder::PathingState PathFinder::advance(short steps)
{
    //If the search has been cancelled, return true so the 
    //pathfinding-master thread knows to destroy this instance,
    //but do not pass the pathdata to unit
    if(m_Cancelled)
    {
        return CANCELLED;
    }

    short currentY = 0;
    short currentX = 0;
    short mapSize = Terrain::getInstance()->getSize();

    while(steps)
    {
        DEBUG_steps++;

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
                            addNode(adjaX, adjaY, 0, 0, NODE_OPEN, current);                            
                            //Path found!
                            buildPath(current);
                            //TODO: INFORM UNIT
                            
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
            //TODO: INFORM UNIT
            return NOT_FOUND;
        }

        --steps;
    }

    //Search isn't done yet
    return NOT_FINISHED;
}

void PathFinder::cancel()
{
    m_Cancelled = true;
}

//TODO: If path is searched in reverse (goal and start swapped), this becomes much simpler
void PathFinder::buildPath(PathNode* pCurrent)
{    
    while(1)
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
    }    
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