/**
 * UIProjectile
 *
 * 3D-representation of model-side Projectile 
 *
 * $Revision: 242 $
 * $Date: 2009-03-31 00:27:53 +0300 (ti, 31 maalis 2009) $
 * $Id: UI3DObjectManager.h 242 2009-03-30 21:27:53Z murgo $
 */

#ifndef __UIPROJECTILE_H__
#define __UIPROJECTILE_H__

#include "../App/C3DObject.h"
#include "../../Model/Weapon/Projectile.h"
#include "../Terrain/UITerrain.h"

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
        float t = m_pProjectile->getFlightT();
            
        m_mWorld._41 = m_Origin.x + m_Target.x * t;
        m_mWorld._42 = m_Origin.y + m_Target.y * t;
        m_mWorld._43 = m_Origin.z + m_Target.z * t - sin(t * D3DX_PI) * m_FlyHeight;
        
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