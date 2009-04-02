/**
 * Explosion
 *
 * Explosions are caused by projectiles, they are used to pass damage data to relevant units
 * and to show explosions graphically in view-side
 *
 * $Revision: 70 $
 * $Date: 2009-02-27 23:40:55 +0200 (pe, 27 helmi 2009) $
 * $Id: Damage.h 70 2009-02-27 21:40:55Z zem $
 */

#ifndef __EXPLOSION_H__
#define __EXPLOSION_H__

#include "Damage.h"
#include "../Terrain/Terrain.h"
#include "../Asset/IAsset.h"
#include "../Asset/AssetCollection.h"

class Explosion
{
public:
    Explosion(unsigned short x, unsigned short y, unsigned short radius, float damage)
    {
        m_PositionX = x;
        m_PositionY = y;
        m_Radius = radius;
        m_Damage = damage;
    }

    virtual ~Explosion()
    {
    }

    inline void update()
    {
        Terrain* pTerrain = Terrain::getInstance();
        int damageRange = m_Radius >> 1;
	    for(int x = -(damageRange); x <= damageRange; ++x)
	    {
		    for(int y = -(damageRange); y <= damageRange; ++y)
		    {
                int xPos = (x + m_PositionX);
                int yPos = (y + m_PositionY);
			    if( xPos >= 0 && xPos < pTerrain->getSize() && yPos >= 0 && yPos < pTerrain->getSize())
			    {
                    IAsset* pAsset = AssetCollection::getAssetAt(xPos, yPos);
                    if(pAsset != NULL)
				    {
					    float dist = (float)(abs(x) + abs(y) + 1);
					    float currentDamage = m_Damage / (float)dist;
    					
                        Damage* pDamage = new Damage(Damage::EXPLOSIVE, currentDamage);

                        pAsset->handleDamage(pDamage);
    					
				    }
			    }
		    }
	    }
    }
    //Getters for position and radius
    inline const unsigned short getX() const { return m_PositionX; }
    inline const unsigned short getY() const { return m_PositionY; }
    inline const unsigned short getRadius() const { return m_Radius; }

private:

    unsigned short m_PositionX;
    unsigned short m_PositionY;

    unsigned short m_Radius;

    /**
     * Damage caused on center square (diminishes farther away)
     */
    float m_Damage;
};

#endif //__EXPLOSION_H__
