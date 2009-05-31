/**
 * Ore-mine is a special case building
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __OREMINE_H__
#define __OREMINE_H__

#include "Building.h"

class OreMine : public Building
{
public:

    OreMine(AssetDef& def) : Building(def)
    {
        m_Created = false;
        m_Hitpoints = 100000.0f;
    }

    inline virtual void create()
    {
        Building::create();
        m_State = STATE_PARALYZED;
    }

    /**
     * Overridden update-method: OreMine cannot be built or destroyed
     * @param deltaT Time passed since last update
     */
    virtual inline char update(const float deltaT)
    {
        return RESULT_OK;
    }

};

#endif  //__OREMINE_H__