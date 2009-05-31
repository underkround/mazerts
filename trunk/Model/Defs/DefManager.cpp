/**
 * Copy 'n paste!
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#include "DefManager.h"

#include "../Common/Config.h"
#include "../Asset/IAsset.h"        // for Type-enum validation
#include "../Asset/IAssetRadar.h"   // for Type-enum validation
#include "../Asset/IMovingLogic.h"  // for Type-enum validation
#include "../Weapon/IWeapon.h"      // for Type-enum validation
#include "../Weapon/Projectile.h"   // for Type-enum validation
#include "../Asset/Resourcer.h"     // for Type-enum validation

DefManager::DefManager()
{
}

DefManager::~DefManager()
{
    // delete loaded defs
    {
        ListNode<AssetDef*>* node = m_AssetDefs.headNode();
        while(node) {
            delete node->item;
            node = m_AssetDefs.removeGetNext(node);
        }
    }
    {
        ListNode<MovingDef*>* node = m_MovingDefs.headNode();
        while(node) {
            delete node->item;
            node = m_MovingDefs.removeGetNext(node);
        }
    }
    {
        ListNode<WeaponDef*>* node = m_WeaponDefs.headNode();
        while(node) {
            delete node->item;
            node = m_WeaponDefs.removeGetNext(node);
        }
    }
    {
        ListNode<ResourcerDef*>* node = m_ResourcerDefs.headNode();
        while(node) {
            delete node->item;
            node = m_ResourcerDefs.removeGetNext(node);
        }
    }
    {
        ListNode<RadarDef*>* node = m_RadarDefs.headNode();
        while(node) {
            delete node->item;
            node = m_RadarDefs.removeGetNext(node);
        }
    }
}

void DefManager::loadConfigurations()
{
    Config* c = Config::getInstance();
//    c.setFilename("../data/defs/startup.ini");
//    c.readFile();
    c->setFilename("../data/defs/assets.ini");
    c->readFile();
    c->setFilename("../data/defs/movings.ini");
    c->readFile();
    c->setFilename("../data/defs/weapons.ini");
    c->readFile();
    c->setFilename("../data/defs/resourcers.ini");
    c->readFile();

    // Load all declared asset definitions
    Config::ValueNode* node = c->getNode("def declarations", "declared assets");
    while(node)
    {
        loadAssetDef(node->getInt());
        node = node->next;
    }
}

/**
 * Load asset definitions
 */
bool DefManager::loadAssetDef(int tag)
{
    Config* c = Config::getInstance();
    string tags = intToString(tag);
    AssetDef* d = getAssetDef(tag);
    bool isNew = false;
    if(!d) {
        d = new AssetDef();
        ::memset(d, 0, sizeof(AssetDef));
        isNew = true;
    }
    // asset concrete type
    int type = c->getValueAsInt(tags, "asset concrete type");
    // validate the concrete type - it needs to be n^2 value and declared concrete type
    if( ((type & (type - 1)) != 0)  ||  !(type & (IAsset::TYPE_END - 1)) ) {
        // discard this def for invalid concrete type
        if(isNew)
            delete d;
        return false;
    }

    // type ok, continue
    d->concreteType = type;
    d->tag = tag;

    // load values
    d->anonymous            = c->getValueAsBool(    tags, "asset anonymous", false);
    d->isfactory            = c->getValueAsBool(    tags, "asset isfactory", false);

    d->constructionCostEnergy = c->getValueAsInt(   tags, "asset construction cost energy", 0);
    d->constructionCostOre  = c->getValueAsInt(     tags, "asset construction cost ore", 10);
    d->constructionIn       = c->getValueAsInt(     tags, "asset construction in", 0);
    d->constructionRequires = c->getValueAsInt(     tags, "asset construction requires", 0);

    d->gridPassableAreaX    = c->getValueAsInt(     tags, "asset grid passable area x", 0);
    d->gridPassableAreaY    = c->getValueAsInt(     tags, "asset grid passable area y", 0);
    d->gridPassableAreaWidth = c->getValueAsInt(    tags, "asset grid passable area width", 0);
    d->gridPassableAreaHeight = c->getValueAsInt(   tags, "asset grid passable area height", 0);
    d->gridEntrancePointX   = c->getValueAsInt(     tags, "asset grid entrance point x", 0);
    d->gridEntrancePointY   = c->getValueAsInt(     tags, "asset grid entrance point y", 0);

    d->energyProduction     = c->getValueAsInt(     tags, "asset energy production", 0);
    d->energyConsumption    = c->getValueAsInt(     tags, "asset energy consumption", 0);
    
    d->name                 = c->getValueAsString(  tags, "asset name", "unnamed");
    d->description          = c->getValueAsString(  tags, "asset description", "unset description");
    d->maxHitpoints         = c->getValueAsInt(     tags, "asset max hitpoints", 10);
    d->constructionTime     = c->getValueAsFloat(   tags, "asset construction time", 20);
    d->width                = c->getValueAsInt(     tags, "asset width", 2);
    d->height               = c->getValueAsInt(     tags, "asset height", 2);

    d->threat               = c->getValueAsInt(     tags, "asset threat", 1);
    d->basematerial         = c->getValueAsInt(     tags, "asset base material index", 0);

    // load component definitions for asset
    int t;

//    t = c->getValueAsInt(tags, "asset radar tag");
    // radar goes in same section with asset -> same id 
    t = c->getValueAsInt(tags, d->tag);
    if(t && loadRadarDef(t))
        d->pDefRadar = getRadarDef(t);
    else
        d->pDefRadar = NULL;

    t = c->getValueAsInt(tags, "asset moving tag");
    if(t && loadMovingDef(t))
        d->pDefMoving = getMovingDef(t);
    else
        d->pDefMoving = NULL;

    t = c->getValueAsInt(tags, "asset weapon tag");
    if(t && loadWeaponDef(t))
        d->pDefWeapon = getWeaponDef(t);
    else
        d->pDefWeapon = NULL;

    t = c->getValueAsInt(tags, "asset resourcer tag");
    if(t && loadResourcerDef(t))
        d->pDefResourcer = getResourcerDef(t);
    else
        d->pDefResourcer = NULL;

    // store if new
    if(isNew)
        m_AssetDefs.pushHead(d);
    return true;
}


