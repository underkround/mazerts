#include "CombatAI.h"

//define how often combat AI thinks it's decisions
#define COMBAT_AI_UPDATE_INTERVAL 0.5f

CombatAI::CombatAI(Player* player)
{
    m_fUpdatetime = 0.0f;
    m_pPlayer = player;
}

CombatAI::~CombatAI(void)
{
}

CombatAI::Update(float fFrameTime)
{
    m_fUpdatetime += fFrameTime;
    if(m_fUpdatetime > COMBAT_AI_UPDATE_INTERVAL) {
        m_fUpdatetime -= COMBAT_AI_UPDATE_INTERVAL;
        //do your magic
    }
}