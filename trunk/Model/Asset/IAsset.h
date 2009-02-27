/**
 *
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

// =====

    virtual void create() = 0;

    virtual void release() = 0;

    virtual char update(const float deltaT) = 0;

    inline void setOwner(Team* owner) { m_Owner = owner; }

    inline Team* getOwner() { return m_Owner; }

// ===== Size

    inline void setWidth(const unsigned char width)     { m_Width = width; }
    inline void setHeight(const unsigned char height)   { m_Height = height; }
    inline const unsigned char getWidth()   { return m_Width; }
    inline const unsigned char getHeight()  { return m_Height; }

// ===== Weapon

    /**
     * Indications weather this asset has a weapon attached.
     */
    inline const bool hasWeapon() const { return (m_Weapon) ? true : false; }

    /**
     * @return  Asset's weapon. Returns NULL if asset has no weapon
     *          (hasWeapon() returns false)
     */
    inline const IWeapon* getWeapon() { return m_Weapon; }

    /**
     * Set weapon to this asset (old weapon will be deleted, if any).
     */
    void setWeapon(IWeapon* weapon);

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
    inline Type getAssertType()     { return m_AssetType; }

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

    const int       m_IID;          // unique instance id among all assets
    const Type      m_AssetType;    // the concrete class of this asset

    Team*           m_Owner;

    unsigned char   m_Width;        // sizes
    unsigned char   m_Height;
    Vector3         m_Position;     // vector defining the absolute position
    Vector3         m_Direction;    // facing of the unit

    IWeapon*        m_Weapon;       // weapon attached to this asset, or null
    IAssetRadar*    m_Radar;        // the logic that handles the inspection of
                                    // the surrounding of the asset

};

#endif // __ASSET_H__
