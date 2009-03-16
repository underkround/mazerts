/**
 * Minimap showing the terrain with visible unit and building locations
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include <d3dx9.h>
#include "../App/Vertices.h"
#include "../../Model/Common/DoubleLinkedList.h"
#include "../Unit/UiUnit.h"
#include "../Camera/Camera.h"

//Maximum amount of unit-markers the minimap allocates vertexbuffer for
#define MINIMAP_MAX_UNITS 2000


class MiniMap
{
public:
    MiniMap();
    virtual ~MiniMap();

    /**
     * Creates the minimap vertex-array
     * @param pDevice Direct3D-device to use
     * @return S_OK or error code
     */
    HRESULT create(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Release minimap resources
     */
    void release();

    /**
     * Set the position of minimap (in pixels from upperleft corner)
     * @param x
     * @param y
     */
    void setPosition(const int x, const int y);

    /**
     * Set the size of minimap (size * size pixels)
     * @param size Size of one side of minimap in pixels
     */
    void setSize(const int size);

    /**
     * Set the update interval (in seconds) between unit-position updates
     * @param interval Interval in seconds
     */
    inline void setUpdateInterval(const float interval) { m_UpdateInterval = interval; }

    /** 
     * Updates the unit positions in minimap
     * @param pUnitList Pointer to list containing the units
     * @param deltaTime Time passed since last update in seconds
     */
    void updateUnits(DoubleLinkedList<UIUnit*>* pUnitList, float deltaTime);

    /**
     * Updates the camera position in minimap
     * @param pDevice Pointer to Direct3D-device     
     */
    void updateCamera(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Render the minimap
     * @param pDevice Device to render with
     * @param pTexture Texture to use as background
     */
    void render(LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DTEXTURE9 pTexture);

private:

    /** 
     * Intersect the given ray at the given z-height of xy-plane
     * @param rayOrigin Ray starting point
     * @param rayDir Ray direction
     * @param clipZ XY-plane "height" (z-value) to intersect with
     * @return D3DXVECTOR3 with intersection coordinates
     */
    D3DXVECTOR3 intersectRay(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir, float clipZ);

    /**
     * Minimap left upper corner position (from upper left corner of screen, in pixels)
     */
    D3DXVECTOR2 m_Position;

    /**
     * Minimap size in pixels
     */
    float m_Size;

    /**
     * Factor for positioning unit markers, minimapsize / terrainsize
     */
    float m_SizeFactor;

    /**
     * Background (map-textured quad) vertexbuffer
     */
    LPDIRECT3DVERTEXBUFFER9 m_pBackVB;

    /**
     * Unit-marker vertexbuffer
     */
    LPDIRECT3DVERTEXBUFFER9 m_pUnitVB;

    /**
     * Camera-marker vertexbuffer
     */
    LPDIRECT3DVERTEXBUFFER9 m_pCameraVB;

    /**
     * Number of unit primitives
     */
    int m_UnitPrimitiveCount;

    /**
     * Time in seconds between which the minimap is updated
     */
    float m_UpdateInterval;

    /**
     * Update counter for interval measuring
     */
    float m_UpdateCounter;
};


#endif //__MINIMAP_H__