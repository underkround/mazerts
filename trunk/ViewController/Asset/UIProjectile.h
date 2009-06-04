/**
 * UIProjectile
 *
 * 3D-representation of model-side Projectile 
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __UIPROJECTILE_H__
#define __UIPROJECTILE_H__

#include "../App/C3DObject.h"
#include "../../Model/Weapon/Projectile.h"
#include "../Terrain/UITerrain.h"
#include "../Sound/SoundManager.h"

class UIProjectile : public C3DObject
{
public:

    UIProjectile(Projectile* pProjectile) : m_pProjectile(pProjectile)
    { 
        m_Origin.x = (float)pProjectile->getOriginX();
        m_Origin.y = (float)pProjectile->getOriginY();
        //TODO: Maybe needs smarter way to get z-coordinate (& other coordinates?)... maybe not
        m_Origin.z = UITerrain::getInstance()->calculateTriangleHeightAt(m_Origin.x, m_Origin.y) - 2.0f;

        m_Target.x = (float)pProjectile->getTargetX() - m_Origin.x;
        m_Target.y = (float)pProjectile->getTargetY() - m_Origin.y;
        m_Target.z = UITerrain::getInstance()->calculateTriangleHeightAt((float)pProjectile->getTargetX(), (float)pProjectile->getTargetY()) - m_Origin.z;

        m_FlyHeight = pProjectile->getFlyHeight();
    }

    virtual ~UIProjectile()
    {        
    }

    /**
     * Updates the object
     * @param fFrametime Time passed since last call in seconds
     */
    inline bool Update(float fFrametime)
    {
        Fog* pFog = UITerrain::getInstance()->getCurrentPlayer()->getFog();        
        SetVisible(!pFog->isEnabled() || pFog->isVisibleCoord((int)m_mWorld._41, (int)m_mWorld._42));

        if(IsVisible())
        {
            float t = m_pProjectile->getFlightT();

            m_mWorld._41 = m_Origin.x + m_Target.x * t;
            m_mWorld._42 = m_Origin.y + m_Target.y * t;
            m_mWorld._43 = m_Origin.z + m_Target.z * t - sin(t * D3DX_PI) * m_FlyHeight;

            //Only rotate other than shells
            if(m_pProjectile->getConcreteType() != Projectile::SHELL)
            {
                float nt = t + 0.01f;
                float nx = m_Origin.x + m_Target.x * nt;
                float ny = m_Origin.y + m_Target.y * nt;
                float nz = m_Origin.z + m_Target.z * nt - sin(nt * D3DX_PI) * m_FlyHeight;
                D3DXVECTOR3 forward(nx - m_mWorld._41, ny - m_mWorld._42, nz - m_mWorld._43);
                D3DXVec3Normalize(&forward, &forward);
                D3DXVECTOR3 up(forward.z, forward.x, -forward.y);
                D3DXVECTOR3 right;
                D3DXVec3Cross(&right, &up, &forward);
                D3DXVec3Cross(&up, &right, &forward);
                (*(D3DXVECTOR3*)&m_mWorld._11) = right;
                (*(D3DXVECTOR3*)&m_mWorld._21) = forward;
                (*(D3DXVECTOR3*)&m_mWorld._31) = up;
            }

            if(!m_pProjectile->isAlive())
            {
                //Play sound
                if (m_pProjectile->getConcreteType() == Projectile::ICBM)
                    SoundManager::playSound(SOUND_NUKE, 0.01f, *((D3DXVECTOR3*)&m_mWorld._41), true);
                else
                    SoundManager::playSound(SOUND_EXPLOSION, 0.01f, *((D3DXVECTOR3*)&m_mWorld._41), true);
            }
        }
        return m_pProjectile->isAlive();
    }
    
private:

    /**
     * Target vector from start to target
     */
    D3DXVECTOR3 m_Target;

    /**
     * Start coordinate
     */
    D3DXVECTOR3 m_Origin;

    /**
     * Fly-arc highest point
     */
    float m_FlyHeight;

    /**
     * Pointer to model-side projectile this object represents
     */
    const Projectile* m_pProjectile;

};

#endif //__UIPROJECTILE_H__