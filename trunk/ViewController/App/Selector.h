/**
 * Mesh and methods for selecting units and terrain with mouse dragging
 *
 * $Revision$
 * $Date$
 * $Id$
 */

//TODO: Finish what you started, you lazy bum

#include "d3dx9.h"
#include "../Unit/UIUnit.h"

#ifndef __SELECTOR_H__
#define __SELECTOR_H__

class Selector
{
public:

    const static int SELECTOR_WIDTH = 32;
    const static int SELECTOR_HEIGHT = 32;

    struct SELECTION
    {
        /** 
         * Array of pointers to UI-units
         */
        UIUnit** pUnit;

        /**
         * Amount of selected units
         */
        int unitAmount;

        /**
         * The two selection points in 3d (terrain points)
         */
        D3DXVECTOR3 point1;
        D3DXVECTOR3 point2;
    };

    /**
     * Constructor     
     */
    Selector()
    {
        m_pDevice = NULL;        
        m_pVB = NULL;
        m_pIB = NULL;
        m_pTexture = NULL;

        m_Point1.x = 0;
        m_Point1.y = 0;
        m_Point2.x = 0;
        m_Point2.y = 0;

        D3DXMatrixIdentity(&m_mWorld);
        m_Render = false;
    }

    virtual ~Selector()
    {
        if(m_pIB)
        {
            m_pIB->Release();
            m_pIB = NULL;
        }
        
        if(m_pVB)
        {
            m_pVB->Release();
            m_pVB = NULL;
        }

        if(m_pTexture)
        {
            m_pTexture->Release();
            m_pTexture = NULL;
        }
    }
  
    /**
     * Creates the mesh
     */
    HRESULT create(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Set the point of the selector
     */
    void setPoint(D3DXVECTOR2 point)
    {        
        if(m_Render)
        {
            m_Point2 = point;            
        }
        else
        {
            m_Point1 = point;            
        }
        update();
    }

    /**
     * Renders the selector
     * @param pDevice Device to render with
     */
    void render(LPDIRECT3DDEVICE9 pDevice);

    /**
     * When the button is released, the selector will return any selected units and two D3DXVectors
     * representing the selected points in terrain
     * @return Pointer to SELECTION
     */
    SELECTION* buttonUp();

private:

    /**
     * Updates the mesh
     * @return S_OK if everything went fine, otherwise error code
     */
    HRESULT update();

    /**
     * The points of a rectangle used in selection
     */
    D3DXVECTOR2 m_Point1;
    D3DXVECTOR2 m_Point2;

    /**
     * Direct3D-device for creating mesh
     */
    LPDIRECT3DDEVICE9 m_pDevice;

    /**
     * Vertex-buffer for the selector-mesh
     */
    LPDIRECT3DVERTEXBUFFER9 m_pVB;

    /**
     * Index-buffer for the selector-mesh
     */
    LPDIRECT3DINDEXBUFFER9 m_pIB;

    /**
     * Texture for the selector-mesh
     */
    LPDIRECT3DTEXTURE9 m_pTexture;

    /**
     * World-matrix
     */
    D3DXMATRIX m_mWorld;

    /**
     * Buffer data amount
     */
    DWORD m_NumVertices;
    DWORD m_NumPrimitives;

    /**
     * Used to control whether this is rendered
     */
    bool m_Render;
};

#endif //__SELECTOR_H__