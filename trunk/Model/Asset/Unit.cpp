/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "Unit.h"
#include "AssetCollection.h"

Unit::Unit(AssetDef def) : IAsset(UNIT, def)
{
    //m_UnitType = unitType;
    // set defaults
    m_pMovingLogic = NULL;
    m_pWeapon = NULL;
    m_pRadar = NULL;
    m_Created = false;
}

Unit::~Unit()
{
    release();
}

// ===== INITIALIZE

void Unit::create()
{
    // if already registered, return
    if(m_Created)
        return;
    // transfer the ownership to the collection
    AssetCollection::registerUnit(this);
    // set "old" position (current)
    m_OldPosX = (unsigned short)getPosition()->x;
    m_OldPosY = (unsigned short)getPosition()->y;

    m_Created = true;
}

// ===== RELEASE

void Unit::release()
{
    releaseMovingLogic();
    IAsset::release(); // release weapon & radar
}

void Unit::releaseMovingLogic()
{
    if(m_pMovingLogic) // if movingLogic attached, release it
    {
        // if moving logic is instance-specifig, delete it
        if(m_pMovingLogic->release(this))
            delete m_pMovingLogic;
        m_pMovingLogic = NULL;
    }
}

// =====

char Unit::update(const float deltaT)
{
    // update our radar
    if(m_pRadar)
        m_pRadar->update(deltaT);

    // update our weapon
    if(m_pWeapon)
        m_pWeapon->update(this, deltaT);

    // update our moving
    if(m_pMovingLogic)
        m_pMovingLogic->update(this, deltaT);

    updatePositionInAssetCollection();

    //return RESULT_DELETEME; // unit will be deleted when returning this value
    return RESULT_OK; // normal return value
}

// ===== Targets

/*
bool Unit::hasTarget()
{
    return (!m_TargetQueue.empty());
}

void Unit::addTarget(Target* target)
{
    m_TargetQueue.pushTail(target);
}

void Unit::setTarget(Target* target)
{
    clearAllTargets();
    m_TargetQueue.pushTail(target);
}

Target* Unit::getCurrentTarget()
{
    if(m_TargetQueue.empty())
        return NULL;
    return m_TargetQueue.headNode()->item;
}

void Unit::clearCurrentTarget()
{
    if(!m_TargetQueue.empty())
    {
        delete m_TargetQueue.headNode()->item;
        m_TargetQueue.remove(m_TargetQueue.headNode());
    }
}

void Unit::clearAllTargets()
{
    ListNode<Target*>* node = m_TargetQueue.headNode();
    while(node)
    {
        delete node->item;
        node = m_TargetQueue.removeGetNext();
    }
}

DoubleLinkedList<Target*> Unit::getTargetQueue()
{
    return &m_TargetQueue;
}
*/