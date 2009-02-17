#include "PathFinder.h"

PathFinder::PathFinder(Unit* pUnit, short goalX, short goalY)
{
    //TODO: Remove when not needed anymore
    DEBUG_steps = 0;


    pStartNode = NULL;

    m_pUnit = pUnit;

    //Set start and goal positions
    m_StartX = (short)pUnit->getPosition()->x;
    m_StartY = (short)pUnit->getPosition()->y;

    m_GoalX = goalX;
    m_GoalY = goalY;

    //If goal is unpassable, cancel the search right away
    if(!Terrain::getInstance()->isPassable(goalX, goalY))
    {
        m_Cancelled = true;
    }
    
    //Create needed arrays
    unsigned short size = Terrain::getInstance()->getSize();
        
    //TODO: Initial size?
    m_pOpenList = new CHeapTree<PathNode*, int>(size);

    m_pppClosedArray = new PathNode**[size];
    m_ppInOpenList = new bool*[size];
    for(int i = 0; i < size; i++)
    {
        m_pppClosedArray[i] = new PathNode*[size];
        m_ppInOpenList[i] = new bool[size];
    }    
    
    //Clear lists
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            m_pppClosedArray[i][j] = NULL;
            m_ppInOpenList[i][j] = false;
        }
    }


    //Push starting tile to open list
    m_pOpenList->Insert(new PathNode(m_StartX, m_StartY,
        0, 0, 0,
        NULL), 0);
}

#include <stdio.h>
PathFinder::~PathFinder()
{
    //Empty and delete openlist -heaptree   
    if(!m_pOpenList->IsEmpty())
    {
        /*PathNode* pNode = NULL;
        m_pOpenList->GetTopID(&pNode);
        m_pOpenList->RemoveTop();
        delete pNode;*/

        int size = m_pOpenList->GetSize();
        CHeapTree<PathNode*, int>::_NODE* nodes = m_pOpenList->getDataArray();
        for(int i = 0; i < size; ++i)
        {
            delete nodes[i].id;
        }
    }

    delete m_pOpenList;

    unsigned short size = Terrain::getInstance()->getSize();

    //Remove pathnodes
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            if(m_pppClosedArray[i][j] != NULL)
            {
                delete m_pppClosedArray[i][j];
            }
        }
    }

    //Destroy arrays
    for(int i = 0; i < size; i++)
    {
        delete [] m_pppClosedArray[i];
        delete [] m_ppInOpenList[i];
    }
    delete [] m_ppInOpenList;
    delete [] m_pppClosedArray;
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
            m_pppClosedArray[currentY][currentX] = current;

            //Push adjacent into openlist
            short adjaX = 0;
            short adjaY = 0;
            int F, G, H;
            
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
                            current = new PathNode(adjaX, adjaY, 0, 0, 0, current);
                            //Path found!
                            buildPath(current);
                            //TODO: INFORM UNIT
                            
                            return FOUND;
                        }

                        //The node must not be in the closed list and it has to be passable
                        if( Terrain::getInstance()->isPassable(adjaX, adjaY) && !m_pppClosedArray[adjaY][adjaX] )
                        {
                            //If the node hasn't been in the open list yet, add it there
                            if(!m_ppInOpenList[adjaY][adjaX])
                            {
                                //TODO OPT: getMoveCost? (Maybe not)
                                //Calculate costs                         
                                short cost = Terrain::getInstance()->getMoveCost(currentX, currentY, i, j);
                                G = current->G + cost;
       
                                //TODO: Better heuristics?
                                short distX = abs(adjaX - m_GoalX);
                                short distY = abs(adjaY - m_GoalY);
                                //H = (distX + distY) << HEURISTIC_FACTOR;
                                int diag = (distX < distY) ? distX : distY;
                                int straight = (distX + distY);
                                H = 25 * diag + 18 * straight - 2 * diag;
                                
                                /*                                  
                                short xDist = abs(adjaX-m_GoalX);
                                short yDist = abs(adjaY-m_GoalY);
                                H = ((xDist > yDist) ? xDist : yDist) << HEURISTIC_FACTOR;
                                */
                                F = G + H;

                                //Add to list and mark InOpenList
                                m_pOpenList->Insert(new PathNode(adjaX, adjaY,
                                F, G, H,
                                current), F);
                                m_ppInOpenList[adjaY][adjaX] = true;
                            }
                            //else
                            {                               
                                //TODO: (MAY NOT BE NEEDED)
                                /*If it is on the open list already, check to see if this path to that square is better, 
                                using G cost as the measure. A lower G cost means that this is a better path. 
                                If so, change the parent of the square to the current square, and recalculate the G and F
                                scores of the square. If you are keeping your open list sorted by F score, you may need 
                                to resort the list to account for the change.*/
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
        //Remove pointer from m_pppClosedArray, so the actual path nodes won't
        //get deleted along with the array (the ones used by the unit)
        m_pppClosedArray[pCurrent->y][pCurrent->x] = NULL;
        
        if(pCurrent->pParent != NULL)
        {
            pCurrent->pParent->pChild = pCurrent;        
            pCurrent = pCurrent->pParent;
        }
        else
        {            
            pStartNode = pCurrent;
            return;
        }        
    }    
}