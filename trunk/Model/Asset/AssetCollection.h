/**
 * Collection to handle lifetime of the assets and provide interfaces
 * to get access to them.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __ASSETCOLLECTION_H__
#define __ASSETCOLLECTION_H__

#include "../common.h"
#include "../Common/DoubleLinkedList.h"

#include "IAssetCollectionListener.h"

class Unit;
class Building;

class AssetCollection
{
public:

    //static AssetCollection* getInstance(); // @DEPRECATED: not singleton anymore

// ===== UPDATE

    /**
     * Update every existing building
     */
    static void updateBuildings(const float deltaT);

    /**
     * Update every existing unit
     */
    static void updateUnits(const float deltaT);

// ===== MANAGEMENT

    /**
     * Register new unit to collection
     */
    static void registerUnit(Unit* u);

    /**
     * Register new building to collection
     */
    static void registerBuilding(Building* b);

    /**
     * Release and delete given object
     */
    static void releaseUnit(Unit* u);

    /**
     * Release and delete given object
     */
    static void releaseBuilding(Building* b);

    /**
     * Release all units, delete the objects
     */
    static void releaseAll();

// ===== SEARCHING

    /**
     * Return asset at coordinate, if any
     */
    static IAsset* getAssetAt(const unsigned short x, const unsigned short y);

    /**
     * Return unit at coordinate, or NULL if none
     */
    static Unit* getUnitAt(const unsigned short x, const unsigned short y);

    /**
     * Return building at coordinate, or NULL if none
     */
    static Building* getBuildingAt(const unsigned short x, const unsigned short y);

// ===== OBSERVER FUNCTIONALITY

    static void registerListener(IAssetCollectionListener* listener);

    static void unregisterListener(IAssetCollectionListener* listener);

// ===== DEBUG

    static void debugPrintCollection();

private:

    AssetCollection();

    ~AssetCollection();

    // these are the main lists to store the units, other datastructures
    // can be implemented to gain speed in searches etc
    static DoubleLinkedList<Unit*>      units;
    static DoubleLinkedList<Building*>  buildings;

// ===== OBSERVER FUNCTIONALITY

    // list of registered listeners
    static DoubleLinkedList<IAssetCollectionListener*> listeners;

    /**
     * Notify listeners about asset being created
     */
    static void notifyAssetCreated(IAsset* created);

    /**
     * Notify listeners about asset being destroyed
     */
    static void notifyAssetReleased(IAsset* released);

};

#endif // __ASSETCOLLECTION_H__
