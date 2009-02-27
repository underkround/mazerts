/**
 * Target
 *
 * Target is simple container for coordinate/range -pair that can optionally
 * be set to follow an asset. If target is set to point to asset, it returns
 * variable coordinates representing the location of the asset, rather than
 * static terrain coordinates.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __TARGET_H__
#define __TARGET_H__

#include "../Asset/IAsset.h"
#include "../Asset/AssetCollection.h"

class Target
{
public:

    enum Type
    {
        COORDINATE,
        ASSET
        //UNIT,
        //BUILDING
    };

    /**
     * Create target as coordinate target. If contextSensitive flag is set
     * to true, search for asset in the grid and lock to it if found.
     */
    inline Target(short x, short y, bool contextSensitive)
    {
        setTarget(x, y, contextSensitive);
    }

    /**
     * Create target that is locked to given asset.
     */
    inline Target(IAsset* target)
    {
        setTarget(target);
    }

    ~Target()
    {
    }

    /**
     * Update the target by locking to given asset.
     */
    inline void setTarget(IAsset* target)
    {
        m_TargetX = target->getGridX();
        m_TargetY = target->getGridY();
        m_TargetAsset = target;
    }

    /**
     * Update the target, lock to given coordinates. If contextSensitive
     * flag is set to true, search for asset in the coordinate, and lock
     * to it if found.
     */
    inline void setTarget(short x, short y, bool contextSensitive)
    {
        m_TargetX = x;
        m_TargetY = y;
        m_TargetAsset = NULL;
//        if(contextSensitive)
//            m_TargetAsset = AssetCollection::getAssetAt(x, y);
    }

    /**
     * Return the type of this target
     */
    inline Type getType()           { return m_TargetType; }

    /**
     * Return the asset that this target is locked to, if any.
     * If the target is static coordinates, this returns NULL.
     */
    inline IAsset* getTargetAsset() { return m_TargetAsset; }

    /**
     * Target is NOT static if it's set to point asset that can move (unit).
     * If the target IS static, it's coordinates are not to change in the
     * lifetime of the target.
     * @return  true, if this target is static, and coordinates are not
     *          subject to change
     */
    inline bool isStatic()          { return (m_TargetAsset) ? false : true; }

    /**
     * Return target's y-coordinate, which is either static with
     * target, or from the target's asset
     */
    inline short getTargetX() {
        if(m_TargetType == ASSET && m_TargetAsset)
            return m_TargetAsset->getGridX();
        return m_TargetX;
    }

    /**
     * Return target's y-coordinate, which is either static with
     * target, or from the target's asset
     */
    inline short getTargetY() {
        if(m_TargetType == ASSET && m_TargetAsset)
            return m_TargetAsset->getGridY();
        return m_TargetY;
    }

private:

    // determines if the target is asset or coordinate
    Type        m_TargetType;

    // target's data comes from either these coordinates...
    short       m_TargetX;
    short       m_TargetY;
    // ...or from this target, depending from the target's type
    IAsset*     m_TargetAsset;

};

#endif // __TARGET_H__
