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

class Player
{
public:

    Player(const int index);
    ~Player();

    /**
     * TODO:
     * - fog
     * - territory
     * - resources: energy
     *   - ore is kept in mining units and factory buildings?
     */

    inline const int getId() { return m_Id; }
    inline const int getIndex() { return m_Index; }

    inline void setEnemies(const int enemies) { m_Enemies = enemies; }
    inline const int getEnemies() { return m_Enemies; }

    inline void setAllies(const int allies) { m_Allies = allies; }
    inline const int getAllies() { return m_Allies; }

private:
    int         m_Id; // id of this player
    int         m_Index; // index of this player

    // these two are probably not needed, but are here anyways
    int         m_Enemies;
    int         m_Allies;
};

#endif // __PLAYER_H__
