/**
 * Player is the owner of assets, owner of buildable land (territory),
 * subject of fog, keeper of the resources etc etc
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Fog.h"
//AI
#include "../AI/Lame/LameAI.h"
#include "../AI/Lame/CombatAI.h"
#include "../Defs/Defs.h"
//circular dependencies here
class LameAI;
class CombatAI;

class Player
{
public:
    Player(const int index);
    ~Player();

    /**
     * TODO:
     * - territory (for new buildings?)
     */

    inline const int getId() { return m_Id; }
    inline const int getIndex() { return m_Index; }

    inline void setEnemies(const int enemies) { m_Enemies = enemies; }
    inline const int getEnemies() { return m_Enemies; }

    inline void setAllies(const int allies) { m_Allies = allies; }
    inline const int getAllies() { return m_Allies; }

    bool hasAsset(int reqTag) const;
    bool hasAsset(AssetDef* reqDef) const;

    /**
     * Test if given player is our enemy
     * @return true if enemy
     */
    inline const bool isEnemy(Player* p)
    {
        if(!p) return false; // null is not an enemy <3
		return (m_Enemies & p->getId()) ? true : false;
    }

	inline const bool isFriend(Player* p)
	{
        if(!p) return false; // null is not a friend D:
		return !(m_Enemies & p->getId()) ? true : false;
	}

    inline const bool hasRadar() { return m_bHasRadar; }
    inline void toggleRadar(bool setting) { m_bHasRadar = setting; }

	inline const int getOre() { return m_Ore; }
	inline void setOre(const int amount) { m_Ore = amount; }
    inline void modifyOre(const int amount) { m_Ore += amount; }

    //only for debugging use!
    inline void setEnergyProduced(const int amount) { m_EnergyProduced = amount; }
    //only for debugging use!
	inline void setEnergyConsumed(const int amount) { m_EnergyConsumed = amount; }

    inline const int getEnergyProduced() { return m_EnergyProduced; }
	inline const int getEnergyConsumed() { return m_EnergyConsumed; }

    //these predicted energy values take into account buildings that are still being built
    const int getPredictedEnergyProduction();
    const int getPredictedEnergyConsumption();

    /**
     * Calculates if we are making more energy than we are needing
     */
    bool CalculateEnergyBalance();
    /**
     * enables/disables buildings
     *
     * @param enable false = disables all buildings that consume energy
     * @param enable true  = enables all buildings
     */
    void PowerSwitch(bool enable);

	inline Fog* getFog() { return m_pFog; }

    inline LameAI* getLameAI() { return m_pAI; }
    inline CombatAI* getCombatAI() { return m_pUnitAI; }

    void UpdateAI(float fFrametime);

private:

    int         m_Id; // id of this player
    int         m_Index; // index of this player

    // these two are probably not needed, but are here anyways
    int         m_Enemies;
    int         m_Allies;

	int			m_EnergyProduced; // amount of energy produced
	int			m_EnergyConsumed; // amount of energy consumed

	int			m_Ore; // amount of "money"

    bool        m_bHasRadar;

    Fog*		m_pFog; // FOG OF WAR!! This is a pointer because of initialization order.

    //AI
    LameAI*    m_pAI;
    CombatAI*  m_pUnitAI;
};

#endif // __PLAYER_H__
