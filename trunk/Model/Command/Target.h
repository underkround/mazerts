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
#include "../Asset/IAssetListener.h"

class Target : public IAssetListener
{
public:

    enum TargetType
    {
        COORDINATE,
        ASSET
        //UNIT,
        //BUILDING
    };

    enum TargetFlags
    {
        TGTFLAG_MAKEWAY = 1,     //When forcing units to move out of the way
        TGTFLAG_FORCEATTACK      //For forcing weapons to attack ground
    };

    /**
     * Create target as coordinate target. If contextSensitive flag is set
     * to true, search for asset in the grid and lock to it if found.
     */
    Target(const unsigned short x, const unsigned short y, const bool contextSensitive)
    {
        m_TargetAsset = NULL;
        m_TargetX = m_TargetY = 0;
        m_Range = 0.0f;
        setTarget(x, y, contextSensitive);
        m_Flags = 0;
    }

    /**
     * Create target that is locked to given asset.
     */
    Target(IAsset* target)
    {
        m_TargetAsset = NULL;
        m_TargetX = m_TargetY = 0;
        m_Range = 0.0f;
        setTarget(target);
    }

    /**
     * Copy constructor
     */
    Target(const Target& other)
    {
        m_Range = other.m_Range;
        m_Flags = other.m_Flags;
        m_TargetX = other.m_TargetX;
        m_TargetY = other.m_TargetY;
        m_TargetAsset = NULL;
        m_TargetType = COORDINATE;
        if(other.m_TargetType == ASSET) {
            setTarget(other.m_TargetAsset);
        }
        else {
            m_TargetAsset = NULL;
            m_TargetType = other.m_TargetType;
        }
    }

    ~Target()
    {
    }

    void release()
    {
        clearAssetTarget();
    }

    /**
     * Update this target to track given asset.
     */
    void setTarget(IAsset* target)
    {
        clearAssetTarget();
        if(target)
        {
            // proper target asset given
            m_TargetX = target->getGridX();
            m_TargetY = target->getGridY();
            m_TargetAsset = target;
            m_TargetType = ASSET;
            // register as listener to target (in case asset is destroyed while we are tracking it)
            m_TargetAsset->registerListener(this);
        }
    }

    /**
     * Update the target, lock to given coordinates. If contextSensitive
     * flag is set to true, search for asset in the coordinate, and lock
     * to it if found.
     */
    void setTarget(const unsigned short x, const unsigned short y, const bool contextSensitive)
    {
        if(m_TargetAsset)
            clearAssetTarget();
        m_TargetX = x;
        m_TargetY = y;
        m_TargetAsset = NULL;
        m_TargetType = COORDINATE;
        if(contextSensitive)
        {
            IAsset* newTarget = AssetCollection::getAssetAt(x, y);
            if(newTarget)
            {
                // if we found asset from coordinate, lock to it
                setTarget(newTarget);
            }
        }
        m_Range = 0.0f;
    }

    /**
     * Set range to maintain to the target
     */
    void setRange(float range)
    {
        m_Range = range;
    }

    /**
     * Return the range associated with the target
     */
    const float getRange()
    {
        return m_Range;
    }

    /**
     * Return the type of target we are currently locked to.
     * @return  indication of what this target is tracking
     */
    const TargetType getTargetType() const 
    {
        return m_TargetType;
    }

    /**
     * Return the asset that this target is locked to, if any.
     * If the target is static coordinates, this returns NULL.
     */
    IAsset* getTargetAsset() 
    {
        return m_TargetAsset;
    }

    /**
     * Target is NOT static if it's set to point asset that can move (unit).
     * If the target IS static, it's coordinates are not to change in the
     * lifetime of the target.
     * @return  true, if this target is static, and coordinates are not
     *          subject to change
     */
    const bool isStatic() const 
    {
        return (m_TargetAsset) ? false : true;
    }

    /**
     * Return target's y-coordinate, which is either static with
     * target, or from the target's asset
     */
    const unsigned short getTargetX() const 
    {
        if(m_TargetAsset)
            if(m_TargetType == ASSET)
                return m_TargetAsset->getGridX();
        return m_TargetX;
    }

    /**
     * Return target's y-coordinate, which is either static with
     * target, or from the target's asset
     */
    const unsigned short getTargetY() const 
    {
        if(m_TargetAsset)
            if(m_TargetType == ASSET)
                return m_TargetAsset->getGridY();
        return m_TargetY;
    }

    /**
     * If our target is asset, stop tracking it and set target to coordinates
     * where the target asset was when we lost it (where it is now, that is).
     * This function is safe to call even when we are not locked to asset.
     */
    void clearAssetTarget()
    {
        if(m_TargetAsset)
        {
            // store target from old in case NULL was given as new target
            m_TargetX = m_TargetAsset->getGridX();
            m_TargetY = m_TargetAsset->getGridY();
            // remove us from asset's listeners and clear target asset
            m_TargetAsset->unregisterListener(this);
            m_TargetAsset = NULL;
            m_TargetType = COORDINATE;
        }
    }

// ===== Listener implementation

    virtual void handleAssetStateChange(IAsset* pAsset, IAsset::State newState)
    {
        // not needed
    }

    virtual void handleAssetReleased(IAsset* pAsset)
    {
//        clearAssetTarget();   // DO NOT DO THIS - CONTAINS LISTENER UNREGISTERING, WHICH LEADS TO
                                // ALL HELL BREAKING LOOSE (- 2 hours of debugging behind ^^)
        m_TargetX = m_TargetAsset->getGridX();
        m_TargetY = m_TargetAsset->getGridY();
        m_TargetAsset = NULL;
        m_TargetType = COORDINATE;
    }

    /** 
     * Returns the flags-variable
     */
    inline unsigned int getFlags() { return m_Flags; }

    /**
     * Check if a flag is set
     */ 
    inline bool isFlag(unsigned int flag) 
    { 
        if(m_Flags & flag)
        {
            return true;
        }
        return false;
    }

    /**
     * Set a flag
     */
    inline void setFlag(unsigned int flag)
    {
        m_Flags |= flag;
    }

    /**
     * Unset a flag
     */
    inline void unsetFlag(unsigned int flag)
    {
        m_Flags &= ~flag;
    }

private:

    // range to maintain to the target
    float           m_Range;    

    // determines if the target is asset or coordinate
    TargetType      m_TargetType;

    // target's data comes from either these coordinates...
    unsigned short  m_TargetX;
    unsigned short  m_TargetY;
    // ...or from this target, depending from the target's type
    IAsset*         m_TargetAsset;

    /** 
     * Special purpose-flags
     */
    unsigned int m_Flags;

};

#endif // __TARGET_H__
