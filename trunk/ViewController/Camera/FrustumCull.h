/**
 * FrustumCull
 * Static helpers for frustum culling
 * 
 * Original:  http://www.toymaker.info/Games/html/direct3d_faq.html#D3D5
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __FRUSTUMCULL_H__
#define __FRUSTUMCULL_H__

#include "d3dx9.h"

class FrustumCull
{
public:

    struct TFrustumPlane
    {
        /**
        * Plane normal
        */
        D3DXVECTOR3 m_Normal;

        /**
        * Plane distance
        */
        float m_Distance;

        /**
         * Normalises normals and distance
         */
        inline void normalise()
        {
            float denom = 1 / sqrt((m_Normal.x*m_Normal.x) + (m_Normal.y*m_Normal.y) + (m_Normal.z*m_Normal.z));
            m_Normal.x = m_Normal.x * denom;
            m_Normal.y = m_Normal.y * denom;
            m_Normal.z = m_Normal.z * denom;
            m_Distance = m_Distance * denom;
        }

        inline float distanceToPoint(D3DXVECTOR3& pnt)
        {
            return D3DXVec3Dot(&m_Normal, &pnt) + m_Distance;
        }
    };

    /**
     * Rebuilds frustum planes from view and projection matrix
     * @param pMatView   View matrix-pointer
     * @param pMatProj   Projection matrix-pointer
     */
    static void rebuildFrustum(D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj);

    /**
     * Culls an AABB against the view matrix
     * @param AABBMin "Minimum" corner of the AABB in world space
     * @param AABBMax "Maximum" corner of the AABB in world space
     * @return True, if the AABB intersects or is within view frustum, otherwise false
     */
    static bool cullAABB(const D3DXVECTOR3& AABBMin, const D3DXVECTOR3& AABBMax);


private:
    /**
     * Array of 6 planes forming the view frustum
     */
    static TFrustumPlane* frustumPlanes;

};

#endif //__FRUSTUMCULL_H__