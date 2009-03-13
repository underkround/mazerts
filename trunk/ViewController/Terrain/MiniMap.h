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

class MiniMap
{
public:
    MiniMap();
    virtual ~MiniMap();

    /**
     * Creates the minimap vertex-array
     */
    void create();

    /**
     * Release minimap resources
     */
    void release();

    /**
     * Set the base texture used by minimap
     * @param pTexture Pointer to texture used as base colormap (usually terrain colormap)
     */
    inline void setTexture(LPDIRECT3DTEXTURE9 pTexture)
    {
        m_pTexture = pTexture;
    }

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
     * Render the minimap
     */
    void render(LPDIRECT3DDEVICE9 pDevice);

private:

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
     * Minimap uses terrain pixel-texture direcly, so only pointer to texture is needed
     */
    LPDIRECT3DTEXTURE9 m_pTexture;

    /**
     * Vertices of the back quad
     */
    TRANSLITVERTEX* m_pBackVertices;

    /**
     * Vertices of the unit-markers
     */
    TRANSLITVERTEX* m_pUnitVertices;

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