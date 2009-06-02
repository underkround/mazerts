/**
 * Money Plant is a building that creates ore resource for player
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#pragma once

//these values should probably go to .ini but didn't feel like touching that defs stuff
#define INCOME_INTERVAL 10.0f
#define INCOME_AMOUNT 25

#include "Building.h"

class MoneyPlant : public Building
{
public:

    /**
     * Overridden constructor
     * @param deltaT Time passed since last update
     */

    MoneyPlant(AssetDef& def) : Building(def)
    {
        m_Created = false;
        m_fIncomeTimer = 0.0f;
    }

    inline void create()
    {
        Building::create();
    }

    /**
     * Overridden update-method
     * @param deltaT Time passed since last update
     */
    char MoneyPlant::update(const float deltaT)
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
            changeState(IAsset::STATE_ACTIVE);
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

            m_fIncomeTimer += deltaT;

            if(m_fIncomeTimer > INCOME_INTERVAL)
            {
                m_fIncomeTimer -= INCOME_INTERVAL;
                m_pOwner->modifyOre(INCOME_AMOUNT);
            }

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

private:
    float m_fIncomeTimer;

};