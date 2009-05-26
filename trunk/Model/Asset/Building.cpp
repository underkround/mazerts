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

    Terrain* pTerrain = Terrain::getInstance();

    int averageHeight = 0;
    //Look for average height
    for(int i = 0; i < m_Def.height; i++)
    {
        for(int j = 0; j < m_Def.width; j++)
        {
            averageHeight += pTerrain->getTerrainHeight((unsigned short)(m_Position.x + j), (unsigned short)(m_Position.y + i));
        }
    }

    averageHeight /= (m_Def.height * m_Def.width);

    //Mark position as unpassable in map and flatten
    for(int i = 0; i < m_Def.height; i++)
    {
        for(int j = 0; j < m_Def.width; j++)
        {
            pTerrain->setPassability((unsigned short)(m_Position.x + j), (unsigned short)(m_Position.y + i), false);
            pTerrain->setTerrainHeight((unsigned short)(m_Position.x + j), (unsigned short)(m_Position.y + i), averageHeight);
        }
    }

    //Passable area within building (for mines, factories, resource yard...)
    if(m_Def.gridPassableAreaHeight > 0 && m_Def.gridPassableAreaWidth > 0)
    {
        for(int i = 0; i < m_Def.gridPassableAreaHeight; i++)
        {
            for(int j = 0; j < m_Def.gridPassableAreaWidth; j++)
            {
                Terrain::getInstance()->setPassability((unsigned short)(m_Position.x + m_Def.gridPassableAreaX + j), 
                                                       (unsigned short)(m_Position.y + m_Def.gridPassableAreaY + i), true);
            }        
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
            hpMod /= (float)m_Def.constructionTime;
            modifyHitpoints(hpMod*deltaT);
        }
        break;

    case STATE_PARALYZED:
        changeState(STATE_ACTIVE);
        break;

    case STATE_DISABLED:
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
        //when building is destroyed we need to recalculate our energy needs
        //another place to do so is whenever building becomes active
        m_pOwner->CalculateEnergyBalance();
        return RESULT_DELETEME; // unit will be deleted when returning this value
        break;

    } // switch

    return RESULT_OK; // normal return value
}

void Building::setPower(bool active)
{
    if(m_State == STATE_ACTIVE || m_State == STATE_DISABLED)
    {
        if(active)
            changeState(STATE_ACTIVE);
        else
            changeState(STATE_DISABLED);
    }
}