/**
 * Declarations of asset-related definition-structs/classes go here.
 *
 * NOTE: although these are structs with public members, (currently) none of
 * the members are meant to be modified outside the DefManager (or possibly
 * the AssetFactory).
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __DEFS_H__
#define __DEFS_H__

//#include "../Asset/IAsset.h"
#include <string>
using namespace std;

/*
struct BaseDef
{
    int tag; // id for definition
    int concreteType; // concrete type for the class to implement this
};
*/

/**
 * WeaponDef goes inside the IWeapon, the weapon then uses
 * the definition to init and launch projectiles.
 */
struct WeaponDef //: public BaseDef
{
    int tag; // id for definition
    int concreteType; // the weapon concrete class type
    int projectileConcreteType; // the projectile concrete class type

    string name;
    // how many rounds it can fire before reloading (0 for constant fire)
    int clipSize;
    // how long it takes (seconds) to reload
    float reloadTime;
    // rate of fire, time (seconds) between the launched projectiles
    float rof;
    // range of this projectile
    float range;
    // turning speed
    float turnSpeed;

    //Damage caused by single projectile
    int damage;

    //damage range
    int damageRange;
};


/**
 * RadarDef goes inside every IAssetRadar -class
 */
struct RadarDef //: public BaseDef
{
    int tag; // id for definition
    int concreteType; // concrete type for the class to implement this

    float range;
};


/**
 * MovingDef goes inside every IMovingLogic -class
 */
struct MovingDef //: public BaseDef
{
    int tag; // id for definition
    int concreteType; // concrete type for the class to implement this

    float turningSpeed;
    float maxSpeed;
    float acceleration;
};


/**
 * BuilderDef goes inside builder-component that enables the ability to
 * build and/or fix stuff (grow hitpoints)
 */
struct BuilderDef //: public BaseDef
{
    int tag; // id for definition
    int concreteType; // concrete type for the class to implement this

};


/**
 * ResourcerDef is for resourcer-component, unit with resourcer-component
 * is capable of extracting ore from mines
 */
struct ResourcerDef
{
    int tag; // id for definition
    int concreteType; // concrete type for the class to implement this

    // storing capacity
    int oreCapacity;

    //Loading time
    float loadTime;
};


/**
 * AssetDef goes inside every IAsset-class.
 */
struct AssetDef //: public BaseDef
{
    int tag; // id for definition
    int concreteType; // concrete type for the class to implement this

    // name of the asset, used only in ui, not with logics
    string name;
    // brief description of the asset to display on ui-side, maybe on tooltip
    string description;

    // maximum hitpoints for asset
    int maxHitpoints;
    // lasting of the destroyed -state (seconds) before object deletion
    float destructionTime;
    // size parameters
    unsigned char width;
    unsigned char height;

    // definitions for energy and resource
    int constructionCostOre;
    int constructionCostEnergy; //?!

    int energyConsumption;
    int energyProduction;

    // for building-types only:
    // this is a pretty much hack, but hey, !IF! it works  ;)
    int gridPassableAreaX;
    int gridPassableAreaY;
    int gridPassableAreaWidth;
    int gridPassableAreaHeight;
    int gridEntrancePointX;
    int gridEntrancePointY;

    int threat;

    // TODO: damage resistance values

    // bindings to other definitions
    // 0 for none, or tag number to bind for definition
    // TODO: these could also be pointers..
/*
    int radarTag;       // radar-component
    int weaponTag;      // projectile/weapon-component
    int movingTag;      // moving-component
    int builderTag;     // builder-component
    int resourcerTag;   // resourcer-component
*/
    RadarDef*       pDefRadar;
    MovingDef*      pDefMoving;
    WeaponDef*      pDefWeapon;
    BuilderDef*     pDefBuilder;
    ResourcerDef*   pDefResourcer;
};


#endif // __DEFS_H__
