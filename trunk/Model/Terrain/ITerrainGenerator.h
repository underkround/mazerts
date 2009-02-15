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
     * Called before the generator is deleted, override if needed
     * ok.. maybe the destructor would do the same.. sorry ;(
     */
    virtual void release()
    {
        // nothing by default, concrete generator could close streams etc
    }

    /**
     * Get the preferred size for generator's preferences.
     * If this method returns 0, the size is up to the Terrain to decide.
     * @return our preferred size for the terrain or 0 to use default
     */
    virtual const unsigned short getPreferredSize() = 0;

    /**
     * Generates a new terrain into given VertexHeightData
     * @param ppVertexHeightData    Pointer to a two-dimensional array holding
     *                              the vertexheightdata of the terrain
     * @param terrainSize           The current size of the terrain
     */
    virtual void generateHeightmap(unsigned char** ppVertexHeightData, const unsigned short terrainSize) = 0;

};

#endif //__ITERRAINGENERATOR_H__