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
#include "../Asset/UIAsset.h"
#include "../Camera/Camera.h"

//Maximum amount of unit-markers the minimap allocates vertexbuffer for
#define MINIMAP_MAX_ASSETS 2000


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
     * @param x How many pixels right
     * @param y How many pixels down
     */
    void setPosition(const int x, const int y);

    /**
     * Returns the position of minimap as D3DXVECTOR2
     * @return Position of minimap as D3DXVECTOR2
     */
    inline const D3DXVECTOR2 getPosition() const { return m_Position; }

    /**
     * Set the size of minimap (size * size pixels)
     * @param size Size of one side of minimap in pixels
     */
    void setSize(const int size);

    /**
     * Returns the size of a single minimap size
     * @return Size as int
     */
    inline const int getSize() const { return (int)m_Size; }

    /**
     * Set the update interval (in seconds) between unit-position updates
     * @param interval Interval in seconds
     */
    inline void setUpdateInterval(const float interval) { m_UpdateInterval = interval; }

    /** 
     * Updates the Asset positions in minimap
     * @param pAssetList Pointer to list containing the units
     * @param deltaTime Time passed since last update in seconds
     */
    void updateAssets(DoubleLinkedList<UIAsset*>* pAssetList, float deltaTime);

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

    /**
     * Called on device lost, releases D3DPOOL_DEFAULT -resources
     */
    HRESULT onDeviceLost();

    /**
     * Called after restoring from device lost, recreates D3DPOOL_DEFAULT-resources
     * @param pDevice Direct3D-device to recreate the resources with
     */
    HRESULT onRestore(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Updates the camera to new position if minimap is clicked
     */
    void moveCameraOnClick();

    /**
     * Converts the given pixel coordinates to world coordinates
     * @param coordinates D3DXVECTOR2 containing pixel coordinates from TOPLEFT CORNER of the screen, NOT map
     */
    D3DXVECTOR2 minimapPositionToWorld(D3DXVECTOR2 coordinates);

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
    LPDIRECT3DVERTEXBUFFER9 m_pAssetVB;

    /**
     * Camera-marker vertexbuffer
     */
    LPDIRECT3DVERTEXBUFFER9 m_pCameraVB;

    /**
     * Number of Asset primitives
     */
    int m_AssetPrimitiveCount;

    /**
     * Time in seconds between which the minimap is updated
     */
    float m_UpdateInterval;

    /**
     * Update counter for interval measuring
     */
    float m_UpdateCounter;

    /**
     * Used to update the camera location in minimap only
     * when countervalue has changed in Camera->getChangeCounter
     */
    unsigned short m_CameraChangeCounter;
};


#endif //__MINIMAP_H__