/**
 * Load moving definitions
 */
bool DefManager::loadMovingDef(int tag)
{
    Config* c = Config::getInstance();
    string tags = intToString(tag);
    MovingDef* d = getMovingDef(tag);
    bool isNew = false;
    if(!d) {
        d = new MovingDef();
        ::memset(d, 0, sizeof(MovingDef));
        isNew = true;
    }
    int type = c->getValueAsInt(tags, "moving concrete type");

    // validate the concrete type - it needs to be n^2 value and declared concrete type
    if( ((type & (type - 1)) != 0)  ||  !(type & (IMovingLogic::TYPE_END - 1)) ) {
        // discard this def for invalid concrete type
        if(isNew) delete d;
        return false;
    }
    d->concreteType = type;
    d->tag = tag;

    // load values
    d->acceleration = c->getValueAsFloat(tags, "moving acceleration", 0.9f);
    d->maxSpeed     = c->getValueAsFloat(tags, "moving maximum speed", 1.0f);
    d->turningSpeed = c->getValueAsFloat(tags, "moving turning speed", 1.0f);

    // store if new
    if(isNew)
        m_MovingDefs.pushHead(d);
    return true;
}


/**
 * Load weapon definitions
 */
bool DefManager::loadWeaponDef(int tag)
{
    Config* c = Config::getInstance();
    string tags = intToString(tag);
    WeaponDef* d = getWeaponDef(tag);
    bool isNew = false;
    if(!d) {
        d = new WeaponDef();
        ::memset(d, 0, sizeof(WeaponDef));
        isNew = true;
    }

    // weapon concrete type
    int type = c->getValueAsInt(tags, "weapon concrete type");
    // validate the concrete type - it needs to be n^2 value and declared concrete type
    if( ((type & (type - 1)) != 0)  ||  !(type & (IWeapon::TYPE_END - 1)) ) {
        // discard this def for invalid concrete type
        if(isNew) delete d;
        return false;
    }
    d->concreteType = type;

    // projectile concrete type
    type = c->getValueAsInt(tags, "weapon projectile concrete type");
    // validate the concrete type - it needs to be n^2 value and declared concrete type
    if( ((type & (type - 1)) != 0)  ||  !(type & (Projectile::TYPE_END - 1)) ) {
        // discard this def for invalid concrete type
        if(isNew) delete d;
        return false;
    }
    d->projectileConcreteType = type;
    d->tag = tag;

    // load values
    d->name         = c->getValueAsString(  tags, "weapon name", "unknown");
    d->clipSize     = c->getValueAsInt(     tags, "weapon clip size", 0);
    d->reloadTime   = c->getValueAsFloat(   tags, "weapon reload time", 1.0f);
    d->rof          = c->getValueAsFloat(   tags, "weapon rof", 1.0f);
    d->range        = c->getValueAsFloat(   tags, "weapon range", 10.0f);
    d->turnSpeed    = c->getValueAsFloat(   tags, "weapon turning speed", 1.0f);
    d->damage       = c->getValueAsInt(     tags, "weapon damage");
    d->damageRange  = c->getValueAsInt(     tags, "weapon explosion radius", 0);
    // store if new
    if(isNew)
        m_WeaponDefs.pushHead(d);
    return true;
}


