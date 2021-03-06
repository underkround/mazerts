/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "AssetFactory.h"
#include "AssetCollection.h"
#include "IAsset.h"
#include "Unit.h"
#include "Building.h"
#include "OreMine.h"
#include "MoneyPlant.h"
#include "RadarBuilding.h"
#include "WarFactory.h"
#include "GroundMovingLogic.h"
#include "Radar.h"
#include "../Weapon/Weapon.h"
#include "../Weapon/ControlledWeapon.h"
#include "Resourcer.h"
#include "../Player/PlayerManager.h"

#include "../Defs/DefManager.h"
#include "../Defs/Defs.h"


Unit* AssetFactory::createUnit(Player* owner, int unitType, Building* building)
{
    Unit* u = NULL;

    //check if factory is empty
    int x = building->getDef()->gridEntrancePointX + building->getGridX();
    int y = building->getDef()->gridEntrancePointY + building->getGridY();
    int w = building->getDef()->gridPassableAreaWidth;
    int h = building->getDef()->gridPassableAreaHeight;
    if (AssetCollection::getUnitsAt(NULL, x, y, w, h) == 0)
    {
        //actually create the unit
        u = createUnit( owner,
                    unitType,
                    building->getGridX() + building->getDef()->gridEntrancePointX,
                    building->getGridY() + building->getDef()->gridEntrancePointY
                    );
        //give newly created unit orders to drive out (weaponless units know by themselves what to do)
        if(u->hasWeapon())
        u->getMovingLogic()->addTarget( new Target( ((WarFactory*)building)->getExtractionPointX(),
                                                    ((WarFactory*)building)->getExtractionPointY(),
                                                    false,
                                                    0)
                                       );
    }

    return u;
}

Unit* AssetFactory::createUnit(Player* owner, int unitType, short positionX, short positionY)
{
    // TODO: What to do if there's already unit / building?
    DefManager* dm = DefManager::getInstance();
    AssetDef* def = dm->getAssetDef(unitType);
    if(!def)
        return NULL; // invalid asset type
    if(def->concreteType != 1)
        return NULL; // asset is not unit

    Unit* u = new Unit( (AssetDef&)(*def) );
    u->setOwner(owner);
    u->forcePosition(positionX, positionY);

    // add components
    setRadar(u);
    setResourcer(u);
    setWeapon(u);
    setMoving(u);

    u->create();

    return u;
}

Building* AssetFactory::createBuilding(Player* owner, int buildingType, short positionX, short positionY)
{
    // TODO: What to do if there's already unit / building?
    DefManager* dm = DefManager::getInstance();
    AssetDef* def = dm->getAssetDef(buildingType);
    if(!def)
        return NULL; // invalid asset type
    if(def->concreteType != 2)
        return NULL; // type is not building

    Building* b = NULL;
    if(buildingType != BUILDING_TYPE_FACTORY)
    {
        b = new Building((AssetDef&)(*def));
    }
    else {
        b = new WarFactory((AssetDef&)(*def));
    }
    b->setOwner(owner);
    b->forcePosition(positionX, positionY);

    // add components
    setRadar(b);
    setResourcer(b);
    setWeapon(b);

    b->create();

    //resources yard bundles one harvester unit
    if(buildingType == BUILDING_TYPE_YARD)
    {
        createUnit( owner,
                    UNIT_TYPE_MINER,
                    positionX + b->getDef()->gridEntrancePointX,
                    positionY + b->getDef()->gridEntrancePointY
                    );
    }

    return b;
}

IAsset* AssetFactory::createAsset(Player* owner, int assetType, short positionX, short positionY)
{
    // TODO: What to do if there's already unit / building?
    AssetDef* def = DefManager::getInstance()->getAssetDef(assetType);
    if(!def)
        return NULL; // invalid assetType

    // special case for ore mine
    // TODO: changed here from gamestate, temporary solution for Levels-demo. Do it better :)
    if(assetType == BUILDING_TYPE_MINE)
        return createOreMine(positionX, positionY);

    if(assetType == BUILDING_TYPE_BORE)
        return createDeepBoreMine(owner, positionX, positionY);

    if(assetType == BUILDING_TYPE_RADAR)
        return createRadarBuilding(owner, positionX, positionY);

    // general Units and Buildings
    switch(def->concreteType)
    {
    case 1:
        return createUnit(owner, assetType, positionX, positionY);
    case 2:
        return createBuilding(owner, assetType, positionX, positionY);
    default:
        return NULL;
    }
}


