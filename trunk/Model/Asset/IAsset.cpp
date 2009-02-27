/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "IAsset.h"

#include <iostream>

int IAsset::m_InstanceCounter = 0;
int IAsset::m_InstanceDestructionCounter = 0;

IAsset::IAsset(const Type assetType) : m_AssetType (assetType), m_IID (m_InstanceCounter)
{
    m_InstanceCounter++;
    // set to 0 coordinate
    m_Position.x = m_Position.y = m_Position.z = 0;
    // set direction to upwards
    m_Direction.x = 0;
    m_Direction.z = 0;
    m_Direction.y = 1;
    //
    m_Weapon = NULL;
}

IAsset::~IAsset()
{
    m_InstanceDestructionCounter++;
}


/*
const bool IAsset::hasWeapon() const {
    return (m_Weapon) ? true : false;
}
const IWeapon* getWeapon() {
    return m_Weapon;
}
*/