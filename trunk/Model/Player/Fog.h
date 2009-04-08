/**
 * Fog of war.
 *
 * $Revision: 292 $
 * $Date: 2009-04-05 18:58:13 +0300 (su, 05 huhti 2009) $
 * $Id: Player.h 292 2009-04-05 15:58:13Z zem $
 */
#ifndef __FOG_H__
#define __FOG_H__

#include "../Common/DoubleLinkedList.h"

// forward declaration
class Player;
class IAsset;

class Fog
{
public:
    Fog();
    ~Fog();

    // this struct represents one row in fog range "bitmasks"
    struct FOGROW
    {
        int offset;
        int amount;
    };

    // this struct represents one fog range "bitmask"
    struct FOGMASK
    {
        int radius;
        DoubleLinkedList<FOGROW*> data;
    };

    /**
     * Doesn't do anything (yet?)
     */
    static void create();

    /**
     * Releases static fogmasks
     */
    static void release();

    /**
     * Owner of this fog
     */
    inline void setOwner(Player* player) { m_pOwner = player; }
    inline Player* getOwner() { return m_pOwner; }

    /**
     * Is a map coord visible?
     */
    inline const bool isVisibleCoord(const int x, const int y) { return m_Fog[y][x]; }

    /**
     * Set map coord visibility (probably not too much use, this one)
     */
	inline void setVisibleCoord(const int x, const int y, const bool val) { m_Fog[y][x] = val; }

    /**
     * @return Pointer to boolean array containing true if area is visible and false if area is in fog or NULL if fogging is disabled
     */
    inline bool** getFogArray() { return (m_Enabled) ? m_Fog : NULL; }

    /**
     * If enough time has passed, calculates new fog
     */
    void update(float deltaT);

    /**
     * This returns a FOGMASK-type mask for specified range (radius)
     * @param range / radius of the mask
     */
    static FOGMASK* getFogMask(int range);

    /**
     * Set the update interval (in seconds) between fog updates
     * @param interval Interval in seconds
     */
    inline void setUpdateInterval(const float interval) { m_UpdateInterval = interval; }

    /**
     * Returns the value of the changecounter, used by outside objects to
     * check if the countervalue has changed from what they have stored, so
     * they know to update themselves (on things related to fog)
     * @return Countervalue, wraps from 65535 to 0
     */
    inline const unsigned short getChangeCounter() const { return m_ChangeCounter; }

    inline bool isEnabled() { return m_Enabled; }
    inline void setEnabled(bool toggle) { m_Enabled = toggle; }

    /**
     * Is this asset visible or hidden in the fog
     * @param asset to check
     */
    const bool isAssetVisible(IAsset* pAsset);

private:
    static FOGMASK* calculateFogMask(const int radius);
    static void addValues(bool** tempArray, const int x, const int y, const int radius);

    bool**  m_Fog;

    Player*  m_pOwner;

    bool m_Enabled;

    /**
     * List containing fogmasks
     */
    static DoubleLinkedList<FOGMASK*> m_FogMasks;

    /**
     * Time in seconds between which the fog is updated
     */
    float m_UpdateInterval;

    /**
     * Update counter for interval measuring
     */
    float m_UpdateCounter;

    /**
     * Changecounter, used by outside classes to check if the fog has been changed and
     * to react on said changes
     */
    unsigned short m_ChangeCounter;
};

#endif // __PLAYER_H__
