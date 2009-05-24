/**
 * CombatAI
 *
 * Handles AI's attacking and defending in the update method
 *
 * $Revision: 326 $
 * $Date: 2009-04-15 22:32:11 +0300 (ke, 15 huhti 2009) $
 * $Id: Target.h 326 2009-04-15 19:32:11Z ezbe $
 */

#pragma once
#include "../IAI.h"
#include "../../Player/Player.h"
#include "../../Asset/IAssetCollectionListener.h"

class UnitGroup;
class Target;

class CombatAI : public IAI,  public IAssetCollectionListener
{
public:
    CombatAI(Player* player);
    virtual ~CombatAI(void);

    /**
     * This is where the magic happens
     * @param you know the drill
     */
    void Update(float fFrameTime);

private:
    /**
     * Checks if defense is needed
     * @return Target which one to attack, null if no defending is needed
     */
    Target* needDefending();

    /**
     * Finds best attack target
     * @param pointer to float in which to store threat value
     * @return best target or NULL
     */
    Target* findAttackTarget(float* threat = 0);

    /**
     * Assigns units from m_ReserveGroup to g
     * @param g guess
     */
    void assignReservesToGroup(UnitGroup* g);

    /**
     * Assigns units from g to m_ReserveGroup
     * Also reissues target to reservegroup
     * @param g guess
     */
    void releaseToReserve(UnitGroup* g);

    /**
     * @return true if we're ready to attack
     */
    const bool readyToAttack();

    /**
     * @return true if attacking is feasible (doable, affordable, not-a-suicide)
     */
    const bool assessFeasibility(const float threat);

    /**
     * Calculates euclidian distance between points.
     * There's probably already a method for this, but it was faster to create new than locate old one :P
     * @param blah blah
     */
    const float unitDistance(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2) const;

    /**
     * Sets own base center point to provided pointers. This is stolen from LameAI, and it might or might not suck
     */
    void FindBaseCenterPoint(unsigned int *xCenter, unsigned int *yCenter);

    /**
     * Calculates threat to provided asset using its' and surrounding assets' "threat" -defs.
     */
    const float calculateThreat(IAsset* pAsset);
    
    /**
     * clear defense/offense group
     */
    void clearDefense();
    void clearAttacks();

    /**
     * Adds new units into reserve
     */
    void handleCreatedAsset(IAsset* instance);

    /**
     * Does nothing
     */
    void handleReleasedAsset(IAsset* instance);

    /**
     * Loads related configs.
     */
    void LoadConfigFromFile(void);

    Player*         m_pPlayer; // owner
    float           m_fUpdatetime; // duh

    bool            m_Defending; // are we defending
    bool            m_Attacking; // are we attacking

    Target*         m_pAttackTarget; // attacking target
    Target*         m_pDefenseTarget; // defending target

    UnitGroup*      m_pDefenseGroup; // storage of defenders
    UnitGroup*      m_pAttackGroup; // storage of attackers
    UnitGroup*      m_pReserveGroup; // storage of rest

    unsigned int    m_DefensePointX; // base center
    unsigned int    m_DefensePointY; // base center
    unsigned int    m_RallyPointX; // unit rally point
    unsigned int    m_RallyPointY; // unit rally point

    bool            m_PrintOutput; // whether to print output or not
    int             m_UpdatesSinceAttackBegan; // internal tmp
    int             m_TargetFlags; // not really used

    // settings, see ai.ini if you'll need explanations
    float           m_CombatUpdateInterval;
    float           m_DefenseDistanceMin;
    float           m_DefenseDistanceMax;
    int             m_ThreatRange;
    float           m_ThreatThreshold;
    int             m_MinUnitsInReserve;
    int             m_MinAttackGroupSize;
    int             m_AttackTimeout;
    int             m_RallyPointDistance;

};
