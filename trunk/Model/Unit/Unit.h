/**
 * Unit
 *
 * The Unit-class represents both moving vehicles and stationary buildings. 
 * Units are stored within the UnitContainer, and they themselves contain
 * multiple Components
 *
 * $Revision$
 * $Date$
 * $Id$
 */

//TODO: This class is unfinished, STC

#ifndef __UNIT_H__
#define __UNIT_H__

#include "IUpdatable.h"
#include "../Common/Vector3.h"

class Unit : public IUpdatable
{
public:
    /**
     * Default constructor
     */
    Unit(void);

    /**
     * Destructor
     */
    virtual ~Unit(void);

    /**
     * //TODO: Update method
     */
    bool update(const float deltaT);

    /**
     * Sets the width of the unit (x-tiles)
     * @param width The width of the unit
     */
    inline void setWidth(const unsigned char width)
    {
        m_Width = width;
    }

    /**
     * Returns the width of the unit (x-tiles)
     * @return the width of the unit
     */
    inline unsigned char getWidth()
    {
        return m_Width;
    }

    /**
     * Sets the height of the unit (y-tiles)
     * @param height The height of the unit
     */
    inline void setHeight(const unsigned char height)
    {
        m_Height = height;
    }

    /**
     * Returns the height of the unit (y-tiles)
     * @return the height of the unit
     */
    inline unsigned char getHeight()
    {
        return m_Height;
    }

    /**
     * Return pointer to the position of the unit as Vector3 object.
     * @return position vector of the unit
     */
    inline Vector3* getPosition(void)
    {
        return &m_Position;
    }

    /**
     * Return pointer to the direction of the unit as Vector3 object.
     * @return direction vector of the unit
     */
    inline Vector3* getDirection(void)
    {
        return &m_Direction;
    }

private:

    /**
     * 3D vector representing the placement of the unit.
     *
     * x and y -components:
     *   Location of the unit in the terrain-grid.
     *   To get the cell coordinates, cast floats to int.
     *   Note that on units with size other than 1 * 1, this represents the
     *   location of the units' topleft corner-tile!
     *
     * z-component:
     *   Altitude from sealevel (0), usually calculated from
     *   the altitude of the tile-cells the unit is in
     */
    Vector3 m_Position;

    /**
     * 3D vector representing which way the unit is facing. The z-component
     * might not have meaningful value.
     */
    Vector3 m_Direction;

    /**
     * Size of the unit as m_Width * m_Height (x * y)
     * For vehicles, ALWAYS use same size for both width and height
     * (width = height) On structures, the width and height can differ
     */
    unsigned char m_Width;
    unsigned char m_Height;

};

#endif // __UNIT_H__