/**
 * Load resourcer definitions
 */
bool DefManager::loadResourcerDef(int tag)
{
    Config* c = Config::getInstance();
    string tags = intToString(tag);
    // use old or create new
    ResourcerDef* d = getResourcerDef(tag);
    bool isNew = false;
    if(!d) {
        d = new ResourcerDef();
        ::memset(d, 0, sizeof(ResourcerDef));
        isNew = true;
    }
    // values
    int type = c->getValueAsInt(tags, "resourcer concrete type");
    // validate the concrete type - it needs to be n^2 value and declared concrete type
    if( ((type & (type - 1)) != 0)  ||  !(type & (Resourcer::TYPE_END - 1)) ) {
        // discard this def for invalid concrete type
        if(isNew) delete d;
        return false;
    }
    d->concreteType = type;
    d->tag = tag;
    d->oreCapacity = c->getValueAsInt(tags, "resourcer capacity", 0);
    d->loadTime = c->getValueAsFloat(tags, "resourcer load time", 0.0f);

    // store if new
    if(isNew)
        m_ResourcerDefs.pushHead(d);
    return true;
}


/**
 * Load radar definitions
 */
bool DefManager::loadRadarDef(int tag)
{
    Config* c = Config::getInstance();
    string tags = intToString(tag);
    RadarDef* d = getRadarDef(tag);
    bool isNew = false;
    if(!d) {
        d = new RadarDef();
        ::memset(d, 0, sizeof(RadarDef));
        isNew = true;
    }
    // values
    int type = c->getValueAsInt(tags, "radar concrete type");
    // validate the concrete type - it needs to be n^2 value and declared concrete type
    if( ((type & (type - 1)) != 0)  ||  !(type & (IAssetRadar::TYPE_END - 1)) ) {
        // discard this def for invalid concrete type
        if(isNew) delete d;
        return false;
    }
    d->tag = tag;
    d->concreteType = type;

    // load values
    d->range        = c->getValueAsFloat(   tags, "radar range", 10.0f);

    // store if new
    if(isNew)
        m_RadarDefs.pushHead(d);
    return true;
}


// ===== Has - methods


bool DefManager::hasAssetDef(int tag) {
    ListNode<AssetDef*>* node = m_AssetDefs.headNode();
    while(node)  {
        if(node->item->tag == tag) return true;
        node = node->next;
    } return false;
}

bool DefManager::hasMovingDef(int tag) {
    ListNode<MovingDef*>* node = m_MovingDefs.headNode();
    while(node)  {
        if(node->item->tag == tag) return true;
        node = node->next;
    } return false;
}

bool DefManager::hasWeaponDef(int tag) {
    ListNode<WeaponDef*>* node = m_WeaponDefs.headNode();
    while(node)  {
        if(node->item->tag == tag) return true;
        node = node->next;
    } return false;
}

bool DefManager::hasResourcerDef(int tag) {
    ListNode<ResourcerDef*>* node = m_ResourcerDefs.headNode();
    while(node)  {
        if(node->item->tag == tag) return true;
        node = node->next;
    } return false;
}

bool DefManager::hasRadarDef(int tag) {
    ListNode<RadarDef*>* node = m_RadarDefs.headNode();
    while(node)  {
        if(node->item->tag == tag) return true;
        node = node->next;
    } return false;
}


// ===== Get - methods


AssetDef* DefManager::getAssetDef(int tag) {
    ListNode<AssetDef*>* node = m_AssetDefs.headNode();
    while(node)  {
        if(node->item->tag == tag) return node->item;
        node = node->next;
    } return NULL;
}

WeaponDef* DefManager::getWeaponDef(int tag) {
    ListNode<WeaponDef*>* node = m_WeaponDefs.headNode();
    while(node)  {
        if(node->item->tag == tag) return node->item;
        node = node->next;
    } return NULL;
}

RadarDef* DefManager::getRadarDef(int tag) {
    ListNode<RadarDef*>* node = m_RadarDefs.headNode();
    while(node)  {
        if(node->item->tag == tag) return node->item;
        node = node->next;
    } return NULL;
}

MovingDef* DefManager::getMovingDef(int tag) {
    ListNode<MovingDef*>* node = m_MovingDefs.headNode();
    while(node)  {
        if(node->item->tag == tag) return node->item;
        node = node->next;
    } return NULL;
}

ResourcerDef* DefManager::getResourcerDef(int tag) {
    ListNode<ResourcerDef*>* node = m_ResourcerDefs.headNode();
    while(node)  {
        if(node->item->tag == tag) return node->item;
        node = node->next;
    } return NULL;
}
