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
//#include "XComponent.h"
//#include "YComponent.h"
//#include "ZComponent.h"

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
    bool update(float deltaT);

    /**
     * Sets the width of the unit (x-tiles)
     * @param width The width of the unit
     */
    inline void setWidth(unsigned char width)
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
    inline void setHeight(unsigned char height)
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

private:
    
    /**
     * Location of the unit in the terrain-grid
     * Note that on units with size other than 1 * 1, this represents the
     * location of the units' topleft corner-tile!
     */
    unsigned int m_GridX;
    unsigned int m_GridY;

    /**
     * Size of the unit as m_Width * m_Height (x * y)
     * For vehicles, ALWAYS use same size for both width and height (width = height)
     * On structures, the width and height can differ
     */
    unsigned char m_Width; 
    unsigned char m_Height;

    /**
     * Altitude from sealevel (0), usually calculated from
     * the altitude of the tile-cells the unit is in
     */
    float m_Altitude;
};

#endif // __UNIT_H__