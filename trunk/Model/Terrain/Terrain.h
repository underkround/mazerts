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

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "ITerrainGenerator.h"
#include "../Common/Vector3.h"
#include <math.h>
 
#ifndef NULL
#define NULL 0
#endif

class Terrain
{
public:

// ===== CONSTANTS

    /**
     * Default map size, if not defined by constructor
     */
    static const short DEFAULT_MAPSIZE = 256;

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
    static const short MOVECOST_OFFSET = 550;

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
    static const short MOVE_OUTOFBOUNDS = 20000;

// =====

    /**
     * Returns the current singleton-instance, if no instance
     * exists, a new on is created with default values
     * @return Reference to ITerrain-instance
     */
    static Terrain* getInstance();

// ===== INITIALIZATION & CLEANUP

    /**
     * Create new flat terrain with size and other values from
     * properties
     */
    void initialize();

    /**
     * Create new flat terrain with size*size cells
     * NOTE: if generator is set, it may override the size
     */
    void initialize(const unsigned short size);

    /**
     * Create new terrain with given generator
     */
    void initialize(ITerrainGenerator* generator);

    /**
     * Releases and deletes the generator, if any.
     * If this is used to get a flat terrain, initialize -method needs
     * to be called after this.
     */
    void releaseGenerator();

    /**
     * Release and dispose the terrain temporarily so it doesn't take
     * up memory.
     * Actually this sets the terrain to 1x1 flat.
     * Doesn't lose the generator, but the size is set to 1.
     */
    void release();

// ===== GETTERS

    /**
     * Returns the map size.
     * @return Size of one map side
     */
    inline unsigned short getSize() const
    {
        return m_Size;
    }

    /**
     * Returns a two-dimensional unmodifiable array of terrain tile-height -data
     * First dimension is Y, second dimension is X
     * @return Pointer to two-dimensional array of terrain tile-heights
     */
    inline const unsigned char * const * const getTerrainHeightData(void) const
    {
        return (const unsigned char* const * const)m_ppTerrainHeightData;
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
     * Returns a two-dimensional unmodifiable array of terrain vertex-height -data.
     * First dimension is Y, second dimension is X
     * @return Pointer to two-dimensional array of terrain vertex-heights
     */
    inline const unsigned char * const * getTerrainVertexHeightData() const
    {
        return m_ppVertexHeightData;
    }

    /**
     * Return the terrain generator
     * NOTE: Returns NULL if no generator is set
     * @return used terrain generator or null
     */
    inline const ITerrainGenerator * const getTerrainGenerator() const
    {
        return m_pTerrainGenerator;
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
     * Returns the water-level of the map
     * @return Water-level
     */
    inline unsigned char getWaterLevel() const
    {
        return m_WaterLevel;
    }

    /**
     * Checks if the tile at x, y is passable or not
     * @param x X-coordinate of the tile
     * @param y Y-coordinate of the tile
     * @param size Size of the unit, size*size tiles
     * @return True if the tile is passable, otherwise false
     */
    inline bool isPassable(short x, short y, unsigned char size)
    {
        if(size == 1)
        {
            //Bounds checking
            if(x < 0 || x >= m_Size || y < 0 ||y >= m_Size)
            {
                return false;
            }
            else
            {
                return m_ppPassableTile[y][x];
            }
        }
        else
        {
            if(x < 0 || (x+size) >= m_Size || y < 0 || (y+size) >= m_Size)
            {
                return false;
            }
            else
            {
                for(int i = 0; i < size; ++i)
                {
                    for(int j = 0; j < size; ++j)                    
                    {
                        if(!m_ppPassableTile[y+j][x+i])
                            return false;
                    }
                }

                return true;
            }
        }
    }

// ===== SETTERS

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
        
        //Re-calculates also passable-array, which can be affected by changed waterlevel
        calculateHeightMapFromVertices();
    }

    /**
     * Set the terraingenerator to use
     * @param pTerraGen Pointer to an object implementing ITerrainGenerator
     */
    inline void setTerrainGenerator(ITerrainGenerator* pTerraGen)
    {
        //Get rid of old instance, if any
        if(m_pTerrainGenerator)
        {
            delete m_pTerrainGenerator;
            m_pTerrainGenerator = NULL;
        }

        m_pTerrainGenerator = pTerraGen;
        // @TODO: initialize with new generator?
    }

// MISC

    /**
     * Sync the given vector's z-component to match the current level of the
     * terrain in the point defined by vector's x and y components.
     * If step is greater than zero, the z-component adjust is no more than
     * the value of step. If the z-component is not in sync with terrain after
     * alter by step, false is returned.
     * @param v     the vector to synchronize
     * @param step  maximum amout of modification allowed, or 0 for unlimited
     * @return      true, if the z-component is in sync with the terrain level
     *
     * !TODO!: for now this only syncs to the tile level, not the precise
     * calculated level of the terrain in the given point.
     *
     * !TODO!: step not implemented!
     */
    inline bool syncToTerrainLevel(Vector3* v, float step) {
        if(v->x < 0 || v->y < 0) return false;            // not really right return values
        if(v->x > m_Size || v->y > m_Size) return false;  // since these are out of sync!
        float height = (float)m_ppTerrainHeightData[(int)v->y][(int)v->x];
        float diff = height - v->z;
        step = fabs(step);
        if(step > 0.0001 && step < fabs(diff)) {
            // modify by step, not by diff
            if(diff > 0)
                v->z += step;
            else
                v->z -= step;
            return false; // not in sync yet if step was smaller than height difference
        }
        // full sync
        v->z = height;
        return true;
    }

// TERRAIN GENERATING

    /**
     * Sets all heights to given value
     * @param heightValue height-value to set
     */
    void flattenMap(const unsigned char heightValue);

    /**
     * Smooths (blurs) the heightmap with given number
     * of passes
     * @param smoothPasses how many times the smoothing is done
     */
    void smoothMap(int smoothPasses);


protected:

    /**
     * Default constructor, protected because class is singleton
     */
    Terrain();

    virtual ~Terrain();

    /**
     * Calculates the tile-heights based on vertexdata
     */
    void calculateHeightMapFromVertices();

    /**
     * Calculate tileheight from surrounding vertices and
     * mark as passable/non-passable
     * @param x Tile x-location
     * @param y Tile y-location
     */
    void calculateTileHeight(const short x, const short y);

    /**
     * Deallocates internal data and cleans up.
     */
    void destroy();

// ===== MEMBERS

    /**
     * Terraingenerator used
     */
    ITerrainGenerator* m_pTerrainGenerator;

    /**
     * Dynamic two-dimensional array of heights
     * First dimension is Y, second dimension is X
     */
    unsigned char** m_ppTerrainHeightData;

    /**
     * Dynamic two-dimensional array of vertice heights
     * Vertex-height array size is always [size + 1][size + 1], because
     * 4 points are needed to define one tile (quad)
     * First dimension is Y, second dimension is X
     */
    unsigned char** m_ppVertexHeightData; 

    /**
     * Tells if the object has been initialized or not
     */
    bool m_Initialized;

    /**
     * Quick lookup-table for passable/non-passable squares
     * First dimension is Y, second dimension is X
     */
    bool** m_ppPassableTile;

    /**
     * Seed-number used to initialize terrain
     */
    int m_Seed;

    /**
     * Size of one side of the map, both sides 
     * are always the sam size
     */
    unsigned short m_Size;

    /**
     * The height value at which the water-level is
     */
    unsigned char m_WaterLevel;

};

#endif // __TERRAIN_H__