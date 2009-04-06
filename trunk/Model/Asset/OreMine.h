/**
 * Ore-mine is a special case building
 *
 * $Revision: 281 $
 * $Date: 2009-04-03 20:16:06 +0300 (pe, 03 huhti 2009) $
 * $Id: Unit.h 281 2009-04-03 17:16:06Z zem $
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