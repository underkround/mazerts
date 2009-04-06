/**
 * Fog of war.
 *
 * $Revision: 292 $
 * $Date: 2009-04-05 18:58:13 +0300 (su, 05 huhti 2009) $
 * $Id: Player.h 292 2009-04-05 15:58:13Z zem $
 */
#ifndef __FOG_H__
#define __FOG_H__

class Fog
{
public:
	inline const bool getFogAt(const int x, const int y) { return false; }
	inline void setFogAt(const int x, const int y, const bool val) { }

private:
};

#endif // __PLAYER_H__
