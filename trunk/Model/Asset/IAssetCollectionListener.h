/**
 * Classes implementing IAssetCollectionListener can register themselves
 * to AssetCollection and receive notifications of the create & deleted
 * asset instances.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __IASSETCOLLECTIONLISTENER_H__
#define __IASSETCOLLECTIONLISTENER_H__

class IAsset;

class IAssetCollectionListener
{
public:
    /**
     * @TODO: notification of the assets entered into destroyed-state?
     *        (waiting x seconds to be destroyed)
     *        or should this go to asset-level notifications? maybe
     */

    /**
     * Handle notification of new asset that has just been created to the game.
     * @param instance  pointer to created new asset
     */
    virtual void handleCreatedAsset(IAsset* instance) = 0;

    /**
     * This gets called when asset object is being destroyed. The given asset
     * is alive while calling this, but is to be destroyed right after this
     * call returns.
     * @param instance  pointer to asset instance to be deteled
     */
    virtual void handleReleasedAsset(IAsset* instance) = 0;

};

#endif // __IASSETCOLLECTIONLISTENER_H__
