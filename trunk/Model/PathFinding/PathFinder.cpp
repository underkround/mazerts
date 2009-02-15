#include "PathFinder.h"

PathFinder::PathFinder(Unit* pUnit, short goalX, short goalY)
{
    m_pTerrain = &Terrain::getInstance();
    m_pUnit = pUnit;

    //Set start and goal positions
    m_StartX = (short)pUnit->getPosition()->x;
    m_StartY = (short)pUnit->getPosition()->y;

    m_GoalX = goalX;
    m_GoalY = goalY;

    //Create needed arrays
    unsigned short size = m_pTerrain->getSize();
    
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

PathFinder::~PathFinder()
{
    delete m_pOpenList;

    unsigned short size = m_pTerrain->getSize();

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


bool PathFinder::advance(short steps)
{
    //If the search has been cancelled, return true so the 
    //pathfinding-master thread knows to destroy this instance,
    //but do not pass the pathdata to unit
    if(m_Cancelled)
    {
        return true;
    }

    short currentY = 0;
    short currentX = 0;
    short mapSize = m_pTerrain->getSize();

    while(steps)
    {
        if(!m_pOpenList->IsEmpty())
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

            for(int i = -1; i < 2; i++)
            {
                for(int j = -1; j < 2; j++)
                {
#ifdef EIGHTWAYSEARCH
                    if(!(i == 0 && j == 0))
#else
                    if(i == 0 || j == 0)
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
                            return true;
                        }

                        //The node must not be in the closed list it has to be passable
                        if( m_pTerrain->isPassable(adjaX, adjaY) && !m_pppClosedArray[adjaY][adjaX] )
                        {
                            //If the node hasn't been in the open list yet, add it there
                            if(!m_ppInOpenList[adjaY][adjaX])
                            {
                                //Calculate costs
                                G = m_pTerrain->getMoveCost(currentX, currentY, i, j);
                                H = abs(m_GoalX - adjaX) + abs(m_GoalY - adjaY);                            
                                F = current->G + G + H;

                                //Add to list and mark InOpenList
                                m_pOpenList->Insert(new PathNode(adjaX, adjaY,
                                F, G, H,
                                current), F);
                                m_ppInOpenList[adjaY][adjaX] = true;
                            }
                            else
                            {
                                //TODO:
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
            //TODO: BUILD PATH, INFORM UNIT
            return true;
        }

        --steps;
    }

    //Search isn't done yet
    return false;
}

void PathFinder::cancel()
{
    m_Cancelled = true;
}

#include <stdio.h>
//TODO: DEBUG-implementation
void PathFinder::buildPath(PathNode* pCurrent)
{    
    while(pCurrent != NULL)
    {        
        printf("\n%d:%d", pCurrent->x, pCurrent->y);
        pCurrent = pCurrent->pParent;
    }    
}