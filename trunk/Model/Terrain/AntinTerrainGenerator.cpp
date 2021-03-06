#include "AntinTerrainGenerator.h"
#include "Terrain.h"
#include <stdlib.h>
#include <cmath>
#include <ctime>

#ifndef PI
#define PI 3.141592653589793238462f
#endif

AntinTerrainGenerator::AntinTerrainGenerator(const unsigned int seed, const unsigned short size)
{
    m_Size = size;
    m_Seed = seed;
}


AntinTerrainGenerator::~AntinTerrainGenerator()
{
    m_height.clear();
    m_posx.clear();
    m_posy.clear();
}

void AntinTerrainGenerator::generateHeightmap(unsigned char** ppVertexHeightData, const unsigned short terrainSize)
{
    if(m_Seed == 0) srand((unsigned int)time(0));
    else srand(m_Seed);

    //PERUNAPELTOGENERAATTORI aka. just testing algorithms - NOT an actual way to make good terrain!
    makeFlat(ppVertexHeightData, terrainSize, Terrain::DEFAULT_FLATHEIGHT);

    //some bigger gaps
    faultLines(ppVertexHeightData, terrainSize, 4, 15);

    /*for(int i=8;i<120;i+=8)
    {
        makeHills(ppVertexHeightData, terrainSize, 2, i, i/2);
    }*/
    //invertTerrain(ppVertexHeightData, terrainSize);

    //some nice random terrain
    faultLines(ppVertexHeightData, terrainSize, 1000, 2);

    //"sweet spots"
    for(int i=0;i<20;++i)
    {
        flattenArea(    ppVertexHeightData,
                        terrainSize,
                        Terrain::DEFAULT_FLATHEIGHT + rand() % 100,
                        -1,
                        -1,
                        20,
                        45);
    }

    //"starting areas"
    if(m_height.size() > 0)
    {
        for(unsigned int i=0; i<m_height.size(); ++i)
        {
            flattenArea(ppVertexHeightData, terrainSize, m_height.at(i), m_posx.at(i), m_posy.at(i), 50, 100);
        }
    }

    //"minmax volcano"
    flattenArea(ppVertexHeightData, terrainSize, 255, 50, 100, 15, 20);
    flattenArea(ppVertexHeightData, terrainSize, 0, 50, 100, 10, 0);

    Terrain::getInstance()->smoothMap(1);
}

void AntinTerrainGenerator::makeFlat(unsigned char **ppVertexHeightData, const unsigned short terrainSize, const unsigned short height)
{
    for(int y = 0; y < m_Size; y++)
    {
        for(int x = 0; x < m_Size; x++)
        {
            ppVertexHeightData[y][x] = (unsigned char)height;
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
        for(int x = 0; x < terrainSize; ++x)
        {
            for(int y = 0; y < terrainSize; ++y)
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

void AntinTerrainGenerator::makeHills(  unsigned char **ppVertexHeightData,
                                        const unsigned short terrainSize,
                                        int cirCount,
                                        int cirRad,
                                        int cirHeight)
{
    int x0 = 0;
    int y0 = 0;
    int cir2 = cirRad*cirRad; //lol@^2
    for(int i = 0;i<cirCount;++i)
    {
        x0 = rand() % terrainSize;
        y0 = rand() % terrainSize;
        for(int x = 0; x < terrainSize; ++x)
        {
            for(int y = 0; y < terrainSize; ++y)
            {
                int z = (x0-x)*(x0-x)+(y0-y)*(y0-y);
                if(z < cir2)
                {
                    int heightMod = (int)((cirHeight/2)*(1+cos(PI*z/cir2)));
                    ppVertexHeightData[y][x] = ((ppVertexHeightData[y][x]+heightMod) < 256) ? ppVertexHeightData[y][x] += heightMod : ppVertexHeightData[y][x] = 255;
                }
            }
        }
    }
}

void AntinTerrainGenerator::invertTerrain(unsigned char **ppVertexHeightData, const unsigned short terrainSize)
{
    for(int y = 0; y < terrainSize; ++y)
    {
        for(int x = 0; x < terrainSize; ++x)
        {
            ppVertexHeightData[y][x] = 255-ppVertexHeightData[y][x];
        }
    }
}

int AntinTerrainGenerator::calculateAverageHeight(  unsigned char **ppVertexHeightData,
                                                    const unsigned short terrainSize,
                                                    int xCenter,
                                                    int yCenter,
                                                    int radius)
{
    int total = 0;
    int i = 0;
    int radius2 = radius*radius;
    for(int y = 0; y < terrainSize; ++y)
    {
        for(int x = 0; x < terrainSize; ++x)
        {
            int z = (xCenter-x)*(xCenter-x)+(yCenter-y)*(yCenter-y);
            if(z < radius2)
            {
                total += ppVertexHeightData[y][x];
                ++i;
            }
        }
    }
    return total/i;
}

void AntinTerrainGenerator::flattenArea(    unsigned char **ppVertexHeightData,
                                            const unsigned short terrainSize,
                                            int height,
                                            int xCenter,
                                            int yCenter,
                                            int innerRadius,
                                            int outerRadius)
{
    if(height < 0) height = rand() % 255;
    if(xCenter < 0) xCenter = rand() % terrainSize;
    if(yCenter < 0) yCenter = rand() % terrainSize;
    int innerCir2 = innerRadius*innerRadius;
    int outerCir2 = outerRadius*outerRadius;
    float donutWidth = (float)(outerRadius-innerRadius);
    for(int x = 0; x < terrainSize; ++x)
    {
        for(int y = 0; y < terrainSize; ++y)
        {
            int z = (xCenter-x)*(xCenter-x)+(yCenter-y)*(yCenter-y);
            if(z < innerCir2)
            {
                ppVertexHeightData[y][x] = height;
            } else if(z < outerCir2 && innerRadius < outerRadius) {
                float step = (float)(sqrt((double)z) - sqrt((double)innerCir2));
                //float stepSize = (float)abs(height - ppVertexHeightData[y][x]) / (float)donutWidth;
                float ratio = (1/donutWidth)*step;
                ppVertexHeightData[y][x] = (unsigned char)(ppVertexHeightData[y][x]*ratio + height*(1-ratio));
            }
        }
    }
}

void AntinTerrainGenerator::makeIsland(unsigned char **ppVertexHeightData, const unsigned short terrainSize, int borders)
{
    int x0 = terrainSize/2;
    int y0 = terrainSize/2;
    int cir2 = (x0-borders)*(y0-borders);
    
    for(int x = 0; x < terrainSize; ++x)
    {
        for(int y = 0; y < terrainSize; ++y)
        {
            int z = (x0-x)*(x0-x)+(y0-y)*(y0-y);
            if(z > cir2)
            {
                float heightdrop = (float)(sqrt((double)z) - sqrt((double)cir2))+(ppVertexHeightData[y][x]*0.8f);
                ppVertexHeightData[y][x] = (ppVertexHeightData[y][x]-heightdrop > 0) ? ppVertexHeightData[y][x] -= (unsigned char)(heightdrop) : 0;
            }
        }
    }
}
