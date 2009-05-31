/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "../common.h" // for NULL etc
#include "IAsset.h"
#include "IAssetListener.h"
#include "AssetCollection.h"

#include "../Defs/Defs.h"
#include "../Weapon/Damage.h"
#include "Armor.h"

int IAsset::m_InstanceCounter = 0;
int IAsset::m_InstanceDestructionCounter = 0;

IAsset::IAsset(const Type assetType, AssetDef& def) : m_ConcreteType (assetType), m_Def (def), m_IID (m_InstanceCounter)
{
    m_InstanceCounter++;
    // set to 0 coordinate
    m_Position.x = m_Position.y = m_Position.z = 0;
    m_OldPosX = m_OldPosY = 0;
    // set direction to upwards
    m_Direction.x = 0;
    m_Direction.y = 1;
    m_Direction.z = 0;
    //
    m_Hitpoints = 1;
    m_State = STATE_BEING_BUILT;
    // components
    m_pOwner = NULL;
    m_pWeapon = NULL;
    m_pRadar = NULL;
    m_pArmor = new Armor(this);

    m_HalfWidth = m_Def.width * 0.5f;
    m_HalfHeight = m_Def.height * 0.5f;
}

IAsset::~IAsset()
{
    m_InstanceDestructionCounter++;
}

// ===== Initialization

void IAsset::create()
{
    if(m_Created)
        return;
    m_Created = true;
    AssetCollection::registerAsset(this);
    //updatePositionInAssetCollection();
}

void IAsset::setWeapon(IWeapon* weapon)
{
    // if old logic exists, release it first
    if(m_pWeapon)
        releaseWeapon();
    m_pWeapon = weapon;
    // tell the logic that it is now in control of the moving of
    // this unit
    m_pWeapon->attach(this);
}

void IAsset::setRadar(IAssetRadar* radar)
{
    // if old radar exists, release it first
    if(m_pRadar)
        releaseRadar();
    m_pRadar = radar;
    m_pRadar->attach(this);
}

// ===== Releasing

void IAsset::release()
{
    notifyDestroyed();
    releaseWeapon();
    releaseRadar();
    if(m_pArmor) {
        delete m_pArmor;
        m_pArmor = NULL;
    }
}

void IAsset::releaseWeapon()
{
    if(m_pWeapon)
    {
        if(m_pWeapon->release())
            delete m_pWeapon;
        m_pWeapon = NULL;
    }
}

void IAsset::releaseRadar()
{
    if(m_pRadar)
    {
        if(m_pRadar->release())
            delete m_pRadar;
        m_pRadar = NULL;
    }
}

void IAsset::updatePositionInAssetCollection()
{
    if(!m_Created)
        return;
    // check if coordinate changed
    if (m_OldPosX != (unsigned short)getPosition()->x || m_OldPosY != (unsigned short)getPosition()->y)
    {
        AssetCollection::updatePosition(this, m_OldPosX, m_OldPosY);
        m_OldPosX = (unsigned short)getPosition()->x;
        m_OldPosY = (unsigned short)getPosition()->y;
    }
}

// =====

void IAsset::changeState(State newState)
{
    if(m_State != newState) {
        m_State = newState;
        notifyStateChanged();
    }
}

// ===== Listeners

void IAsset::registerListener(IAssetListener* listener)
{
    m_pListeners.pushHead(listener);
}

void IAsset::unregisterListener(IAssetListener* listener)
{
    m_pListeners.remove(listener);
}

void IAsset::notifyStateChanged()
{
    if(m_pListeners)
    {
        ListNode<IAssetListener*>* node = m_pListeners.headNode();
        while(node)
        {
            node->item->handleAssetStateChange(this, m_State);
            node = node->next;
        }
    }
}

void IAsset::notifyDestroyed()
{
    if(m_pListeners)
    {
        ListNode<IAssetListener*>* node = m_pListeners.headNode();
        while(node)
        {
            node->item->handleAssetReleased(this);
            node = node->next;
        }
        m_pListeners.release();
    }
}


// ===== Damage and hitpoints


const float IAsset::modifyHitpoints(const float amount)
{
    // if we are already dead, no hitpoint modification thank you
    if(m_State == STATE_DESTROYED)
        return 0;
    if(amount > 0  &&  (m_Hitpoints + amount) > (float)m_Def.maxHitpoints)
        m_Hitpoints = (float)m_Def.maxHitpoints;
    else
        m_Hitpoints += amount;
    // if our hitpoints reached zero, we are dead and enter destroyed state
    if(m_Hitpoints <= 0)
    {
        changeState(STATE_DESTROYED);
        m_Hitpoints = 0; // prevent the hitpoints to go below 0
    }
    // if we are in the state of being build and our hitpoints reach max,
    // we are ready and enter to active state
    else if(m_State == STATE_BEING_BUILT && m_Hitpoints == m_Def.maxHitpoints)
    {
        changeState(STATE_ACTIVE);
        //we enable minimap here if we just built a radar
        if(m_Def.tag == BUILDING_TYPE_RADAR)
            m_pOwner->toggleRadar(true);
        //to reduce update overload, this is the optimal place to recalculate energy consumption
        //another place is whenever building gets destroyed
        m_pOwner->CalculateEnergyBalance();
    }
    return m_Hitpoints;
}


float IAsset::handleDamage(Damage* pDamage)
{
    // if we are already dead, there's no point to handle damage
    if(m_State != STATE_DESTROYED)
    {
        // filter the damage with our armor
        if(m_pArmor)
        {
            m_pArmor->filterDamage(pDamage);
        }
        // reduce the hitpoints from our hitpoints
        modifyHitpoints(-pDamage->getTotalDamage());
    }
    delete pDamage;
    return m_Hitpoints;
}
