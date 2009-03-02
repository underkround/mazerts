#include "../Asset/AssetCollection.h"
#include "../Asset/Unit.h"
#include "../Asset/Building.h"

#include "DebugAssetCollectionListener.h"

#include <assert.h>
#include <iostream>
using namespace std;

void testAssetCollection()
{
    Building* b;
    Unit* u;

    // create few assets
    b = new Building();
    b->create();
    b = new Building;
    b->create();
    u = new Unit();
    u->create();
    u = new Unit();
    u->create();

    AssetCollection::debugPrintCollection();

    cout << " - removing 1 building and 1 unit\n";
    AssetCollection::releaseBuilding(b);
    AssetCollection::releaseUnit(u);

    AssetCollection::debugPrintCollection();

    cout << " - total of " << IAsset::getInstanceCount() << " assets created\n";
    cout << " - total of " << IAsset::getInstanceDestructionCount() << " assets deleted\n";

    cout << " - delete remaining assets\n";
    AssetCollection::releaseAll();

    AssetCollection::debugPrintCollection();

    cout << " - total of " << IAsset::getInstanceCount() << " assets created\n";
    cout << " - total of " << IAsset::getInstanceDestructionCount() << " assets deleted\n";

    cout << "\n *** Testing listener ***\n";
    DebugAssetCollectionListener* listener = new DebugAssetCollectionListener();
    // create few assets
    b = new Building;
    b->create();
    u = new Unit();
    u->create();
    b = new Building;
    b->create();
    u = new Unit();
    u->create();
    b = new Building;
    b->create();

    AssetCollection::debugPrintCollection();

    // remove few assets
    AssetCollection::releaseBuilding(b);
    AssetCollection::releaseUnit(u);
    // remove listener
    delete listener;
    // create one more to check the listener was removed
    u = new Unit();
    u->create();
    u = new Unit();
    u->create();
    u = new Unit();
    u->create();

    // all done
    AssetCollection::debugPrintCollection();
    cout << " - delete all units\n";
    AssetCollection::releaseAll();
    AssetCollection::debugPrintCollection();

    cout << endl;
    cout << " - total of " << IAsset::getInstanceCount() << " assets created\n";
    cout << " - total of " << IAsset::getInstanceDestructionCount() << " assets deleted\n";

    cout << "\n\ntesting deletion\n\n";
    u = new Unit(); u->create();
    u = new Unit(); u->create();
    u = new Unit(); u->create();
    u = new Unit(); u->create();
    b = new Building(); b->create();
    b = new Building(); b->create();
    b = new Building(); b->create();
    b = new Building(); b->create();

    AssetCollection::updateUnits(0.5f);
    AssetCollection::updateBuildings(0.5f);
    AssetCollection::updateUnits(0.5f);
    AssetCollection::updateBuildings(0.5f);
    AssetCollection::updateUnits(0.5f);
    AssetCollection::updateBuildings(0.5f);
    AssetCollection::releaseAll();

    cout << " - total of " << IAsset::getInstanceCount() << " assets created\n";
    cout << " - total of " << IAsset::getInstanceDestructionCount() << " assets deleted\n";

}
