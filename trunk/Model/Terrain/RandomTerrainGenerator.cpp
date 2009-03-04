#include "RandomTerrainGenerator.h"
#include "Terrain.h"
#include <stdlib.h>

RandomTerrainGenerator::RandomTerrainGenerator(const unsigned int seed, const unsigned short size)
{
    m_Size = size;
    m_Seed = seed;
}


RandomTerrainGenerator::~RandomTerrainGenerator()
{
    
}


void RandomTerrainGenerator::generateHeightmap(unsigned char** ppVertexHeightData, const unsigned short terrainSize)
{
    // sanity check - if the terrain did not respect our preferred size
    if(terrainSize != m_Size + 1)
    {
        // @TODO: actually random generator can still generate based on this,
        // this is just to demonstate action on generators that need certain
        // size like the saved terrain generator
        return;
    }

    for(int y = 0; y < m_Size; y++)
    {
        for(int x = 0; x < m_Size; x++)
        {
            ppVertexHeightData[y][x] = + y + Terrain::DEFAULT_WATERLEVEL;
        }
    }
    //return;

    //TODO: uncomment
    //srand((unsigned int)m_Seed);

    for(int y = 0; y < m_Size; y++)
    {
        for(int x = 0; x < m_Size; x++)
        {        
            ppVertexHeightData[y][x] =  rand() % 255;//100 + ((rand() % 20)-10);
        }
    }

    Terrain::getInstance()->smoothMap(10);

    //Create mountains
    int passes = rand() % 50;
    for(int i = 0; i < passes; i++)
    {
        //Select a random location for the mountain
        int x = rand() % terrainSize;
        int y = rand() % terrainSize;

        //For rivers
        int base = 0;
        //For mountains
        if(rand() % 2 == 0)
            base = 192;

        int diff = (rand() %  64) + 20;


        //Select a random size for the mountain/river
        int mountSize = (rand() % 61) + 10;

        //Do steps starting from the chosen random x, y -location
        for(int j = 0; j < mountSize; j++)
        {
            //Move the current location randomly between -1...1 tiles in both axis
            x += -1 + rand()%3;
            y += -1 + rand()%3;

            //Bounds checks
            if(x >= 0 && x < terrainSize)
            {    
                
                if(y >= 0 && y < terrainSize)
                {
                    //Change the heights in the location to 192...255
                    ppVertexHeightData[y][x] = base + rand() % diff;
                }
            }
        }
    }

    Terrain::getInstance()->smoothMap(1);

    //comparison points
    ppVertexHeightData[0][m_Size-2] = 255;
    ppVertexHeightData[0][m_Size-1] = 0;
}
