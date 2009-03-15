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

int IAsset::m_InstanceCounter = 0;
int IAsset::m_InstanceDestructionCounter = 0;

IAsset::IAsset(const Type assetType) : m_AssetType (assetType), m_IID (m_InstanceCounter)
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
    m_State = BEING_BUILT;
    // components
    m_pOwner = NULL;
    m_pWeapon = NULL;
    m_pRadar = NULL;
}

IAsset::~IAsset()
{
    m_InstanceDestructionCounter++;
}

// ===== Initialization

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
}

void IAsset::releaseWeapon()
{
    if(m_pWeapon)
    {
        if(m_pWeapon->release(this))
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
    m_State = newState;
    notifyStateChanged();
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
