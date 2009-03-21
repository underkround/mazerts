#ifndef __IMAGETERRAINGENERATOR_H__
#define __IMAGETERRAINGENERATOR_H__

#define DEFAULT_FILENAME "map.bmp"
#define DEFAULT_SIZE 512

#include "ITerrainGenerator.h"

class ImageTerrainGenerator : public ITerrainGenerator
{
public:

    /**
     * Initializes using default parametres.
     */
    ImageTerrainGenerator();

    /**
     * Initializes using provided BMP-image.
     * @param filename  name of the bmp.
     */
    ImageTerrainGenerator(char* filename);

    /**
     * Get the preferred size for generator's preferences.
     * @return our preferred size for the terrain or 0 to use default
     */
    inline virtual const unsigned short getPreferredSize() { 
        return m_Size; 
    }

    /**
     * Generates a new terrain into given VertexHeightData
     * @param ppVertexHeightData    Pointer to a two-dimensional array holding
     *                              the vertexheightdata of the terrain
     * @param terrainSize           The current size of the terrain
     */
    virtual void generateHeightmap(unsigned char** ppVertexHeightData, const unsigned short terrainSize);

    /**
     * Loads image and initializes size. If this is not called, assumes default
     * map size and tries to open "map.bmp" when generateHeightmap is called.
     * @param filename name of the map
     */
    void initialize(char* filename);

private:
    unsigned short  m_Size;
    char*           m_Filename;
};

#endif
