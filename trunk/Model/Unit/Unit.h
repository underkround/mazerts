/**
 * Unit
 *
 * The Unit-class represents both moving vehicles and stationary buildings. 
 * Units are stored within the UnitContainer, and they themselves contain
 * multiple Components
 *
 *
 * *** USAGE ***
 *
 * You create the unit object by giving the type id in the constructor:
 *  Unit* u = new Unit(UNIT_ID_BASIC);
 *
 * Then you set the unit's size and other parameteris:
 *  u->setWidth(2); u->setHeight(2);
 *
 * Set the components of the unit:
 *  u->addComponent(new SomeNewComponent(params));
 *  u->addComponent(someExistingComponent);
 *
 * Position the unit to where it should be build in the map:
 *  u->forcePosition(200,200);
 *
 * Tell the unit that it's ready to be send to the game:
 *  u->create();
 *
 * You can now lose the pointer u since the UnitCollection takes care of the
 * unit from this point on.
 *
 * The unit is destroyed when it's hitpoints drop to 0 or below, or you can
 * instantly get rid of the unit by calling unit->release();
 *
 * !DO NOT! however delete the unit even when manually calling the release
 * method! (the UnitCollection handles that).
 *
 * You can register as observer to single unit for it's position, or to get
 * notices of the new and deleted objects via the unit collection.
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
#include "UnitCollection.h"

class Unit : public IUpdatable
{
public:

    static const int STATE_BEING_BUILD  = 0; // cannot take commands, switces to active when hitpoints reach max
    static const int STATE_ACTIVE       = 1; // can take commands
    static const int STATE_PARALYZED    = 2; // updates to components temporarily disabled until counter reaches 0
    static const int STATE_DESTROYED    = 3; // object is destroyed when countdown reaches 0

    /**
     * How many slots to allocate to components by default.
     * If there are more components, the array will be expanded by
     * this value -> set to a value that should be enough for units
     * in the game so growing is not needed.
     */
    const static unsigned short DEFAULT_COMPONENT_ARRAY_SIZE = 5;

    /**
     * Default constructor
     */
    Unit(int typeId=0);

    /**
     * Destructor
     */
    virtual ~Unit(void);

    /**
     * This should be called when the unit is initialized and it's ready
     * to be put in the game
     */
    void create(void);

    void release(void);

    /**
     * Updates the components of this unit, and performs checks to unit's
     * member data.
     */
    virtual char update(const float deltaT);

// ========== SETTERS

    /**
     * Sets the width of the unit (x-tiles)
     * @param width The width of the unit
     */
    inline void setWidth(const unsigned char width) { m_Width = width; }

    /**
     * Sets the height of the unit (y-tiles)
     * @param height The height of the unit
     */
    inline void setHeight(const unsigned char height) { m_Height = height; }

    /**
     * Set maximum hitpoints for unit. If the Unit's current hitpoints top
     * the set maximum value, current hitpoints are reduced to match the max.
     * If unit's current hitpoints are below new max, they are left untouched.
     * @param maxHitpoints new value for maximum hitpoints the unit can have
     */
    inline void setMaxHitpoints(const int maxHitpoints)
    {
        if(maxHitpoints <= 0)
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
    inline unsigned char getWidth() { return m_Width; }

    /**
     * Returns the height of the unit (y-tiles)
     * @return the height of the unit
     */
    inline unsigned char getHeight() { return m_Height; }

    /**
     * @return the unit's cell coordinate x
     */
    inline const unsigned short getCellX(void) { return m_CellX; }

    /**
     * @return the unit's cell coordinate y
     */
    inline const unsigned short getCellY(void) { return m_CellY; }

    /**
     * Return pointer to the position of the unit as Vector3 object.
     *
     * !NOTE!:  only components are allowed to alter this vector directly!
     *          otherwise the unit's grid position might not be short
     *          time out of sync (ok.. until next update call.. but still :)
     * @return position vector of the unit
     */
    inline Vector3* getPosition(void) { return &m_Position; }

    /**
     * Return pointer to the direction of the unit as Vector3 object.
     * Direction can be altered directly?
     * @return direction vector of the unit
     */
    inline Vector3* getDirection(void) { return &m_Direction; }

    /**
     * Return the DamageHandler of this unit for setting it's values.
     * DamageHandler reduces the amount of the damage the unit receives.
     * @return pointer to the DamageHandler of this unit
     */
    inline DamageHandler const * const getDamageHandler(void) { return &m_DamageHandler; }

    /**
     * @return count of the components attached to this unit
     */
    inline const unsigned short getComponentCount() { return m_ComponentCount; }

    /**
     * Source should pass fresh damage object.
     *
     * !NOTE!: this will delete the damage object after it's been used,
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
        m_DamageHandler.filterDamage(d);
        // reduce the modified damage from our hitpoints
        m_Hitpoints -= d->getTotalDamage();
        // @TODO: update needed?
        delete d;
        // check if we're dead
        if(m_Hitpoints <= 0)
            m_State = STATE_DESTROYED;
        // return the amount of damage reduced
        return d->getTotalDamage();
    }

private:

// =====

    /**
     * Grow the component array by default component array size..
     * Pointers are cheap (?) so it might be best to ensure enough capacity
     * in the first place :)
     * But just to be sure..
     */
    void increaseComponentCapacity(void);

// ===== MISC MEMBERS

    int m_TypeId;           // the type of the unit
    int m_Level;            // the level of the unit (type+level define the looks)

    int m_State;            // the current state of the unit
    bool m_Initialized;     // flag for the initialization phase

    DamageHandler m_DamageHandler;  // Damagehandler for this unit. All damage
                                    // to this unit is filtered by damagehandler

    unsigned char m_Width;  // Size of the unit as m_Width * m_Height (x * y)
    unsigned char m_Height; // For vehicles, ALWAYS use same size for both width and height
                            // (width = height) On structures, the width and height can differ

    int m_Hitpoints;        // Unit's current hitpoints
    int m_MaxHitpoints;     // Current hitpoints cannot top this

    IComponent** m_Components;          // Components that make up the unit's logic
    unsigned char m_ComponentCount;     // Count of the components attached
    unsigned char m_ComponentArraySize; // The current capacity of the component array

    bool m_CanMove;             // enabled by moving logic, if any
    bool m_CanFire;             // enabled by weapon component, if any
    bool m_SyncHeightToTerrain; // weather to sync the position z to terrain height (for ground units)

    // timers for states
    float m_DestroyTimer;   // downcounter for delay of death to object destruction
    float m_ParalyzeTimer;  // downcounter for time to be paralized (idling)

// ===== POSITION MEMBERS

    /**
     * 3D vector representing the placement of the unit.
     *
     * x and y -components:
     *  Location of the unit in the terrain-grid.
     *  To get the cell coordinates, cast floats to int.
     *  Note that on units with size other than 1 * 1, this represents the
     *  location of the units' topleft corner-tile!
     *
     * z-component:
     *  Altitude usually calculated from the altitude of the tile-cells the
     *  unit is in
     */
    Vector3 m_Position;
    //Vector3 m_PositionCache;  // just a reminder of possible future use..

    unsigned short m_CellX;         // The cell coordinates of the terrain, these are
    unsigned short m_CellY;         // updated from the position vector.
    unsigned short m_PreviousCellX; // stored old position to use when handling the
    unsigned short m_PreviousCellY; // change in coordinates

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

};

#endif // __UNIT_H__
