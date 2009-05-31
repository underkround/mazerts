/**
 * Keeps track of players
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __PLAYERMANAGER_H__
#define __PLAYERMANAGER_H__

#include <vector>
#include "player.h"

using namespace std;

class PlayerManager
{
public:

    PlayerManager() {
        m_Created = false;
    }

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
        pInstance->m_Created = true;
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
        pInstance->m_Created = false;
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

    /**
     * Run AI
     */
    inline static void update(float fFrametime) {
        PlayerManager* pInstance = getInstance();
        for (int i = 0; i < pInstance->m_PlayerCount + 1; ++i)
        {
            Player* p = pInstance->m_arrPlayers[i];
            p->UpdateAI(fFrametime);
        }
    }

    /**
     * Gets the PlayerManager-instance
     * @return Pointer to PlayerManager
     */
    inline static PlayerManager* getInstance() 
    { 
        static PlayerManager instance;
        return &instance; 
    }

    inline static const bool isCreated()  { return getInstance()->m_Created; }

private:

    int                 m_PlayerCount; // player count
    vector<Player*>     m_arrPlayers; // pointers to players
    bool                m_Created;

};

#endif
