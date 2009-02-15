/**
 * RandomTerrainGenerator
 *
 * Default implementation for ITerrainGenerator
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __RANDOMTERRAINGENERATOR_H__
#define __RANDOMTERRAINGENERATOR_H__

#include "ITerrainGenerator.h"
 
class RandomTerrainGenerator : public ITerrainGenerator
{
public:

    /**
     * Default constructor
     */
    RandomTerrainGenerator(const unsigned int seed, const unsigned short size);

    /**
     * Destructor
     */
    virtual ~RandomTerrainGenerator();

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

    /**
     * Our preferred size
     */
    unsigned short      m_Size;

    /**
     * Our seed
     */
    unsigned int        m_Seed;

};

#endif //__RANDOMTERRAINGENERATOR_H__
