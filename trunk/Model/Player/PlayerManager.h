/**
 * Keeps track of players
 *
 * $Revision: 161 $
 * $Date: 2009-03-15 19:18:00 +0200 (su, 15 maalis 2009) $
 * $Id: Player.h 161 2009-03-15 17:18:00Z murgo $
 */

#ifndef __PLAYERMANAGER_H__
#define __PLAYERMANAGER_H__

#include <vector>
#include "player.h"

using namespace std;

class PlayerManager
{
public:

    /**
     * Creates provided amount of players + 1 neutral player (index 0)
     * @param playerCount   number of players (2 for two-player game)
     */
    inline static void create(int playerCount)
    {
        PlayerManager* pInstance = getInstance();
        pInstance->m_PlayerCount = playerCount;
        for (int i = 0; i < playerCount + 1; i++)
        {
            Player* p = new Player(i);
            pInstance->m_arrPlayers.push_back(p);
        }
        
        // set everyone as enemy
        for (int i = 0; i < playerCount + 1; i++)
        {
            Player* p = pInstance->m_arrPlayers[i];
            for (int j = 0; j < playerCount + 1; j++)
            {
                if (j == i) continue;
                Player* p2 = pInstance->m_arrPlayers[j];
                p->setEnemies(p->getEnemies() | p2->getId());
            }
        }
    }

    /**
     * Destroys players
     */
    inline static void release()
    {
        PlayerManager* pInstance = getInstance();
        for (int i = 0; i < pInstance->m_PlayerCount + 1; i++)
            delete pInstance->m_arrPlayers[i];
        pInstance->m_arrPlayers.clear();
    }

    /**
     * Gets player
     * @param index     index of player (1 for player 1)
     * @return Player*  pointer to player
     */
    inline static Player* getPlayer(int index)
    {
        return getInstance()->m_arrPlayers[index];
    }

    /**
     * Returns array containing all players
     * @return array containing all players
     */
    inline static vector<Player*>& getPlayers() { return getInstance()->m_arrPlayers; }

    /**
     * Player count
     * @return how many players there are in game
     */
    inline static const int getPlayerCount() { return getInstance()->m_PlayerCount; }

private:
    /**
     * Gets the PlayerManager-instance
     * @return Pointer to PlayerManager
     */
    inline static PlayerManager* getInstance() 
    { 
        static PlayerManager instance;
        return &instance; 
    }

    int                 m_PlayerCount; // player count
    vector<Player*>     m_arrPlayers; // pointers to players
};

#endif
