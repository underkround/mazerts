/**
 *
 * !IMPORTANT IMPLEMENTATION NOTES!
 *
 * There are few key notes regarding the assets:
 *  - assets are created with static AssetFactory -class
 *    asset creation consist of:
 *      - setting the properties, moving, weapon etc. for the asset
 *      - calling asset->create() that registers the asset to the game
 *  - after the create, the unit is managed by the collection, and
 *    should not be deleted, updated etc by themselves!
 *  - asset can be forcely destroyed by the collection's methods
 *  - assets are updated via the collection's update-methods
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __ASSET_H__
#define __ASSET_H__

#include "../Common/DoubleLinkedList.h"
#include "IAssetRadar.h"
#include "../Common/Vector3.h"
#include "../Weapon/IWeapon.h"
#include "../Player/Player.h"

//struct AssetDef;
#include "../Defs/Defs.h"

class IAssetListener;
class Damage;
class Armor;


class IAsset
{
public:

    /**
     * Concrete types of IAsset classes. When adding new concrete class
     * to implement asset, add value here.
     * These need to be bit-values, so increase the shift value with 1
     * when adding.
     * Remember to keep the END -type last
     */
    enum Type
    {
        UNIT            = 1 << 1,
        BUILDING        = 1 << 2,
        TYPE_END        = 1 << 3    // remember to advance the shift when adding new concrete type
    };

    enum State
    {
        STATE_BEING_BUILT = 0,  // set when created, until hitpoints reach max,
                                // cannot operate until active
        STATE_ACTIVE,           // normal state
        STATE_PARALYZED,        // cannot do anything
        STATE_DESTROYED         // downcounter running for object destruction
    };

    // result codes for update method
    static const char RESULT_OK        = 0;
    static const char RESULT_DELETEME  = 1;

    IAsset(Type assetType, AssetDef& def);
    virtual ~IAsset();

// ==== update

    /**
     * Update & dispatch the update to the components
     */
    virtual char update(const float deltaT) = 0;

    /**
     * Called by the collection only!
     * Release resources, called right before the instance is to be deleted.
     */
    virtual void release();

// ===== Initialization

    /**
     * Should be called after setting the properties & initial position.
     * Adds the asset to the game, and transfers the ownershp to the
     * collection.
     */
    virtual void create();

    /**
     * Set weapon to this asset (old weapon will be deleted, if any).
     */
    void setWeapon(IWeapon* weapon);

    /**
     * Set the radar
     */
    void setRadar(IAssetRadar* radar);

    /**
     * Set the owner of this unit
     */
    void setOwner(Player* owner) { m_pOwner = owner; }

// ===== Getters

    /**
     * @return  The owner of this asset, or NULL if no owner
     */
    inline Player* getOwner()       { return m_pOwner; }

    /**
     * @return  The current main state of this asset
     */
    inline State getState()         { return m_State; }

    /**
     * @return  The concrete class type of this asset
     */
    inline Type getAssetType()      { return m_ConcreteType; }

    /**
     * @return  The def struct that defines this asset
     */
    inline AssetDef* getDef()       { return &m_Def; }

    /**
     * @return    Type id of this asset from defines
     */
    inline int getTypeTag()         { return m_Def.tag; }

    /**
     * Returns the object implementing radar interface
     * @return Pointer to IAssetRadar
     */
    inline IAssetRadar* getRadar() { return m_pRadar; }

// ===== Size

    /**
     * Getters for unit's size properties
     */
    inline const unsigned char getWidth()   { return m_Def.width; }
    inline const unsigned char getHeight()  { return m_Def.height; }

// ===== Weapon

    /**
     * Indications weather this asset has a weapon attached.
     * @return  true, if this asset has weapon attached to it
     */
    inline const bool hasWeapon() const { return (m_pWeapon) ? true : false; }

    /**
     * @return  Asset's weapon. Returns NULL if asset has no weapon
     *          (hasWeapon() returns false)
     */
    inline IWeapon* getWeapon() { return m_pWeapon; }

// ===== Damage and hitpoints

    inline const float getHitpoints() { return m_Hitpoints; }

    /**
     * Return the maximum hitpoints for this asset
     */
    inline const int getHitpointsMax() { return m_Def.maxHitpoints; }

    /**
     * Straight modification of asset's hitpoints (not to use with
     * weapon damage).
     * Resulting hitpoints cannot top max hitpoints.
     * If the resulting hitpoints go to or below zero, the asset enters
     * destroyed state.
     * @param amount    value which to add to hitpoins, use negative
     *                  value to reduce
     * @return          hitpoins after modification
     */
    inline const float modifyHitpoints(const float amount);

    /**
     * When asset receives damage, it's done through this method.
     *
     * NOTE: ownership of the passed Damage-object is transferred
     * to this asset, and the damage-object is destroyed inside this
     * method.
     *
     * @return  asset's hitpoints after the hit
     */
    virtual float handleDamage(Damage* d);

