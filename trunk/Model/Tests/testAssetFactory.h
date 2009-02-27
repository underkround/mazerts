#include "../Asset/AssetFactory.h"
#include "DebugAssetCollectionListener.h"

#include <assert.h>
#include <iostream>
using namespace std;

void testAssetFactory()
{
    cout << " *** TESTING ASSET FACTORY ***\n";
    AssetCollection::debugPrintCollection();

    cout << " - creating few listeners for created & released assets\n";
    DebugAssetCollectionListener* listener1 = new DebugAssetCollectionListener();
    DebugAssetCollectionListener* listener2 = new DebugAssetCollectionListener();
    DebugAssetCollectionListener* listener3 = new DebugAssetCollectionListener();

    cout << " - creating units through factory..\n";
    AssetFactory::createUnit(NULL, AssetFactory::UNIT_TYPE_DEBUG, 2, 2);
    AssetFactory::createUnit(NULL, AssetFactory::UNIT_TYPE_DEBUG, 4, 4);
    AssetFactory::createUnit(NULL, AssetFactory::UNIT_TYPE_DEBUG, 2, 8);

    cout << " - creating buildings through factory..\n";
    AssetFactory::createBuilding(NULL, AssetFactory::BUILDING_TYPE_DEBUG, 10, 10);
    AssetFactory::createBuilding(NULL, AssetFactory::BUILDING_TYPE_DEBUG, 10, 20);
    AssetFactory::createBuilding(NULL, AssetFactory::BUILDING_TYPE_DEBUG, 5, 15);

    // print end stats
    AssetCollection::debugPrintCollection();
    AssetCollection::releaseAll();

    delete listener1;
    delete listener2;
    delete listener3;

    cout << " - total of " << IAsset::getInstanceCount() << " assets created\n";
    cout << " - total of " << IAsset::getInstanceDestructionCount() << " assets deleted\n";
}
