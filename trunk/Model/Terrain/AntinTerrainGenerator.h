/**
 * AntinTerrainGenerator
 *
 * $Revision: 31 $
 * $Date$
 * $Id$
 */

#pragma once

#include "ITerrainGenerator.h"
 
class AntinTerrainGenerator : public ITerrainGenerator
{
public:

    /**
     * Default constructor
     */
    AntinTerrainGenerator(const unsigned int seed, const unsigned short size);

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
     * Our preferred size
     */
    unsigned short      m_Size;

    /**
     * Our seed
     */
    unsigned int        m_Seed;

};
