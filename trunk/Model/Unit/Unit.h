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
#include "IComponent.h"
#include "DamageHandler.h"
#include "Damage.h"
#include "../Common/Vector3.h"

class Unit : public IUpdatable
{
public:

    /**
     * How many slots to allocate to components.
     * If there is more components, the array will be expanded by
     * this value -> set to a value that should be enough for units
     * in the game so growing is not needed.
     */
    const static unsigned short DEFAULT_COMPONENT_ARRAY_SIZE = 5;

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
    virtual char update(const float deltaT);

// ========== SETTERS

    /**
     * Sets the width of the unit (x-tiles)
     * @param width The width of the unit
     */
    inline void setWidth(const unsigned char width)
    {
        m_Width = width;
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
     * Set maximum hitpoints for unit. If the Unit's current hitpoints top
     * the set maximum value, current hitpoints are reduced to match the max.
     * If unit's current hitpoints are below new max, they are left untouched.
     * @param maxHitpoints new value for maximum hitpoints the unit can have
     */
    inline void setMaxHitpoints(const int maxHitpoints)
    {
        if(maxHitpoints == 0)
            return; // maximum hitpoints can't be zero since the unit would be
                    // dead instantly
        m_MaxHitpoints = maxHitpoints;
        // if unit's current are greater than max, reduce
        if(m_Hitpoints > maxHitpoints)
            m_Hitpoints = maxHitpoints;
    }

    /**
     * Add a component to the unit.
     * The unit will attach the component, and external sources should not
     * call the unit's attach method but instead use this to add the
     * component.
     */
    bool addComponent(IComponent* component);

    /**
     * Force the unit to new position.
     * This method is not for normal moving of the unit.
     * @param x the x cell of the terrain
     * @param y the y cell of the terrain
     * @return false, if the target coordinate is not passable
     */
    bool forcePosition(unsigned const short x, unsigned const short y);

// ========== GETTERS

    /**
     * Returns the width of the unit (x-tiles)
     * @return the width of the unit
     */
    inline unsigned char getWidth()
    {
        return m_Width;
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
     * @return the unit's cell coordinate x
     */
    inline const unsigned short getCellX(void)
    {
        return m_CellX;
    }

    /**
     * @return the unit's cell coordinate y
     */
    inline const unsigned short getCellY(void)
    {
        return m_CellY;
    }

    /**
     * Return pointer to the position of the unit as Vector3 object.
     *
     * !NOTE!:  only components are allowed to alter this vector directly!
     *          otherwise the unit's grid position might not be short
     *          time out of sync (ok.. until next update call.. but still :)
     * @return position vector of the unit
     */
    inline Vector3* getPosition(void)
    {
        return &m_Position;
    }

    /**
     * Return pointer to the direction of the unit as Vector3 object.
     * Direction can be altered directly?
     * @return direction vector of the unit
     */
    inline Vector3* getDirection(void)
    {
        return &m_Direction;
    }

    /**
     * Return the DamageHandler of this unit that filters damage
     * hitting it.
     * @return pointer to the DamageHandler of this unit
     */
    inline DamageHandler const * const getDamageHandler(void)
    {
        return &m_DamageHandler;
    }

    /**
     * Source should pass fresh damage object.
     * NOTE: this will delete the damage object after it's been used,
     * since the object is modified and hence cannot be used after this
     * unit is done with it.
     *
     * The simplest way to use this would then be:
     *   unit->handleDamage(new Damage(1,2,3));
     *
     * Source giving the damage object is responsible of removing
     * the modified damage object after it has passed this method.
     */
    inline const int handleDamage(Damage* d)
    {
        // filter the damage with damage handler
        // reduce the modified damage from our hitpoints
        m_Hitpoints -= d->getTotalDamage();
        // @TODO: update needed?
        delete d;
        // return the amount of damage reduced
        return d->getTotalDamage();
    }

    inline const unsigned short getComponentCount()
    {
        return m_ComponentCount;
    }

private:

// =====

    /**
     * Grow the component array by default component array size..
     * Pointers are cheap (?) so it might be best to ensure enough capacity
     * in the first place :)
     * But just to be sure..
     */
    inline void increaseComponentCapacity()
    {
        IComponent** tmpHolder = new IComponent*[m_ComponentCount];
        // this could possibly be done with some fancy memory copy -function
        // but i'm the lame java-guy!
        for(int i=0; i<m_ComponentCount; i++)
            tmpHolder[i] = m_Components[i];
        delete [] m_Components;
        m_Components = new IComponent*[m_ComponentCount + DEFAULT_COMPONENT_ARRAY_SIZE];
        for(int i=0; i<m_ComponentCount; i++) {
            if(i < m_ComponentCount)
                m_Components[i] = tmpHolder[i];
            else
                m_Components[i] = NULL;
        }
        delete [] tmpHolder;
    }

// ===== MEMBERS

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
     * The cell coordinates of the terrain, these are updated from the
     * position vector.
     */
    unsigned short m_CellX;
    unsigned short m_CellY;

    /**
     *
     */
    bool m_SyncHeightToTerrain;

    /**
     * 3D vector representing which way the unit is facing. The z-component
     * might not have meaningful value.
     *
     * TODO: should this be just degree value? relative to what? world z axis
     *      (or whichever axis perpendicular to the terrain plane)?
     *      In this case the unit would not have facing in the 3d world, it
     *      could not be going upwards or downwards (possibility of flying
     *      units etc)..
     *      The unit could have it's facing represented by euler angles?
     */
    Vector3 m_Direction;

    /**
     * Damagehandler for this unit
     * All damage to this unit is filtered by damagehandler
     */
    DamageHandler m_DamageHandler;

    /**
     * Size of the unit as m_Width * m_Height (x * y)
     * For vehicles, ALWAYS use same size for both width and height
     * (width = height) On structures, the width and height can differ
     */
    unsigned char m_Width;
    unsigned char m_Height;

    /**
     * Unit's current hitpoints. If this goes <1, the unit is destroyed.
     * This is left intentionally signed, so that unit's hitpoints can go
     * below zero.
     * This is the hitpoint value that is reduced when the unit receives
     * damage.
     */
    int m_Hitpoints;

    /**
     * Unit's maximum hitpoints. This determines the maximum strength
     * of the unit, and it's current hitpoints cannot top the maximum
     * hitpoints (when repairing etc).
     * By default, this value is not changed. An exception would be
     * if the Unit is upgraded.
     */
    int m_MaxHitpoints;

    IComponent** m_Components;

    /**
     * Count of the components attached
     * 
     */
    unsigned char m_ComponentCount;

    /**
     * The current capacity of the component array
     */
    unsigned char m_ComponentArraySize;

};

#endif // __UNIT_H__