// ===== Position & direction

    /**
     * Used only to place the asset on the map when created.
     * Works only in BEING_BUILT state (just in case and to spot
     * misuse).
     */
    virtual void forcePosition(short x, short y)
    {
        if(m_State == STATE_BEING_BUILT)
        {
            m_OldPosX = x;
            m_OldPosY = y;
            m_Position.x = (float)x;
            m_Position.y = (float)y;
            if(m_Created)
                updatePositionInAssetCollection();
            // @TODO: z from terrain
        }
    }

    virtual void forceDirection(float x, float y, float z)
    {
        m_Direction.x = x;
        m_Direction.y = y;
        m_Direction.z = z;
    }

    inline Vector3* getPosition()   { return &m_Position; }
    inline Vector3* getDirection()  { return &m_Direction; }
    inline short getGridX()         { return (short)m_Position.x; }
    inline short getGridY()         { return (short)m_Position.y; }

    inline unsigned short getCenterGridX()   { return (unsigned short)(m_Position.x + m_HalfWidth); }
    inline unsigned short getCenterGridY()   { return (unsigned short)(m_Position.y + m_HalfHeight); }

// ===== Listeners

    /**
     * Register new listener that starts to receive notifications
     * of this asset.
     * @param listener  class implementing the IAssetListener
     */
    void registerListener(IAssetListener* listener);

    /**
     * Remove registered listener, removed listener does not receive
     * notifications of this asset after this call.
     * @param listener  registered listener that is to be removed
     *                  from the listeners of this asset
     */
    void unregisterListener(IAssetListener* listener);

// ===== Mostly for debugging, but why not for some statistics too

    // Unique instance id, each created asset gets one starting from 0.
    inline const int getIID() { return m_IID; }

    // Number of asset instances created while the program has been running.
    inline static const int getInstanceCount() { return m_InstanceCounter; }

    // Number of asset instances deleted while the program has been running.
    inline static const int getInstanceDestructionCount() { return m_InstanceDestructionCounter; }

private:
    // prevent copying
    IAsset(const IAsset& a);
    IAsset& operator=(IAsset const&);

    static int      m_InstanceCounter; // counter for asset instances created
    static int      m_InstanceDestructionCounter; // counter for asset instances deleted

protected:

    /**
     * Change the asset's state.
     * Use this instead of directly setting the state, since this
     * notifies the state to the listeners.
     */
    void changeState(State newState);

// ===== Listeners

    /**
     * Notify listeners that this asset object has changed it's main state.
     */
    void notifyStateChanged();

    /**
     * Notify listeners that this asset object is being destroyed.
     */
    void notifyDestroyed();

// ===== Release

    /**
     * Release and delete the weapon instance
     */
    void releaseWeapon();

    /**
     * Release and delete the unti radar instance
     */
    void releaseRadar();

    /**
     * Checks if position has changed, and if it is, updates position in 
     * AssetCollection accordingly.
     */
    void updatePositionInAssetCollection();

// ===== MEMBERS

    const int       m_IID;          // unique instance id among all assets
    const Type      m_ConcreteType; // the concrete class of this asset
    AssetDef&       m_Def;          // definition struct for this asset

    bool            m_Created;      // true, if asset is registered to the collection
                                    // this should be taken account when dealing with the collection!

    State           m_State;        // the current state of the asset

    Player*         m_pOwner;       // the owner of this unit

    float           m_Hitpoints;    // current hitpoints of the asset

    Armor*          m_pArmor;       // armor for filtering the received damage

    Vector3         m_Position;     // vector defining the absolute position
    Vector3         m_Direction;    // facing of the unit

    IWeapon*        m_pWeapon;      // weapon attached to this asset, or NULL
    IAssetRadar*    m_pRadar;       // the logic that handles the inspection of
                                    // the surrounding of the asset

    DoubleLinkedList<IAssetListener*>    m_pListeners; // registered listeners

    // old position, used to check if coordinate has changed
    unsigned short              m_OldPosX;
    unsigned short              m_OldPosY;

    float           m_HalfWidth;        //Half of the asset width, used for center-coordinates
    float           m_HalfHeight;       //Half of the asset height
};

#endif // __ASSET_H__
