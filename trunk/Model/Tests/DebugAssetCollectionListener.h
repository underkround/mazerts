/**
 * Demonstrate & test IAssetCollectionListener -interface
 */
#ifndef __DEBUGASSETCOLLECTIONLISTNER_H__
#define __DEBUGASSETCOLLECTIONLISTNER_H__

#include "../Asset/IAsset.h"
#include "../Asset/IAssetCollectionListener.h"

#include <iostream>
using namespace std;

class DebugAssetCollectionListener : public IAssetCollectionListener
{
public:

    DebugAssetCollectionListener() {
        AssetCollection::registerListener(this);
        cout << "COLLECTION LISTENER: registered\n";
    }

    ~DebugAssetCollectionListener() {
        AssetCollection::unregisterListener(this);
        cout << "COLLECTION LISTENER: unregistered\n";
    }

    void handleCreatedAsset(IAsset* instance)
    {
        cout    << "COLLECTION LISTENER: asset (assettype " << instance->getAssetType()
                << ", iid " << instance->getIID() << ") was created\n";
    }

    void handleReleasedAsset(IAsset* instance)
    {
        cout    << "COLLECTION LISTENER: asset (assettype " << instance->getAssetType()
                << ", iid " << instance->getIID() << ") is being destoyed\n";
    }

};



#endif // __DEBUGASSETCOLLECTIONLISTNER_H__
