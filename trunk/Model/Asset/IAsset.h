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

#include "IAssetRadar.h"
#include "../Common/Vector3.h"
#include "../Weapon/IWeapon.h"
#include "../Team/Team.h"

class IAsset
{

public:

    enum Type
    {
        UNIT = 1,
        BUILDING
    };

    // result codes for update method
    static const char RESULT_OK        = 0;
    static const char RESULT_DELETEME  = 1;

    IAsset(Type assetType);
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
    virtual void create() = 0;

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
    inline void setOwner(Team* owner) { m_pOwner = owner; }

    /**
     * Return the owner of this asset, or NULL if no owner
     */
    virtual inline Team* getOwner() { return m_pOwner; }

// ===== Size

    inline void setWidth(const unsigned char width)     { m_Width = width; }
    inline void setHeight(const unsigned char height)   { m_Height = height; }
    inline const unsigned char getWidth()   { return m_Width; }
    inline const unsigned char getHeight()  { return m_Height; }

// ===== Weapon

    /**
     * Indications weather this asset has a weapon attached.
     */
    inline const bool hasWeapon() const { return (m_pWeapon) ? true : false; }

    /**
     * @return  Asset's weapon. Returns NULL if asset has no weapon
     *          (hasWeapon() returns false)
     */
    inline const IWeapon* getWeapon() { return m_pWeapon; }

// ===== Position & direction

    virtual void forcePosition(short x, short y)
    {
        m_Position.x = (float)x;
        m_Position.y = (float)y;
        // @TODO: z from terrain
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
    inline Type getAssetType()      { return m_AssetType; }

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

    static int      m_InstanceCounter; // counter for asset instances created
    static int      m_InstanceDestructionCounter; // counter for asset instances deleted

protected:

    /**
     * Release and delete the weapon instance
     */
    void releaseWeapon();

    /**
     * Release and delete the unti radar instance
     */
    void releaseRadar();

// ===== MEMBERS

    const int       m_IID;          // unique instance id among all assets
    const Type      m_AssetType;    // the concrete class of this asset

    Team*           m_pOwner;       // the owner of this unit

    unsigned char   m_Width;        // sizes
    unsigned char   m_Height;
    Vector3         m_Position;     // vector defining the absolute position
    Vector3         m_Direction;    // facing of the unit

    IWeapon*        m_pWeapon;      // weapon attached to this asset, or NULL
    IAssetRadar*    m_pRadar;       // the logic that handles the inspection of
                                    // the surrounding of the asset

};

#endif // __ASSET_H__
