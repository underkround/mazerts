#include "AntinTerrainGenerator.h"
#include "Terrain.h"
#include <stdlib.h>

AntinTerrainGenerator::AntinTerrainGenerator(const unsigned int seed, const unsigned short size)
{
    m_Size = size;
    m_Seed = seed;
}


AntinTerrainGenerator::~AntinTerrainGenerator()
{
    
}


void AntinTerrainGenerator::generateHeightmap(unsigned char** ppVertexHeightData, const unsigned short terrainSize)
{
    makeFlat(ppVertexHeightData, terrainSize, Terrain::DEFAULT_FLATHEIGHT);
    faultLines(ppVertexHeightData, terrainSize, 200, 5);

    Terrain::getInstance()->smoothMap(1);
}

void AntinTerrainGenerator::makeFlat(unsigned char **ppVertexHeightData, const unsigned short terrainSize, const unsigned short height)
{
    for(int y = 0; y < m_Size; y++)
    {
        for(int x = 0; x < m_Size; x++)
        {
            ppVertexHeightData[y][x] = height;
        }
    }
}

void AntinTerrainGenerator::faultLines(unsigned char **ppVertexHeightData, const unsigned short terrainSize, int flCount, int flMod)
{
    int x0 = 0;
    int y0 = 0;
    int x1 = 0;
    int y1 = 0;
    for(int i = 0; i < flCount; ++i)
    {
        x0 = rand() % terrainSize;
        y0 = rand() % terrainSize;
        x1 = rand() % terrainSize;
        y1 = rand() % terrainSize;
        for(int x = 0; x < terrainSize-1; ++x)
        {
            for(int y = 0; y < terrainSize-1; ++y)
            {
                if( (x1-x0)*(x-y0)-(y1-y0)*(y-x0) > 0)
                {
                    ppVertexHeightData[y][x] = ((ppVertexHeightData[y][x]+flMod) < 256) ? ppVertexHeightData[y][x] += flMod : ppVertexHeightData[y][x] = 255;
                }
                else
                {
                    ppVertexHeightData[y][x] = ((ppVertexHeightData[y][x]-flMod) > 0) ? ppVertexHeightData[y][x] -= flMod : ppVertexHeightData[y][x] = 0;
                }
            }
        }
        
    }
}

void AntinTerrainGenerator::makeHills(unsigned char **ppVertexHeightData, const unsigned short terrainSize, int cirCount, int cirRad, int cirHeight)
{
    //TODO: kukkula-algoritmi
}