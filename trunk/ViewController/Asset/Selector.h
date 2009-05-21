/**
 * Mesh and methods for selecting assets and terrain with mouse dragging
 *
 * $Revision: 218 $
 * $Date: 2009-03-26 19:36:13 +0200 (to, 26 maalis 2009) $
 * $Id: Selector.h 218 2009-03-26 17:36:13Z ezbe $
 */

#include "d3dx9.h"
#include "UIAsset.h"
#include "UI3DObjectManager.h"

#ifndef __SELECTOR_H__
#define __SELECTOR_H__

class Selector
{
public:

    /**
     * Selector states for different usage of selector
     */
    enum SelectorState
    {
        SELECTOR_NORMAL,                //For normal selections
        SELECTOR_BUILDINGPLACEMENT      //For placing buildings
    };

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
         * DoubleLinkedList of selected UI-assets
         */
        DoubleLinkedList<UIAsset*> assets;

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
        
        m_SelectorState = SELECTOR_NORMAL;
        m_BuildingPlacementSize.x = 1;
        m_BuildingPlacementSize.y = 1;
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
     * Set the point of the selector
     * In SELECTOR_NORMAL-state, first call sets the start
     * point, after that the call moves end point, until buttonUp
     * is called (this is used for area selection)
     * In SELECTOR_BUILDINGPLACEMENT-state, the call sets the
     * position of the center point for selector
     * @param point Point grid square xy-coordinates as D3DXVECTOR2
     */
    void setPoint(D3DXVECTOR2 point)
    {
        if(m_SelectorState == SELECTOR_NORMAL)
        {
            bool updateGrid = false;

            //Don't update unless the point has moved
            if(point.x != m_Point2.x || point.y != m_Point2.y)
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
        else if(m_SelectorState == SELECTOR_BUILDINGPLACEMENT)
        {
            m_Render = true;
            m_Point1 = point;
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
     * Sets the size for the Selector for BUILDINGPLACEMENT-selectorstate.
     * @param size D3DXVECTOR2 containing the size in gamemap squares
     */
    void setSize(D3DXVECTOR2 size)
    {
        m_BuildingPlacementSize = size;
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
     * Call when the mouse button is released
     *
     * In SELECTOR_NORMAL -state, the selector will return pointer to SELECTION
     * -struct contaning any selected assets and two D3DXVECTOR3s representing
     * the selected points in terrain and disable the rendering of the selector-mesh
     *
     * In SELECTOR_BUILDINGPLACEMENT -state, empty SELECTION will be always returned,
     * use isBuildable instead for checking buildable areas, and use setRender to
     * hide Selector
     *
     * @return Pointer to SELECTION
     */
    SELECTION* buttonUp();
    
    /**
     * Changes the state of the selector
     * In SELECTOR_NORMAL -state, the selector acts "normally", meaning it can be used to
     * select rectangular areas from game map, and buttonUp returns a pointer to SELECTION
     * -struct containing the units within the selected area
     *
     * In SELECTOR_BUILDINGPLACEMENT -state, the selectors size can be set with setSize(w, h)
     * and isBuildable can be called to check if the selected area is buildable
     *
     * @param state Value from SelectorState-enumeration
     */
    inline void setState(SelectorState state) 
    {
        m_SelectorState = state;
    }

    /**
     * Checks if the current position of the selector is buildable for current size-setting
     * @return true, if the position is buildable, otherwise false, false is also always
     *         returned, if the state is set to SELECTOR_NORMAL instead of SELECTOR_BUILDINGPLACEMENT
     */
    bool isBuildable();

    /**
     * Called when device is lost, releases D3DPOOL_DEFAULT-resources
     * @return S_OK or errorcode
     */
    HRESULT onDeviceLost();

    /**
     * Called when device is restored, recreates D3DPOOL_DEFAULT-resources
     * @param pDevice Pointer to Direct3D-device
     * @return S_OK or errorcode
     */
    HRESULT onRestore(LPDIRECT3DDEVICE9 pDevice);

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
    
    /**
     * State of the selector, for different type of selector
     */
    SelectorState m_SelectorState;

    /**
     * Size used for the mesh-grid in SELECTOR_BUILDINGPLACEMENT-state
     */
    D3DXVECTOR2 m_BuildingPlacementSize;
};

#endif //__SELECTOR_H__