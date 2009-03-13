/**
 * TerrainIntersection
 *
 * Handles intersection-checks with terrain
 * friend of UITerrain
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __TERRAININTERSECTION_H__
#define __TERRAININTERSECTION_H__

#include "d3dx9.h"
#include "../../Model/Common/DoubleLinkedList.h"

class TerrainIntersection
{
public:

    /**
     * X and Y -indices for 2-dimensional arrays, used to store lists of terrain square indices
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
     * Picks the terrain with given ray and returns the coordinate of collision point (if any)
     * @param rayOrigin  Origin of the ray (world-space)
     * @param rayDir     Direction of the ray (world-space)
     * @Return Collision point as D3DXVECTOR or NULL if no collision
     */
    static D3DXVECTOR3* pickTerrain(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir);

    /** 
     * Finds terrain squares in xy-plane between points (can be used as general grid traversal algorithm)
     * @param startX X-coordinate of start point
     * @param startY X-coordinate of start point
     * @param endX X-coordinate of end point
     * @param endY Y-coordinate of end point
     * @return INDICES-list with square indices
     */
    static DoubleLinkedList<INDICES*>* getSquaresBetween(int startX, int startY, int endX, int endY, int maxSize);

private:

    /**
     * Cuts the ray between two planes and returns the plane-intersection points
     * @param rayOrigin  Origin of the ray
     * @param rayDir     Direction of the ray     
     * @return Two points as D3DXVECTOR3s
     */
    static D3DXVECTOR3* getPointsFromPlaneClippedRay(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir);

    /**
     * Culls patches based on their AABBs and ray-origin and -direction
     * @param patches    INDICES-list of patch indices to check, NOTE: this list will be destroyed inside this method
     * @param rayOrigin  Origin of the ray
     * @param rayDir     Direction of the ray
     * @return New list of patches left (the entered one is destroyed)
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