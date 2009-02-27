/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "Building.h"

#include <iostream>

Building::Building(int buildingType) : IAsset(BUILDING)
{
    m_BuildingType = buildingType;
    std::cout << "building (iid " << m_IID << ", type " << m_BuildingType << ") constructor called\n";
}

Building::~Building()
{
    std::cout << "building (iid " << m_IID << ", type " << m_BuildingType << ") destructor called\n";
    release();
}

// =====

void Building::create()
{
    AssetCollection::registerBuilding(this);
}

void Building::release()
{
}

// =====

char Building::update(const float deltaT)
{
    std::cout << "building (iid " << m_IID << ") update\n";
    return RESULT_OK;
}
