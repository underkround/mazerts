/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "AssetFactory.h"
#include "IAsset.h"
#include "Unit.h"
#include "Building.h"
#include "OreMine.h"
#include "GroundMovingLogic.h"
#include "Radar.h"
#include "../Weapon/Weapon.h"
#include "Resourcer.h"
#include "../Player/PlayerManager.h"

#include "../Defs/DefManager.h"
#include "../Defs/Defs.h"

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
    setBuilder(u);
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

    Building* b = new Building((AssetDef&)(*def));
    b->setOwner(owner);
    b->forcePosition(positionX, positionY);

    // add components
    setRadar(b);
    setBuilder(b);
    setResourcer(b);
    setWeapon(b);

    b->create();
    return b;
}


IAsset* AssetFactory::createAsset(Player* owner, int assetType, short positionX, short positionY)
{
    // TODO: What to do if there's already unit / building?
    AssetDef* def = DefManager::getInstance()->getAssetDef(assetType);
    if(!def)
        return NULL; // invalid assetType
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

OreMine* AssetFactory::createOreMine(short positionX, short positionY)
{    
    // TODO: What to do if there's already unit / building?
    DefManager* dm = DefManager::getInstance();
    //TODO: Hardcoded-value, any reasonable way to get from defs-file?
    AssetDef* def = dm->getAssetDef(51);
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

bool AssetFactory::setBuilder(IAsset* a)
{
    BuilderDef* def = a->getDef()->pDefBuilder;
    if(!def)
        return false; // no builder associated
    // Waiting for concrete classes
    /*
    BuilderDef* def = a->getDef()->pDefBuilder;
    */
    return false;
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
