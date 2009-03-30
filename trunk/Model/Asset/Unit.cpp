/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "Unit.h"
#include "AssetCollection.h"

Unit::Unit(AssetDef& def) : IAsset(UNIT, def)
{
    //m_UnitType = unitType;
    // set defaults
    m_pMovingLogic = NULL;
    m_pWeapon = NULL;
    m_pRadar = NULL;
    m_Created = false;
    m_ParalyzeTimer = 0;
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
    switch(m_State)
    {

    /*
     * State while being build
     */
    case STATE_BEING_BUILT:
        // TODO: for now since there is no builder-stuff implemented,
        // we just enter to the active-state. Later it will happen
        // by growing the hitpoints until they reach max
        //modifyHitpoints((float)m_Def.maxHitpoints);

        // for debugging, let's advance our hitpoints so that we will be
        // around 5 seconds in building state (for the ui-state-implementation)
        {
            float hpMod = (float)m_Def.maxHitpoints;
            hpMod /= 5;
            modifyHitpoints(hpMod*deltaT);
        }
        break;

    case STATE_PARALYZED:
        // if we are paralyzed, we reduce the counter
        m_ParalyzeTimer -= deltaT;
        if(m_ParalyzeTimer <= 0)
            changeState(STATE_ACTIVE);
        break;

    /*
     * Active -state, normal state
     */
    default:
    case STATE_ACTIVE:
        // update our radar
        if(m_pRadar)
            m_pRadar->update(deltaT);

        // update our weapon
        if(m_pWeapon)
            m_pWeapon->update(deltaT);

        // update our moving
        if(m_pMovingLogic)
            m_pMovingLogic->update(this, deltaT);

        updatePositionInAssetCollection();
        break;

    /*
     * Destroyed -state
     */
    case STATE_DESTROYED:
        return RESULT_DELETEME; // unit will be deleted when returning this value
        break;

    } // switch

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
