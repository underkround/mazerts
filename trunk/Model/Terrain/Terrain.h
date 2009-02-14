/**
 * Terrain
 *
 * Terrain is the "map" (height data) of the world, generated based on
 * given (or randomized, if default constructor is used) seed-number.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

//TODO: This class is unfinished, STC

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

//#include "ITerrain.h"
#include <stdlib.h> //For srand and rand
#include <time.h>   //For random seed
 
class Terrain //: public ITerrain
{
public:

    /**
     * Default map size, if not defined by constructor
     */
    static const short DEFAULT_MAPSIZE = 255;

    /**
     * Default flattening height
     */
    static const unsigned char DEFAULT_FLATHEIGHT = 100;

    /**
     * Default water-level
     */
    static const unsigned char DEFAULT_WATERLEVEL = 50;


    /**
     * Movecost offset, this is the value that is returned
     * as movecost between tiles at same height
     */
    static const short MOVECOST_OFFSET = 100;

    /**
     * Movecost-threshold: this value is the maximum amount
     * the movecost can differ from threshold
     */
    static const short MOVECOST_THRESHOLD = 10;
    static const short MOVECOST_MAX = MOVECOST_OFFSET + MOVECOST_THRESHOLD;
    static const short MOVECOST_MIN = MOVECOST_OFFSET - MOVECOST_THRESHOLD;

    /**
     * Value returned by getMoveCost, if the move is illegal 
     * (non-passable)
     */
    static const short MOVE_ILLEGAL = 10000;

    /**
     * Value returned by getMoveCost, if the move would result
     * to going out of the map
     */
    static const short MOVE_OUTOFBOUNDS = -10000;

    /**
     * Creates a new instance with given seed and size, if existing
     * instance is detected, it is first released
     * @param seed Seed-number for map generation
     * @param size size for map side
     * @return Reference to created ITerrain-instance
     */
    static Terrain& createNew(const int seed, const short size);

    /**
     * Returns the current singleton-instance, if no instance
     * exists, a new on is created with default values
     * @return Reference to ITerrain-instance
     */
    static Terrain& getInstance();

    /**
     * Releases the current instance (if any)
     */
    static void release();


    virtual ~Terrain()
    {
        destroy();
    }

    /**
     * Returns the map size.
     * @return Size of one map side
     */
    inline unsigned short getSize() const
    {
        return m_Size;
    }

    /**
     * Returns the seed used to initialize the terrain
     * @return Seed-number
     */
    inline int getSeed() const
    {
        return m_Seed;
    }

    /**
     * Returns a two-dimensional unmodifiable array of terrain tile-height -data
     * @return Pointer to two-dimensional array of terrain tile-heights
     */
    inline const unsigned char* const * getTerrainHeightData(void) const
    {
        return (const unsigned char**)m_ppTerrainHeightData;
    }

    /**
     * Returns the height at specified position in map
     * @param x X-position in the map to get the value from
     * @param y Y-position in the map to get the value from
     * @return unsigned char height-value
     */
    inline const unsigned char getTerrainHeight(const short x, const short y) const
    {
        return (const unsigned char)m_ppTerrainHeightData[y][x];
    }

    /**
     * Returns a two-dimensional unmodifiable array of terrain vertex-height -data
     * @return Pointer to two-dimensional array of terrain vertex-heights
     */
    inline const unsigned char * const * getTerrainVertexHeightData() const
    {
        return m_ppVertexHeightData;
    }

    /**
     * Calculates movecost from tile x, y to tile x+dirX, y+dirY
     * @param x X-position in the map to move from
     * @param y Y-position in the map to move from
     * @param dirX X-direction to move in (-1, 0, 1)
     * @param dirY Y-direction to move in (-1, 0, 1)
     * @return Movecost between the tile and the neighbour defined by directions, 
               or MOVE_OUTOFBOUNDS if bounds are broken, or MOVE_ILLEGAL, if the move
               is not possible (too big height difference)
     */
    short getMoveCost(const short x, const short y, const signed char dirX, const signed char dirY) const;

    /**
     * Sets the height of a single vertex in terrain. The vertices for a tile are (tilex, tiley),
     * (tilex+1, tiley), (tilex, tiley+1) and (tilex+1, tiley+1).
     * @param x The x-coordinate of the vertex to set, in range (0...map size+1)
     * @param x The Y-coordinate of the vertex to set, in range (0...map size+1)
     * @param height Height to set the vertex to, 0...255
     */
    void setTerrainVertexHeight(const short x, const short y, const unsigned char height);

    /**
     * Sets the water-level of the map
     * @param waterLevel Water-level to set
     */
    inline void setWaterLevel(const unsigned char waterLevel)
    {
        m_WaterLevel = waterLevel;
    }

    /**
     * Returns the water-level of the map
     * @return Water-level
     */
    inline unsigned char getWaterLevel() const
    {
        return m_WaterLevel;
    }

protected:

    /**
     * Default constructor, protected because class is singleton
     */
    Terrain()
    {
        srand((unsigned int)time(0));
        m_Seed = rand();
        m_Size = Terrain::DEFAULT_MAPSIZE;
        m_WaterLevel = Terrain::DEFAULT_WATERLEVEL;
        initialize();
    }

   /**
     * Parametric constructor for creating map with specific seed and size
     * @param seed Seed-number to use
     * @param size Size of the map-side, actual cell-amount is (size-1)*(size-1)
     */
    Terrain(const int seed, const short size)
    {
        m_Seed = seed;
        m_Size = size;
        m_WaterLevel = Terrain::DEFAULT_WATERLEVEL;
        initialize();
    }

    /**
     * Static pointer to singleton instance
     */
    static Terrain* pTerrain;

    /**
     * Initializes internal data based on seed and size
     */
    void initialize(); 

    /**
     * Deallocates internal data and cleans up
     */
    void destroy();

    /**
     * Generates the heightmap based on size and seed
     */
    void generateMap();

    /**
     * Sets all heights to given value
     * @param heightValue height-value to set
     */
    void flattenMap(const unsigned char heightValue);

    /**
     * Calculates the tile-heights based on vertexdata
     */
    void calculateHeightMapFromVertices();

    /**
     * Calculate tileheight from surrounding vertices
     * @param x Tile x-location
     * @param y Tile y-location
     */
    void calculateTileHeight(const short x, const short y);

    /**
     * Dynamic two-dimensional array of heights
     */
    unsigned char** m_ppTerrainHeightData;

    /**
     * Dynamic two-dimensional array of vertice heights
     * Vertex-height array size is always [size + 1][size + 1], because
     * 4 points are needed to define one tile (quad)
     */
    unsigned char** m_ppVertexHeightData; 

    /**
     * Seed-number used to initialize terrain
     */
    int m_Seed;

    /**
     * Size of one side of the map, both sides 
     * are always the sam size
     */
    short m_Size;

    /**
     * The height value at which the water-level is
     */
    unsigned char m_WaterLevel;
};

#endif // __TERRAIN_H__