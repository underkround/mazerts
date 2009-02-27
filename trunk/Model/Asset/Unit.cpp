/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "Unit.h"
#include "AssetCollection.h"

#include <iostream>

Unit::Unit(int unitType) : IAsset(UNIT)
{
    m_UnitType = unitType;
    std::cout << "unit (iid " << m_IID << ", type " << m_UnitType << ") constructor called\n";
}

Unit::~Unit()
{
    std::cout << "unit (iid " << m_IID << ", type " << m_UnitType << ") destructor called\n";
    release();
}

// =====

void Unit::create()
{
    AssetCollection::registerUnit(this);
}

void Unit::release()
{
}

// =====

char Unit::update(const float deltaT)
{
    std::cout << "unit (iid " << m_IID << ") update\n";
    return RESULT_DELETEME;
//    return RESULT_OK;
}
