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
}

Player::~Player()
{
}
