#pragma once
#include "../IAI.h"
#include "../../Player/Player.h"

class CombatAI : public IAI
{
public:
    CombatAI(Player* player);
    virtual ~CombatAI(void);

    void Update(float fFrameTime);

    void Release(void);

    void EvasiveManeuver();

private:
    Player*     m_pPlayer;
    float       m_fUpdatetime;
};
