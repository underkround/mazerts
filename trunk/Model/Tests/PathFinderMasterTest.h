#include "../PathFinding/PathFinderMaster.h"
#include <stdio.h>

void testPathFinderMaster()
{
    PathFinderMaster* pMaster = PathFinderMaster::getInstance();

    pMaster->start();
    for(int i = 0; i < 20; i++)
    {
        printf("HELLO FROM MAIN-THREAD\n");
    }

    printf("DONE\n");
}