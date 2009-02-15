#include "../Terrain/Terrain.h"

#include <stdio.h>
#include <assert.h>

//Uncomment for stress-testing
//#define TERRAINSTRESSTEST

void testTerrain()
{

    Terrain& terrain = Terrain::getInstance();

    const unsigned char* const * ppMap = terrain.getTerrainHeightData();

    for(int i = 0; i < terrain.getSize(); i++)
    {
        for(int j = 0; j < terrain.getSize(); j++)
        {
            assert(ppMap[i][j] == Terrain::DEFAULT_FLATHEIGHT);
        }
    }

    //Release the terrain
    Terrain::release();

    //Forcing creation of new terrain
    terrain = Terrain::createNew(100, 1000);
    assert(terrain.getSize() == 1000);
    assert(terrain.getSeed() == 100);
    assert(terrain.getWaterLevel() == Terrain::DEFAULT_WATERLEVEL);
    terrain.setWaterLevel(56);
    assert(terrain.getWaterLevel() == 56);
    

    //Stress-test: is all memory freed?
#ifdef TERRAINSTRESSTEST
    for(int i = 0; i < 10; i++)
    {
        terrain = Terrain::createNew(100, 10000);
        printf("%d\n", i);
    }
#endif
    
    terrain = Terrain::getInstance();

    //Alter heights
    terrain.setTerrainVertexHeight(8, 9, 25);    
    terrain.setTerrainVertexHeight(8, 10, 25);
    terrain.setTerrainVertexHeight(9, 9, 25);    
    terrain.setTerrainVertexHeight(9, 10, 25);
    terrain.setTerrainVertexHeight(10, 9, 35);
    terrain.setTerrainVertexHeight(10, 10, 35);

    //Print out surrounding tileheights    
    unsigned char value = terrain.getTerrainHeight(8, 9);
    //printf("%d, %d : %d  ", 8, 9, value);
    /*printf("%d, %d : %d  ", 9, 9, terrain.getTerrainHeight(9, 9));
    printf("%d, %d : %d\n", 10, 9, terrain.getTerrainHeight(10, 9));
    printf("%d, %d : %d  ", 8, 10, terrain.getTerrainHeight(8, 10));
    printf("%d, %d : %d  ", 9, 10, terrain.getTerrainHeight(9, 10));
    printf("%d, %d : %d\n", 10, 10, terrain.getTerrainHeight(10, 10));

    printf("MoveCost 8,9 -> 8,10: %d\n", terrain.getMoveCost(8,9, 0, 1));
    printf("MoveCost 8,9 -> 9,9: %d\n", terrain.getMoveCost(8,9, 1, 0));
    printf("MoveCost 9,9 -> 8,9: %d\n", terrain.getMoveCost(9,9, -1, 0));*/

    assert(terrain.getMoveCost(8,9, 0, 1) == Terrain::MOVE_ILLEGAL);
    assert(terrain.getMoveCost(0, 0, -1, -1) == Terrain::MOVE_OUTOFBOUNDS);
    assert(terrain.getMoveCost(terrain.getSize(), 0, 1, 0) == Terrain::MOVE_OUTOFBOUNDS);
    assert(terrain.getMoveCost(0, terrain.getSize(), 0, 1) == Terrain::MOVE_OUTOFBOUNDS);
    assert(terrain.getMoveCost(terrain.getSize(), terrain.getSize(), 1, 0) == Terrain::MOVE_OUTOFBOUNDS);
    assert(terrain.getMoveCost(terrain.getSize(), terrain.getSize(), 0, 1) == Terrain::MOVE_OUTOFBOUNDS);

    Terrain::release();
    
    //Get instance with default values
    terrain = Terrain::createNew(100, Terrain::DEFAULT_MAPSIZE);

    ppMap = terrain.getTerrainHeightData();
    printf("\nMEM: %d\n", (int)ppMap[4]);

    //Check heights from vertex-data
    ppMap = terrain.getTerrainVertexHeightData();

    for(int i = 0; i < terrain.getSize() + 1; i++)
    {
        for(int j = 0; j < terrain.getSize() + 1; j++)
        {            
            assert(ppMap[i][j] == Terrain::DEFAULT_FLATHEIGHT);
        }
    } 

    for(int i = 0; i < terrain.getSize() + 1; i++)
    {
        for(int j = 0; j < terrain.getSize() + 1; j++)
        {            
            terrain.setTerrainVertexHeight(i, j, j);
        }
    }
    
    ppMap = terrain.getTerrainVertexHeightData();

    for(int i = 0; i < terrain.getSize() + 1; i++)
    {
        for(int j = 0; j < terrain.getSize() + 1; j++)
        {
            assert(ppMap[j][i] == j);
        }
    }
}