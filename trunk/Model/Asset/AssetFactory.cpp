/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "AssetFactory.h"
#include "GroundMovingLogic.h"

Unit* AssetFactory::createUnit(Player* owner, int unitType, short positionX, short positionY)
{
    // create unit
    Unit* u = new Unit(unitType);
    u->setOwner(owner);

    /**
     * Setup unit's type-specifig properties
     */
    switch(unitType)
    {
    default:
    case AssetFactory::UNIT_TYPE_DEBUG:
        // create unit & set it's properties
        u->setWidth(2);
        u->setHeight(2);
        //u->setWeapon(NULL); // TODO
        u->setMovingLogic(new GroundMovingLogic()); // TODO
        break;
    }

    u->forcePosition(positionX, positionY);
    // register unit to collection & return it
    u->create();
    return u;
}


Building* AssetFactory::createBuilding(Player* owner, int buildingType, short positionX, short positionY)
{
    Building* b = new Building(buildingType);
    b->setOwner(owner);

    /**
     * Setup building's type-specifig properties
     */
    switch(buildingType)
    {
    default:
    case AssetFactory::BUILDING_TYPE_DEBUG:
        b->setWidth(4);
        b->setHeight(3);
        //b->setWeapon(NULL); // only on cannon towers etc
    }

    b->forcePosition(positionX, positionY);
    b->create();
    return b;
}
