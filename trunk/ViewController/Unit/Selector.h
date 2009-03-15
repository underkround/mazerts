/**
 * Mesh and methods for selecting units and terrain with mouse dragging
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "d3dx9.h"
#include "UIUnit.h"
#include "UI3DObjectManager.h"

#ifndef __SELECTOR_H__
#define __SELECTOR_H__

class Selector
{
public:

    /**
     * Selector mesh size (quads per side)
     */
    const static int SELECTOR_SIZE = 20;    

    /**
     * Holds data about selected area
     */
    struct SELECTION
    {
        /** 
         * DoubleLinkedList of selected UI-units
         */
        DoubleLinkedList<UIUnit*> units;

        /**
         * The two selection points in 3d (terrain points)
         */
        D3DXVECTOR3 point1;
        D3DXVECTOR3 point2;
    };

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

        ::memset(&m_Mat, 0, sizeof(D3DMATERIAL9));
        m_Mat.Emissive = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
        m_Render = false;
        m_FirstSet = false;
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
     * Creates the index- and vertex-buffers and loads texture
     */
    HRESULT create(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Set the point of the selector, first call sets the start
     * point, after that the call moves end point, until buttonUp
     * is called (this is used for area selection)
     * @param point Point xy-coordinates as D3DXVECTOR2
     */
    void setPoint(D3DXVECTOR2 point)
    {
        bool updateGrid = false;

        //Don't update unless the point has moved
        if(point.x != m_Point1.x && point.y != m_Point1.y &&
           point.x != m_Point2.x && point.y != m_Point2.y)
        {
            updateGrid = true;
        }

        if(m_FirstSet)
        {
            m_Point2 = point;
            m_Render = true;
        }
        else
        {
            m_Point1 = point;            
            m_FirstSet = true;
        }
        
        if(updateGrid)
        {
            update();
        }
    }

    /**
     * Set the both points and updates the mesh data to match these
     * @param point1 D3DXVECTOR2 containing start point xy-coordinates
     * @param point2 D3DXVECTOR2 containing end point xy-coordinates
     *        It won't really matter which is the start and which the
     *        end point, the mesh is drawn correctly both ways
     */
    void setPoints(D3DXVECTOR2 point1, D3DXVECTOR2 point2)
    {
        m_Point1 = point1;
        m_Point2 = point2;
        m_FirstSet = true;
        update();
    }

    /**
     * Sets whether the selector is rendered or not on Render-call
     * @param render True to enable rendering, false to disable
     */
    void setRender(bool render)
    {
        m_Render = render;
    }

    /**
     * Renders the selector
     * @param pDevice Device to render with
     */
    void render(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Called when the mouse button is released, the selector will return any selected units and
     * two D3DXVECTOR3s representing the selected points in terrain and disable the rendering of
     * the selector-mesh
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
     * Material, full emissive
     */
    D3DMATERIAL9 m_Mat;

    /**
     * Buffer data amount, vertex and primitive count
     */
    DWORD m_NumVertices;
    DWORD m_NumPrimitives;

    /**
     * Used internally to control whether selector is rendered
     */
    bool m_Render;

    /**
     * Used to control which point to set
     */
    bool m_FirstSet;
};

#endif //__SELECTOR_H__