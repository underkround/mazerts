#include "../Asset/AssetCollection.h"
#include "../Asset/Unit.h"
#include "../Asset/Building.h"
#include "../Asset/Assetfactory.h"
#include "../Player/Player.h"

#include "DebugAssetCollectionListener.h"

#include <assert.h>
#include <iostream>
using namespace std;

void testAssetCollection()
{
    Building* b;
    Unit* u;
    AssetCollection::create(Terrain::getInstance()->getSize());

    // create few assets
    b = new Building();
    b->forcePosition(0, 0);
    b->create();
    b = new Building;
    b->forcePosition(0, 0);
    b->create();
    u = new Unit();
    u->forcePosition(0, 0);
    u->create();
    u = new Unit();
    u->forcePosition(0, 0);
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
    AssetCollection::create(Terrain::getInstance()->getSize());

    AssetCollection::debugPrintCollection();

    cout << " - total of " << IAsset::getInstanceCount() << " assets created\n";
    cout << " - total of " << IAsset::getInstanceDestructionCount() << " assets deleted\n";

    cout << "\n *** Testing listener ***\n";
    DebugAssetCollectionListener* listener = new DebugAssetCollectionListener();
    // create few assets
    b = new Building;
    b->forcePosition(0, 0);
    b->create();
    u = new Unit();
    u->forcePosition(0, 0);
    u->create();
    b = new Building;
    b->forcePosition(0, 0);
    b->create();
    u = new Unit();
    u->forcePosition(0, 0);
    u->create();
    b = new Building;
    b->forcePosition(0, 0);
    b->create();

    AssetCollection::debugPrintCollection();

    // remove few assets
    AssetCollection::releaseBuilding(b);
    AssetCollection::releaseUnit(u);
    // remove listener
    delete listener;
    // create one more to check the listener was removed
    u = new Unit();
    u->forcePosition(0, 0);
    u->create();
    u = new Unit();
    u->forcePosition(0, 0);
    u->create();
    u = new Unit();
    u->forcePosition(0, 0);
    u->create();

    // all done
    AssetCollection::debugPrintCollection();
    cout << " - delete all units\n";
    AssetCollection::releaseAll();
    AssetCollection::debugPrintCollection();
    AssetCollection::create(Terrain::getInstance()->getSize());

    cout << endl;
    cout << " - total of " << IAsset::getInstanceCount() << " assets created\n";
    cout << " - total of " << IAsset::getInstanceDestructionCount() << " assets deleted\n";

    cout << "\n\ntesting deletion\n\n";
    u = new Unit(); u->forcePosition(0, 0); u->create();
    u = new Unit(); u->forcePosition(0, 0); u->create();
    u = new Unit(); u->forcePosition(0, 0); u->create();
    u = new Unit(); u->forcePosition(0, 0); u->create();
    b = new Building(); b->forcePosition(0, 0); b->create();
    b = new Building(); b->forcePosition(0, 0); b->create();
    b = new Building(); b->forcePosition(0, 0); b->create();
    b = new Building(); b->forcePosition(0, 0); b->create();

    AssetCollection::updateUnits(0.5f);
    AssetCollection::updateBuildings(0.5f);
    AssetCollection::updateUnits(0.5f);
    AssetCollection::updateBuildings(0.5f);
    AssetCollection::updateUnits(0.5f);
    AssetCollection::updateBuildings(0.5f);
    AssetCollection::releaseAll();
    AssetCollection::create(Terrain::getInstance()->getSize());

    cout << " - total of " << IAsset::getInstanceCount() << " assets created\n";
    cout << " - total of " << IAsset::getInstanceDestructionCount() << " assets deleted\n";

    cout << "\n\ntesting searching\n\n";
    Player p(1);
    /**
    layout of test units and buildings
  00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19
00u  u  u  u                                   b  b  b  b
01u  u  u  u                                   b  b  b  b
02u  u  u  u                                   b  b  b  b
03u  u  u  u
04
05b  b  b  b     u  u  u  u
06b  b  b  b     u  u  u  u
07b  b  b  b     u  u  u  u
08               u  u  u  u
09
10               b  b  b  b     u  u  u  u
11               b  b  b  b     u  u  u  u
12               b  b  b  b     u  u  u  u
13                              u  u  u  u
14
15                              b  b  b  b     u  u  u  u
16                              b  b  b  b     u  u  u  u
17                              b  b  b  b     u  u  u  u
18                                             u  u  u  u
19
*/
    u = AssetFactory::createUnit(&p, AssetFactory::UNIT_TYPE_DEBUG, 0, 0);
    u = AssetFactory::createUnit(&p, AssetFactory::UNIT_TYPE_DEBUG, 5, 5);
    u = AssetFactory::createUnit(&p, AssetFactory::UNIT_TYPE_DEBUG, 10, 10);
    u = AssetFactory::createUnit(&p, AssetFactory::UNIT_TYPE_DEBUG, 15, 15);
    b = AssetFactory::createBuilding(&p, AssetFactory::BUILDING_TYPE_DEBUG, 0, 5);
    b = AssetFactory::createBuilding(&p, AssetFactory::BUILDING_TYPE_DEBUG, 5, 10);
    b = AssetFactory::createBuilding(&p, AssetFactory::BUILDING_TYPE_DEBUG, 10, 15);
    b = AssetFactory::createBuilding(&p, AssetFactory::BUILDING_TYPE_DEBUG, 15, 0);

    IAsset* a = NULL;
    DoubleLinkedList<Unit*> listU;
    DoubleLinkedList<Building*> listB;
    DoubleLinkedList<IAsset*> listA;
    int amount = 0;

    u = AssetCollection::getUnitAt(1, 1); assert(u);
    u = AssetCollection::getUnitAt(10, 1); assert(!u);
    u = AssetCollection::getUnitAt(10, 6); assert(!u);
    b = AssetCollection::getBuildingAt(0, 0); assert(!b);
    b = AssetCollection::getBuildingAt(6, 2); assert(!b);
    b = AssetCollection::getBuildingAt(1, 6); assert(b);
    a = AssetCollection::getAssetAt(0, 0); assert(a);
    a = AssetCollection::getAssetAt(2, 16); assert(!a);
    a = AssetCollection::getAssetAt(6, 11); assert(a);
    
    amount = AssetCollection::getUnitsAt(&listU, 0, 0, 11, 11);
    assert(amount == 3); assert(listU.count() == 3);
    listU.release();
    amount = AssetCollection::getUnitsAt(&listU, 5, 0, 4, 4);
    assert(amount == 0); assert(listU.count() == 0);
    listU.release();

    amount = AssetCollection::getBuildingsAt(&listB, 0, 0, 11, 11);
    assert(amount == 2); assert(listB.count() == 2);
    listB.release();
    amount = AssetCollection::getBuildingsAt(&listB, 0, 15, 10, 10);
    assert(amount == 0); assert(listB.count() == 0);
    listB.release();

    amount = AssetCollection::getAssetsAt(&listA, 0, 0, 11, 11);
    assert(amount == 5); assert(listA.count() == 5);
    listA.release();
    amount = AssetCollection::getAssetsAt(&listA, 9, 5, 10, 5);
    assert(amount == 0); assert(listA.count() == 0);
    listA.release();

    amount = AssetCollection::getPlayerUnitsAt(&listU, 2, 0, 0, 11, 11);
    assert(amount == 0); assert(listU.count() == 0);
    listU.release();
    amount = AssetCollection::getPlayerUnitsAt(&listU, 1, 0, 0, 11, 11);
    assert(amount == 3); assert(listU.count() == 3);
    listU.release();
    amount = AssetCollection::getPlayerBuildingsAt(&listB, 2, 0, 0, 11, 11);
    assert(amount == 0); assert(listB.count() == 0);
    listB.release();
    amount = AssetCollection::getPlayerBuildingsAt(&listB, 1, 0, 0, 11, 11);
    assert(amount == 2); assert(listB.count() == 2);
    listB.release();
    amount = AssetCollection::getPlayerAssetsAt(&listA, 2, 0, 0, 11, 11);
    assert(amount == 0); assert(listA.count() == 0);
    listA.release();
    amount = AssetCollection::getPlayerAssetsAt(&listA, 1, 0, 0, 11, 11);
    assert(amount == 5); assert(listA.count() == 5);
    listA.release();


    AssetCollection::releaseAll();
}
