/**
 * Classes implementing IAssetListener can register themselves
 * to Asset and receive notifications of various events of the
 * asset.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __IASSETLISTENER_H__
#define __IASSETLISTENER_H__

#include "IAsset.h"
//class IAsset;

class IAssetListener
{
public:

    /**
     * This gets called when the target assets's state changes.
     * @param pAsset    The asset of which state has changed
     * @param newState  The new state of the asset
     */
    virtual void handleAssetStateChange(IAsset* pAsset, IAsset::State newState) = 0;

    /**
     * This gets called when the asset object is being released & destroyed.
     * This notification will also come through AssetCollection
     * to IAssetCollectionListeners, but if you only need to know
     * weather this specific unit is being destroyed, use this.
     * @param pAsset    Pointer to the unit that is to be released & deleted
     */
    virtual void handleAssetReleased(IAsset* pAsset) = 0;

};

#endif // __IASSETLISTENER_H__
