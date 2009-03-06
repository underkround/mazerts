/**
 * TerrainIntersection
 *
 * Handles intersection-checks with terrain
 * friend of UITerrain
 *
 * $Revision: 89 $
 * $Date: 2009-03-05 05:41:56 +0200 (to, 05 maalis 2009) $
 * $Id: UITerrain.h 89 2009-03-05 03:41:56Z ezbe $
 */

#ifndef __TERRAININTERSECTION_H__
#define __TERRAININTERSECTION_H__

#include "d3dx9.h"
#include "../../Model/Common/DoubleLinkedList.h"

class TerrainIntersection
{
public:

    /**
     * Picks the terrain with given ray and returns the coordinate of collision point (if any)
     * @param rayOrigin  Origin of the ray
     * @param rayDir     Direction of the ray
     * @Return Collision point as D3DXVECTOR or NULL if no collision
     */
    static D3DXVECTOR3* pickTerrain(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir);

private:

    /**
     * X and Y -indices, used to store lists of terrain patches
     */
    struct INDICES
    {
        INDICES(unsigned short xind, unsigned short yind)
        {
            x = xind;
            y = yind;
        }
        unsigned short x;
        unsigned short y;
    };

    /**
     * Cuts the ray between two planes and returns the plane-intersection points
     * @param rayOrigin  Origin of the ray
     * @param rayDir     Direction of the ray     
     * @return Two points as D3DXVECTOR3s
     */
    static D3DXVECTOR3* getPointsFromPlaneClippedRay(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir);

    /** 
     * Finds terrain patches in xy-plane between points
     * @return INDICES-list with terrain patch indices
     */
    static DoubleLinkedList<INDICES*>* getPatchesBetween(int startX, int startY, int endX, int endY, int maxSize);

    /**
     * Culls patches based on their AABBs and ray-origin and -direction
     * @param patches    INDICES-list of patch indices to check
     * @param rayOrigin  Origin of the ray
     * @param rayDir     Direction of the ray
     */
    static DoubleLinkedList<INDICES*>* getAABBCulledPatches(DoubleLinkedList<INDICES*>* patches, D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir);

    /**
     * Get the final collision point (if any)
     * @param patches    INDICES-list of patch indices to check
     * @param rayOrigin  Origin of the ray
     * @param rayDir     Direction of the ray
     * @return D3DXVECTOR or NULL if no collision occurred
     */
    static D3DXVECTOR3* getCollisionPoint(DoubleLinkedList<INDICES*>* patches, D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir);
};


#endif //__TERRAININTERSECTION_H__