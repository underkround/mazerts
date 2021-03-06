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

// ===== CREATE

    static void create(const unsigned int mapSize);

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
     * Register asset
     * This can safely be called instead of the specifig register-method,
     * it does the same
     */
    static void registerAsset(IAsset* a);

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
     * List of all units in game
     */
    inline static DoubleLinkedList<Unit*>* getAllUnits() { return &units; }

    /**
     * List of all buildings in game
     */
    inline static DoubleLinkedList<Building*>* getAllBuildings() { return &buildings; }

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

    /**
     * Fills provided list with units/buildings/assets in region.
     * @param pList the list to add units to. Doesn't clear the list
     * @param x         rectangle x
     * @param y         rectangle y
     * @param width     rectangle width
     * @param height    rectangle height
     * @return          how many units/buildings/assets found
     */
    static const int getUnitsAt(DoubleLinkedList<Unit*>* pList, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height);
    static const int getBuildingsAt(DoubleLinkedList<Building*>* pList, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height);
    static const int getAssetsAt(DoubleLinkedList<IAsset*>* pList, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height);

    /**
     * Same as above, but gets units/buildings/assets based on player
     * @param player    the id of the player, who owns the units
     */
    static const int getPlayerUnitsAt(DoubleLinkedList<Unit*>* pList, const int player, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height);
    static const int getPlayerBuildingsAt(DoubleLinkedList<Building*>* pList, const int player, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height);
    static const int getPlayerAssetsAt(DoubleLinkedList<IAsset*>* pList, const int player, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height);

// ===== OBSERVER FUNCTIONALITY

    static void registerListener(IAssetCollectionListener* listener);

    static void unregisterListener(IAssetCollectionListener* listener);

// ===== DEBUG
#ifdef _DEBUG
    static void debugPrintCollection();
#endif

// ===== UPDATES

    static void updatePosition(IAsset* a, const unsigned short oldPosX, const unsigned short oldPosY);

    static inline const bool isCreated() { return m_Created; }

private:

    AssetCollection();

    ~AssetCollection();

    /**
     * Centralized method to clear unit and building-arrays when subject is
     * destroyed (since this is used in many places, it needs to be updated on
     * one place only).
     */
    static void clearUnitArray(unsigned short posX, unsigned short posY, unsigned short width, unsigned short height);
    static void clearBuildingArray(unsigned short posX, unsigned short posY, unsigned short width, unsigned short height);

// ===== Members

    // these are the main lists to store the units, other datastructures
    // can be implemented to gain speed in searches etc
    static DoubleLinkedList<Unit*>      units;
    static DoubleLinkedList<Building*>  buildings;

    // pointer arrays of units & buildings. This is not primary unit container,
    // do not delete units through this (use lists)
    static Unit***                      m_pppUnitArray;
    static Building***                  m_pppBuildingArray;
    static unsigned short               m_MapSize; // used to delete arrays

    // assets to be released, waiting here for one frame
    // to avoid broken pointers with compicated update-chain-hell
    static DoubleLinkedList<Unit*>      m_UnitReleaseStack;
    static DoubleLinkedList<Building*>  m_BuildingReleaseStack;

    static bool                         m_Created;

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
