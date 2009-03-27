/**
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
#include "../Weapon/Projectile.h"  // for Type-enum validation


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
        ListNode<BuilderDef*>* node = m_BuilderDefs.headNode();
        while(node) {
            delete node->item;
            node = m_BuilderDefs.removeGetNext(node);
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
    c->setFilename("../data/defs/builders.ini");
    c->readFile();
    c->setFilename("../data/defs/resourcers.ini");
    c->readFile();
    c->setFilename("../data/defs/radars.ini");
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
    d->constructionCostEnergy = c->getValueAsInt(   tags, "asset constrcution cost energy", 0);
    d->constructionCostOre  = c->getValueAsInt(     tags, "asset constrcution cost ore", 10);

    d->gridExitPointX       = c->getValueAsInt(     tags, "asset grid exit point x", 0);
    d->gridExitPointY       = c->getValueAsInt(     tags, "asset grid exit point y", 0);
    d->idleEnergyConsumption = c->getValueAsInt(    tags, "asset idle energy consumption", 1);
    d->name                 = c->getValueAsString(  tags, "asset name", "unnamed");
    d->description          = c->getValueAsString(  tags, "asset description", "unset description");
    d->maxHitpoints         = c->getValueAsInt(     tags, "asset max hitpoints", 10);
    d->width                = c->getValueAsInt(     tags, "asset width", 2);
    d->height               = c->getValueAsInt(     tags, "asset height", 2);

    // load component definitions for asset
    int t;

    t = c->getValueAsInt(tags, "asset radar tag");
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

    t = c->getValueAsInt(tags, "asset builder tag");
    if(t && loadBuilderDef(t))
        d->pDefBuilder = getBuilderDef(t);
    else
        d->pDefBuilder = NULL;

    t = c->getValueAsInt(tags, "asset resourer tag");
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
    // store if new
    if(isNew)
        m_WeaponDefs.pushHead(d);
    return true;
}


/**
 * Load builder definitions
 */
bool DefManager::loadBuilderDef(int tag)
{
    return false; // TODO: IMPLEMENT BUILDER COMPONENT!

    Config* c = Config::getInstance();
    string tags = intToString(tag);
    BuilderDef* d = getBuilderDef(tag);
    bool isNew = false;
    if(!d) {
        d = new BuilderDef();
        ::memset(d, 0, sizeof(BuilderDef));
        isNew = true;
    }
    // values
    int type = c->getValueAsInt(tags, "builder concrete type");
    // validate the concrete type - it needs to be n^2 value and declared concrete type
/*
    if( ((type & (type - 1)) != 0)  ||  !(type & (IBuilder::TYPE_END - 1)) ) {
        // discard this def for invalid concrete type
        if(isNew) delete d;
        return false;
    }
*/
    d->tag = tag;

    // load values
    // TODO

    // store if new
    if(isNew)
        m_BuilderDefs.pushHead(d);
    return true;
}


/**
 * Load resourcer definitions
 */
bool DefManager::loadResourcerDef(int tag)
{
    return false; // TODO: IMPLEMENT RESOURCER COMPONENT!

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
    /*
    if( ((type & (type - 1)) != 0)  ||  !(type & (IResourcer::TYPE_END - 1)) ) {
        // discard this def for invalid concrete type
        if(isNew) delete d;
        return false;
    }*/
    d->tag = tag;

    // load values
    // TODO

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
    // TODO

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

bool DefManager::hasBuilderDef(int tag) {
    ListNode<BuilderDef*>* node = m_BuilderDefs.headNode();
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

BuilderDef* DefManager::getBuilderDef(int tag) {
    ListNode<BuilderDef*>* node = m_BuilderDefs.headNode();
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