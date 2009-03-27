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

class Explosion
{
public:
    Explosion(unsigned short x, unsigned short y, unsigned short radius)
    {
        m_PositionX = x;
        m_PositionY = y;
        m_Radius = radius;
    }

    virtual ~Explosion()
    {
    }

    inline void update()
    {

    }

    //Getters for position and radius
    inline const unsigned short getX() const { return m_PositionX; }
    inline const unsigned short getY() const { return m_PositionY; }
    inline const unsigned short getRadius() const { return m_Radius; }

private:

    unsigned short m_PositionX;
    unsigned short m_PositionY;

    unsigned short m_Radius;
};

#endif //__EXPLOSION_H__