// TODO when more time: the ore mine overrides two behaviours:
//  1) it doesn't register to collection
//  2) it cannot be destroyed
//  3) it's anonymous
// -> 1 and 2 could be implemented with def parameters, and 3) is not
//    really needed (same as to pass player 0 to createAsset or createBuilding)
OreMine* AssetFactory::createOreMine(short positionX, short positionY)
{
    // TODO: What to do if there's already unit / building?
    DefManager* dm = DefManager::getInstance();
    AssetDef* def = dm->getAssetDef(BUILDING_TYPE_MINE);
    if(!def)
        return NULL; // invalid asset type
    if(def->concreteType != 2)
        return NULL; // type is not building

    OreMine* b = new OreMine((AssetDef&)(*def));
    b->setOwner(PlayerManager::getPlayer(0));
    b->forcePosition(positionX, positionY);

    b->create();
    return b;
}

MoneyPlant* AssetFactory::createDeepBoreMine(Player* owner, short positionX, short positionY)
{
    // TODO: What to do if there's already unit / building?
    DefManager* dm = DefManager::getInstance();
    AssetDef* def = dm->getAssetDef(BUILDING_TYPE_BORE);
    if(!def)
        return NULL; // invalid asset type
    if(def->concreteType != 2)
        return NULL; // type is not building

    MoneyPlant* b = new MoneyPlant((AssetDef&)(*def));
    b->setOwner(owner);
    b->forcePosition(positionX, positionY);

    setRadar(b);

    b->create();
    return b;
}

RadarBuilding* AssetFactory::createRadarBuilding(Player* owner, short positionX, short positionY)
{
    // TODO: What to do if there's already unit / building?
    DefManager* dm = DefManager::getInstance();
    AssetDef* def = dm->getAssetDef(BUILDING_TYPE_RADAR);
    if(!def)
        return NULL; // invalid asset type
    if(def->concreteType != 2)
        return NULL; // type is not building

    RadarBuilding* b = new RadarBuilding((AssetDef&)(*def));
    b->setOwner(owner);
    b->forcePosition(positionX, positionY);

    setRadar(b);

    b->create();
    return b;
}



// ===== Set components based on asset's definition

bool AssetFactory::setRadar(IAsset* a)
{
    RadarDef* def = a->getDef()->pDefRadar;
    if(!def)
        return false; // no radar associated
    switch(def->concreteType)
    {
    case 1:
        a->setRadar(new Radar((RadarDef&)(*def)));
        break;
    default:
        return false;
    }
    return true;
}

bool AssetFactory::setResourcer(IAsset* a)
{
    ResourcerDef* def = a->getDef()->pDefResourcer;
    //Only units can have resourcers
    if(!def || a->getAssetType() != IAsset::UNIT)
        return false; // no resourcer associated
   
    Unit* pUnit = (Unit*)a;
    
    switch(def->concreteType)
    {
    case 1:
        pUnit->setResourcer(new Resourcer(pUnit, (ResourcerDef&)(*def)));
        break;
    default:
        return false;
    }
    return true;
}

bool AssetFactory::setWeapon(IAsset* a)
{
    WeaponDef* def = a->getDef()->pDefWeapon;
    if(!def)
        return false; // no projectile associated

    switch(def->concreteType)
    {
    case 1:
        a->setWeapon(new Weapon((WeaponDef&)(*def)));
        break;
    case 2:
        a->setWeapon(new ControlledWeapon((WeaponDef&)(*def)));
        break;
    default:
        return false;
    }
    return true;
}

bool AssetFactory::setMoving(Unit* u)
{
    MovingDef* def = u->getDef()->pDefMoving;
    if(!def)
        return false;
    switch(def->concreteType)
    {
    case 1:
        u->setMovingLogic(new GroundMovingLogic((MovingDef&)(*def)));
        break;
    default:
        return false;
    }
    return true;
}
