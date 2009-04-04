/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "Building.h"

#include "../Terrain/Terrain.h"

Building::Building(AssetDef& def) : IAsset(BUILDING, def)
{
    m_Created = false;
}

Building::~Building()
{
    if(m_Created)
        release();
}

// =====

void Building::create()
{
    if(m_Created)
        return;
    // IAsset's create registers & transfers the ownership to the collection
    IAsset::create();

    //Mark position as unpassable in map
    //TODO: For factories, it is necessary to leave passable area where the units come out
    for(int i = 0; i < m_Def.height; i++)
    {
        for(int j = 0; j < m_Def.width; j++)
        {
            Terrain::getInstance()->setPassability((unsigned short)(m_Position.x + j), (unsigned short)(m_Position.y + i), false);
        }
    }    

    m_Created = true;
}

void Building::release()
{
    IAsset::release();

    //Set ground again as passable
    for(int i = 0; i < m_Def.height; i++)
    {
        for(int j = 0; j < m_Def.width; j++)
        {
            Terrain::getInstance()->setPassability((unsigned short)(m_Position.x + j), (unsigned short)(m_Position.y + i), true);
        }
    }

    m_Created = false;
}

// =====

char Building::update(const float deltaT)
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
        {
            float hpMod = (float)m_Def.maxHitpoints;
            hpMod /= 20;
            modifyHitpoints(hpMod*deltaT);
        }
        break;

    case STATE_PARALYZED:
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
