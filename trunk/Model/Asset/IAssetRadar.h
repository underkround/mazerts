/**
 * IAssetRadar's responsibility is to keep track of the interesting
 * subjects in the unit's visibility area, update the players fog-
 * and possibly the terrain -bitmaps.
 *
 * IAssetRadar should advertise the subjects in the range to the
 * unit's other logics, or the logics could ask for list of them
 * from the radar.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __IASSETRADAR_H__
#define __IASSETRADAR_H__

class IAsset;

class IAssetRadar
{
public:

    /**
     * This gets called by the asset when the radar is attached to
     * a asset.
     * DO NOT call this from outside!
     * @see IAsset::setRadar(..)
     */
    virtual void attach(IAsset* host) = 0;

    /**
     * Normal per-frame update call from the hosting unit.
     * @param host      the unit which update-call this is
     * @param deltaT    time from last update
     */
    virtual void update(IAsset* host, const float deltaT) = 0;

    /**
     * Return true, if the concrete radar is spesific to hosting asset
     * (one radar-instance per asset-instance)
     * When this returns true, the host will destroy this instance when
     * returned.
     * If the same radar-instance is used with multiple asset-instances,
     * this should return false, and the host will not destroy this
     * radar instance.
     */
    virtual bool release(IAsset* host) = 0;

};

#endif // __IASSETRADAR_H__
