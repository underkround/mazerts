#include "RandomTerrainGenerator.h"

RandomTerrainGenerator::RandomTerrainGenerator(const unsigned int seed, const unsigned short size)
{
    m_Size = size;
    m_Seed = seed;
}


RandomTerrainGenerator::~RandomTerrainGenerator()
{
    //
}


void RandomTerrainGenerator::generateHeightmap(unsigned char** ppVertexHeightData, const unsigned short terrainSize)
{
    // sanity check - if the terrain did not respect our preferred size
    if(terrainSize != m_Size)
    {
        // @TODO: actually random generator can still generate based on this,
        // this is just to demonstate action on generators that need certain
        // size like the saved terrain generator
        return;
    }

    // @TODO: modify ppVertexHeightData
    //srand((unsigned int)m_Seed);
}
