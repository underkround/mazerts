/**
 * ITerrain DEPRECATED
 *
 * Interface for the singleton Terrain-class, that defines the methods used to
 *  fetch terrain-related data both inside and outside the Model, and some needed
 * internal methods.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __ITERRAIN_H__
#define __ITERRAIN_H__

class ITerrain
{
public:

    /**
     * Returns a two-dimensional unmodifiable array of terrain heightdata
     * @return Pointer to two-dimensional array of unsigned chars
     */
    virtual const unsigned char* const * getTerrainHeightData(void) const = 0;

    /**
     * Returns the height at specified position in map
     * @param x X-position in the map to get the value from
     * @param y Y-position in the map to get the value from
     * @return unsigned char height-value
     */
    virtual const unsigned char getTerrainHeight(const short x, const short y) const = 0; 

    /**
     * Calculates movecost from tile x, y to tile x+dirX, y+dirY
     * @param x X-position in the map to move from
     * @param y Y-position in the map to move from
     * @param dirX X-direction to move in (-1, 0, 1)
     * @param dirY Y-direction to move in (-1, 0, 1)
     * @return Movecost between the tile and the neighbour defined by directions, or -1 if bounds are broken
     */
    virtual short getMoveCost(const short x, const short y, const signed char dirX, const signed char dirY) const = 0;

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
     * Sets the height for one map point
     * @param x x-coordinate of the point to set
     * @param y y-coordinate of the point to set
     * @param height Height to set the point to
     */
    virtual void setTerrainHeight(const short x, const short y, const unsigned char height) = 0;

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
     * Default constructor, protected because Terrain is singleton
     */
    ITerrain()
    {
    }
    
    /**
     * Destructor
     */
    virtual ~ITerrain()
    {
    }
    /**
     * Initializes internal data based on seed and size
     */
    virtual void initialize() = 0; 

    /**
     * Deallocates internal data and cleans up
     */
    virtual void destroy() = 0;

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
#endif // __ITERRAINDATAACCESS_H__