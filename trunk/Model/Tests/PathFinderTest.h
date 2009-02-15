#include "../Terrain/Terrain.h"
#include "../Unit/Unit.h"
#include "../Pathfinding/PathFinder.h"
#include <stdio.h>

void testPathFinder()
{
    Terrain::getInstance();
    Unit* pUnit = new Unit();

    PathFinder* pFinder = new PathFinder(pUnit, 100, 100);

    while(!pFinder->advance(10))
    {        
    }

    printf("DONE!");
}