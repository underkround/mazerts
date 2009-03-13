/**
 * AntinTerrainGenerator
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __ANTINTERRAINGENERATOR_H__
#define __ANTINTERRAINGENERATOR_H__

#include "ITerrainGenerator.h"
 
class AntinTerrainGenerator : public ITerrainGenerator
{
public:

    /**
     * Default constructor
     */
    AntinTerrainGenerator(const unsigned int seed, const unsigned short size, const bool islands);

    /**
     * Destructor
     */
    virtual ~AntinTerrainGenerator();

    /**
     * Generates a new terrain heightmap based on the seed
     * @param ppVertexHeightData    Pointer to a two-dimensional array holding
     *                              the vertexheightdata of the terrain
     * @param terrainSize           The current size of the terrain
     */
    virtual void generateHeightmap(unsigned char** ppVertexHeightData, const unsigned short terrainSize);

    /**
     * @return our preferred size for the terrain
     */
    inline const unsigned short getPreferredSize()
    {
        return m_Size;
    }

    /**
     * Get the RandomGenerator's seed
     * @return the random seed
     */
    inline const unsigned int getSeed()
    {
        return m_Seed;
    }

private:
    /*** own helper methods ***/

    /**
     * makeFlat
     * flattens terrain totally
     * oh yeah there is Terrain::flattenMap didn't notice it
     * @param ppVertexHeightData    height data
     * @param terrainSize           terrain height/width
     * @param height                height to set to all map squares
     */
    void makeFlat(unsigned char** ppVertexHeightData, const unsigned short terrainSize, const unsigned short height);
    /**
     * makes fault lines (makes nice terrain by itself if you make a lot of tiny ones)
     * @param ppVertexHeightData    height data
     * @param terrainSize           terrain height/width
     * @param flCount               amount of fault lines to create
     * @param flMod                 height of a single fault line
     */
    void faultLines(unsigned char **ppVertexHeightData, const unsigned short terrainSize, int flCount, int flMod);
    /**
     * makes some perfectly circular hills to random locations
     * @param ppVertexHeightData    height data
     * @param terrainSize           terrain height/width
     * @param cirCount              amount of hills
     * @param cirRad                radius of a single hill
     * @param cirHeight             height of a single hill
     */
    void makeHills(unsigned char **ppVertexHeightData, const unsigned short terrainSize, int cirCount, int cirRad, int cirHeight);

    /**
     * invertTerrain
     * flips terrain around
     */
    void invertTerrain(unsigned char **ppVertexHeightData, const unsigned short terrainSize);

    /**
     * calculateAverageHeight
     * calculates average height of specified area (circle!)
     * @param xCenter               center point of area to be flattened
     * @param yCenter               center point of area to be flattened
     * @param radius                radius of area
     * @return                      average height of area
     */
    int calculateAverageHeight(unsigned char **ppVertexHeightData, const unsigned short terrainSize, int xCenter, int yCenter, int radius);

    /**
     * flattenArea
     * makes circular area flat
     * @param ppVertexHeightData    height data
     * @param terrainSize           terrain height/width
     * @param height                height to flatten to, use with calculateAverageHeight for nice results!
     * @param xCenter               center point of area to be flattened, use negative value for random location
     * @param yCenter               center point of area to be flattened, use negative value for random location
     * @param innerRadius           area that becomes totally flat
     * @param outerRadius           area that is only flattened by a linear percentage
     *                              if this is smaller than inner radius it is disabled
     */
    void flattenArea(unsigned char **ppVertexHeightData, const unsigned short terrainSize, int height, int xCenter, int yCenter, int innerRadius, int outerRadius);

    /**
     * makeIsland
     * lowers the borders of map to make it an island
     */
    void makeIsland(unsigned char **ppVertexHeightData, const unsigned short terrainSize, int borders);

    /**
     * Our preferred size
     */
    unsigned short      m_Size;

    /**
     * Our seed
     */
    unsigned int        m_Seed;

    // islands or continent?
    bool                m_Islands;

};

#endif // __ANTINTERRAINGENERATOR_H__
