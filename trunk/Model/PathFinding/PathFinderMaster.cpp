#include "PathFinderMaster.h"

PathFinderMaster* PathFinderMaster::pInstance = new PathFinderMaster();

#include <stdio.h>
void PathFinderMaster::run()
{
    for(int i=0;i<20;i++)
        printf("HELLO FROM PATHFINDER-MASTER -THREAD\n");
}