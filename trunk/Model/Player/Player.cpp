/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "Player.h"

Player::Player(const int index)
{
    m_Index = index;
    if (index == 0) m_Id = 0;
    else m_Id = 1 << (index - 1);

    m_Enemies = 0;
    m_Allies = 0;
    
    m_pFog = new Fog();
    m_pFog->setOwner(this);

    m_pAI = NULL;

    //HACK: make all players AI except first one
    if(index > 1)
    {
        m_pAI = new LameAI(this);
    }
}

Player::~Player()
{
    if (m_pFog)
    {
        delete m_pFog;
        m_pFog = NULL;
    }
    if (m_pAI)
    {
        delete m_pAI;
        m_pAI = NULL;
    }
}

void Player::UpdateAI(float fFrametime)
{
    if(m_pAI)
    {
        m_pAI->Update(fFrametime);
    }
}