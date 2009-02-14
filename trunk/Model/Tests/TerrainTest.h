#include "../Terrain/ITerrain.h"
#include "../Terrain/Terrain.h"

#include <stdio.h>
#include <assert.h>

void testTerrain()
{

    Terrain& terrain = Terrain::getInstance();

    const unsigned char* const * ppMap = terrain.getTerrainHeightData();

    /*for(int i = 0; i < terrain.getSize(); i++)
    {
        for(int j = 0; j < terrain.getSize(); j++)
        {
            assert(ppMap[i][j] == Terrain::DEFAULT_FLATHEIGHT);
        }
    }*/

    //Release the terrain
    Terrain::release();

    //Next getInstance call automatically creates new instance
    assert(&terrain != (Terrain*)&Terrain::getInstance());

    //Forcing creation of new terrain
    terrain = Terrain::createNew(100, 1000);
    assert(terrain.getSize() == 1000);
    assert(terrain.getSeed() == 100);
    assert(terrain.getWaterLevel() == Terrain::DEFAULT_WATERLEVEL);
    terrain.setWaterLevel(56);
    assert(terrain.getWaterLevel() == 56);
    

    //Stress-test: is all memory freed?
    /*for(int i = 0; i < 10; i++)
    {
        Terrain::createNew(100, 10000);
        printf("%d\n", i);
    }*/

    //Alter heights
    terrain.setTerrainVertexHeight(9, 10, 25);
    terrain.setTerrainVertexHeight(10, 10, 10);

    //Print out surrounding tileheights
    printf("%d, %d : %d  ", 8, 9, terrain.getTerrainHeight(8, 9));
    printf("%d, %d : %d  ", 9, 9, terrain.getTerrainHeight(9, 9));
    printf("%d, %d : %d\n", 10, 9, terrain.getTerrainHeight(10, 9));
    printf("%d, %d : %d  ", 8, 10, terrain.getTerrainHeight(8, 10));
    printf("%d, %d : %d  ", 9, 10, terrain.getTerrainHeight(9, 10));
    printf("%d, %d : %d\n", 10, 10, terrain.getTerrainHeight(10, 10));

    printf("MoveCost 8,9 -> 8,10: %d\n", terrain.getMoveCost(8,9, 0, 1));
    printf("MoveCost 8,9 -> 9,9: %d\n", terrain.getMoveCost(8,9, 1, 0));
    printf("MoveCost 10,9 -> 9,9: %d\n", terrain.getMoveCost(10,9, -1, 0));
    
    
    Terrain::release();

    //Get instance with default values
    terrain = Terrain::getInstance();

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

    for(int i = 0; i < terrain.getSize() + 1; i++)
    {
        for(int j = 0; j < terrain.getSize() + 1; j++)
        {
            assert(ppMap[j][i] == j);
        }
    }
    
}