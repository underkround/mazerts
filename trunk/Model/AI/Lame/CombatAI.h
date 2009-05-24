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

    void Update(float fFrameTime);

    void Release(void);

    void EvasiveManeuver();

    Target* needDefending();
    Target* findAttackTarget(float* threat = 0);

    void assignReservesToGroup(UnitGroup* g);
    void releaseToReserve(UnitGroup* g);

    const bool readyToAttack();
    const bool assessFeasibility(const float threat);

    // there's probably already a method for this, but it was faster to create new than locate old one :P
    const float unitDistance(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2) const;

    void FindBaseCenterPoint(unsigned int *xCenter, unsigned int *yCenter);
    const float calculateThreat(IAsset* pAsset);
    
    void clearDefense();
    void clearAttacks();

    void handleCreatedAsset(IAsset* instance);
    void handleReleasedAsset(IAsset* instance);

private:
    Player*         m_pPlayer;
    float           m_fUpdatetime;

    bool            m_Defending;
    bool            m_Attacking;

    Target*         m_pAttackTarget;
    Target*         m_pDefenseTarget;

    UnitGroup*     m_pDefenseGroup;
    UnitGroup*     m_pAttackGroup;
    UnitGroup*     m_pReserveGroup;

    unsigned int    m_DefensePointX;
    unsigned int    m_DefensePointY;

    bool            m_PrintOutput;
    unsigned int    m_UpdatesSinceAttackBegan;
};
