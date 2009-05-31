/**
 * PlainTerrainGenerator
 *
 * Just creates a plain level with provided size.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __PLAINTERRAINGENERATOR_H__
#define __PLAINTERRAINGENERATOR_H__

#define DEFAULT_SIZE 512
#define LEVEL_ABOVE_WATER 2

#include "ITerrainGenerator.h"
#include "Terrain.h"

class PlainTerrainGenerator : public ITerrainGenerator
{
public:
    inline PlainTerrainGenerator(const unsigned int size = DEFAULT_SIZE) { m_Size = size; }
    inline ~PlainTerrainGenerator(void) {}

    /**
     * Get the preferred size for generator's preferences.
     * If this method returns 0, the size is up to the Terrain to decide.
     * @return our preferred size for the terrain or 0 to use default
     */
    inline const unsigned short getPreferredSize()
    {
        return m_Size;
    }

    /**
     * Generates a new terrain into given VertexHeightData
     * @param ppVertexHeightData    Pointer to a two-dimensional array holding
     *                              the vertexheightdata of the terrain
     * @param terrainSize           The current size of the terrain
     */
    inline void generateHeightmap(unsigned char** ppVertexHeightData, const unsigned short terrainSize)
    {
        for (unsigned int y = 0; y < terrainSize; y++)
            for (unsigned int x = 0; x < terrainSize; x++) {
                ppVertexHeightData[y][x] = Terrain::getInstance()->getWaterLevel() + LEVEL_ABOVE_WATER;
            }
    }

    inline void setSize(const unsigned int size) { m_Size = size; }
    inline const unsigned int getSize() { return m_Size; }

private:
    unsigned int    m_Size;

};

#endif