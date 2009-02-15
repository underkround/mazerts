#include "../Terrain/Terrain.h"

#include <stdio.h>
#include <assert.h>

//Uncomment for stress-testing
//#define TERRAINSTRESSTEST

#include <iostream>
using namespace std;

void testTerrain()
{

    Terrain* terrain = Terrain::getInstance(); // by default, terrain is flat with default size
    cout << "fresh terrain size: " << terrain->getSize() << "\n";

    const unsigned char * * ppMap = (const unsigned char **) terrain->getTerrainHeightData();

    for(int i = 0; i < terrain->getSize(); i++)
    {
        for(int j = 0; j < terrain->getSize(); j++)
        {
            assert(ppMap[i][j] == Terrain::DEFAULT_FLATHEIGHT);
        }
    }


    // Initialize the terrain with size 1000*1000
    Terrain::getInstance()->initialize(1000);
    cout << "terrain size: " << Terrain::getInstance()->getSize() << "\n";

    assert(terrain->getSize() == 1000);
    assert(terrain->getWaterLevel() == Terrain::DEFAULT_WATERLEVEL);
    terrain->setWaterLevel(56);
    assert(terrain->getWaterLevel() == 56);


    //Stress-test: is all memory freed?
#ifdef TERRAINSTRESSTEST

    terrain->initialize(5000);
    // insert breakpoint here and check the memory usage, should be 50+M
    terrain->release();
    // insert breakpoint here and check the memory usage, should be dropped
    terrain->initialize(5000);
    // insert breakpoint here and check the memory usage, should be 50+M
    terrain->release();
    // insert breakpoint here and check the memory usage, should be dropped

/*
    for(int i = 0; i < 10; i++)
    {
        terrain = Terrain::createNew(100, 10000);
        printf("%d\n", i);
    }*/

#endif


    terrain = Terrain::getInstance();

    //Alter heights
    terrain->setTerrainVertexHeight(8, 9, 25);    
    terrain->setTerrainVertexHeight(8, 10, 25);
    terrain->setTerrainVertexHeight(9, 9, 25);    
    terrain->setTerrainVertexHeight(9, 10, 25);
    terrain->setTerrainVertexHeight(10, 9, 35);
    terrain->setTerrainVertexHeight(10, 10, 35);

    //Print out surrounding tileheights    
    unsigned char value = terrain->getTerrainHeight(8, 9);
    //printf("%d, %d : %d  ", 8, 9, value);
    printf("%d, %d : %d  ", 9, 9,   terrain->getTerrainHeight(9, 9));
    printf("%d, %d : %d\n", 10, 9,  terrain->getTerrainHeight(10, 9));
    printf("%d, %d : %d  ", 8, 10,  terrain->getTerrainHeight(8, 10));
    printf("%d, %d : %d  ", 9, 10,  terrain->getTerrainHeight(9, 10));
    printf("%d, %d : %d\n", 10, 10, terrain->getTerrainHeight(10, 10));

    printf("MoveCost 8,9 -> 8,10: %d\n", terrain->getMoveCost(8,9, 0, 1));
    printf("MoveCost 8,9 -> 9,9: %d\n",  terrain->getMoveCost(8,9, 1, 0));
    printf("MoveCost 9,9 -> 8,9: %d\n",  terrain->getMoveCost(9,9, -1, 0));

    assert(terrain->getMoveCost(8,9, 0, 1) == Terrain::MOVE_ILLEGAL);
    assert(terrain->getMoveCost(0, 0, -1, -1) == Terrain::MOVE_OUTOFBOUNDS);
    assert(terrain->getMoveCost(terrain->getSize(), 0, 1, 0) == Terrain::MOVE_OUTOFBOUNDS);
    assert(terrain->getMoveCost(0, terrain->getSize(), 0, 1) == Terrain::MOVE_OUTOFBOUNDS);
    assert(terrain->getMoveCost(terrain->getSize(), terrain->getSize(), 1, 0) == Terrain::MOVE_OUTOFBOUNDS);
    assert(terrain->getMoveCost(terrain->getSize(), terrain->getSize(), 0, 1) == Terrain::MOVE_OUTOFBOUNDS);

    terrain->release();
    
    // initialize with default values
    terrain->initialize(Terrain::DEFAULT_MAPSIZE);

    ppMap = (const unsigned char * *)terrain->getTerrainHeightData();
    printf("\nMEM: %d\n", (int)ppMap[4]);

    // check heights from vertex-data
    ppMap = (const unsigned char * *)terrain->getTerrainVertexHeightData();

    for(int i = 0; i < terrain->getSize() + 1; i++)
    {
        for(int j = 0; j < terrain->getSize() + 1; j++)
        {            
            assert(ppMap[i][j] == Terrain::DEFAULT_FLATHEIGHT);
        }
    } 

    for(int i = 0; i < terrain->getSize() + 1; i++)
    {
        for(int j = 0; j < terrain->getSize() + 1; j++)
        {            
            terrain->setTerrainVertexHeight(i, j, j);
        }
    }
    
    ppMap = (const unsigned char * *)terrain->getTerrainVertexHeightData();

    for(int i = 0; i < terrain->getSize() + 1; i++)
    {
        for(int j = 0; j < terrain->getSize() + 1; j++)
        {
            assert(ppMap[j][i] == j);
        }
    }

}