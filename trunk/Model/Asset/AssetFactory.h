/**
 * Static class used to create assets to the game.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __ASSETFACTORY_H__
#define __ASSETFACTORY_H__

#include "IAsset.h"
#include "Unit.h"
#include "Building.h"

class AssetFactory
{
public:

    static const int UNIT_TYPE_DEBUG = 0;
    static const int BUILDING_TYPE_DEBUG = 0;

    /**
     * Creates new unit, sets the parameters against unitType
     * @return pointer to new Unit instance
     */
    static Unit* createUnit(Team* owner,
                            int unitType,
                            short positionX,
                            short positionY);

    /**
     * Creates new building, sets the parameters against buildingType
     * @return pointer to new Building instance
     */
    static Building* createBuilding(Team* owner,
                                    int buildingType,
                                    short positionX,
                                    short positionY);

private:

    /**
     * Private ctor and dtor for static class
     */
    AssetFactory();
    ~AssetFactory();

};

#endif // __ASSETFACTORY_H__
