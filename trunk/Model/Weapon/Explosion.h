/**
 * Explosion
 *
 * Explosions are caused by projectiles, they are used to pass damage data to relevant units
 * and to show explosions graphically in view-side
 *
 * $Revision$
 * $Date$
 * $Id$
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

        DoubleLinkedList<IAsset*> list;
        if(AssetCollection::getAssetsAt(&list, m_PositionX - damageRange, m_PositionY - damageRange, m_Radius, m_Radius) != 0)
        {
            ListNode<IAsset*>* pNode = list.headNode();
            while(pNode)
            {
                IAsset* pAsset = pNode->item;
                float dist = (float)(abs(m_PositionX - pAsset->getCenterGridX()) + abs(m_PositionY - pAsset->getCenterGridY()) + 1);
			    float currentDamage = m_Damage / dist;

                Damage* pDamage = new Damage(Damage::EXPLOSIVE, currentDamage);

                pAsset->handleDamage(pDamage);

                pNode = pNode->next;
            }
        }
        
        if(m_Damage > 1000)
        {
            pTerrain->createCrater(m_PositionX, m_PositionY, damageRange); 
        }
    }

    //Getters for position and radius
    inline const unsigned short getX() const { return m_PositionX; }
    inline const unsigned short getY() const { return m_PositionY; }
    inline const unsigned short getRadius() const { return m_Radius; }

    //Getter for damage
    inline const float getDamage() const { return m_Damage; }

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
