/**
 * ITerrainGenerator
 *
 * Interface for terrain-generator the Terrain will use 
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __ITERRAINGENERATOR_H__
#define __ITERRAINGENERATOR_H__
 
class ITerrainGenerator
{
public:
    ITerrainGenerator()
    {
    }
    virtual ~ITerrainGenerator()
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
    virtual void generateTerrain(int seed, unsigned short size, unsigned char** ppVertexHeightData) = 0;
};

#endif //__ITERRAINGENERATOR_H__