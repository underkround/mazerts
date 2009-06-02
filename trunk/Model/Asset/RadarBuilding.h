/**
 * Radar building enables/disables player's minimap
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#pragma once


#include "Building.h"

class RadarBuilding : public Building
{
public:
    RadarBuilding(AssetDef& def) : Building(def)
    {
        m_Created = false;
    }

    inline void create()
    {
        Building::create();
    }

    inline void RadarBuilding::setPower(bool active)
    {
        if(m_State == STATE_ACTIVE || m_State == STATE_DISABLED)
        {
            if(active)
            {
                m_pOwner->toggleRadar(active);
                changeState(IAsset::STATE_ACTIVE);
            }
            else
            {
                m_pOwner->toggleRadar(active);
                changeState(IAsset::STATE_DISABLED);
            }
        }
    }

};