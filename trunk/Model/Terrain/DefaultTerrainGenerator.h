/**
 * DefaultTerrainGenerator
 *
 * Default implementation for ITerrainGenerator
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __DEFTERRAINGENERATOR_H__
#define __DEFTERRAINGENERATOR_H__

#include "ITerrainGenerator.h"
 
class DefaultTerrainGenerator : public ITerrainGenerator
{
public:
    /**
     * Default constructor
     */
    DefaultTerrainGenerator()
    {
    }

    /**
     * Destructor
     */
    virtual ~DefaultTerrainGenerator()
    {
    }

    /**
     * Generates a new terrain into given VertexHeightData based on seed
     * and size
     * @param seed The seed number to use in generation
     * @param size Size of the map (size * size tiles)
     * @param ppVertexHeightData Pointer to a two-dimensional array holding
     *        the vertexheightdata
     */
    void generateTerrain(int seed, unsigned short size, unsigned char** ppVertexHeightData);
};

#endif //__DEFTERRAINGENERATOR_H__