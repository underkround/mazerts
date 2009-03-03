/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "Unit.h"
#include "AssetCollection.h"

Unit::Unit(int unitType) : IAsset(UNIT)
{
    m_UnitType = unitType;
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
    m_Created = true;
}

// ===== RELEASE

void Unit::release()
{
    IAsset::release(); // release weapon & radar
    releaseMovingLogic();
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
        m_pRadar->update(this, deltaT);

    // update our weapon
    if(m_pWeapon)
        m_pWeapon->update(this, deltaT);

    // update our moving
    if(m_pMovingLogic)
        m_pMovingLogic->update(this, deltaT);

    //return RESULT_DELETEME; // unit will be deleted when returning this value
    return RESULT_OK; // normal return value
}
