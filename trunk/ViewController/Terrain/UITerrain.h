/**
 * UITerrain
 *
 * Handles the ui 3d-representation of the terrain, SINGLETON
 *
 * $Revision$
 * $Date$
 * $Id$
 */


#ifndef __UITERRAIN_H__
#define __UITERRAIN_H__


#include <d3dx9.h>
#include "MiniMap.h"
#include "../../Model/Player/PlayerManager.h"

class UITerrain
{
public:

    /**
     * Friend class for intersection tests
     */
    friend class TerrainIntersection;

    /**
     * Returns the instance of terrain, note that create needs to be called first before
     * trying to do anything with it
     * @return UITerrain-pointer or NULL, if create() hasn't been called
     */
    static UITerrain* getInstance()
    {
        static UITerrain instance;
        return &instance;
    }

    /**
     * Destructor
     */
    virtual ~UITerrain();

    /**
     * Height factor for scaling
     */
    static const float HEIGHTFACTOR;

    /**
     * Size of the side of a single terrain patch, actual size is PATCHSIZE * PATCHSIZE tiles
     * NOTE! Value should be 2^n and terrain size (in model-side) must be divisable by this! 
     * Do not use value over 128 (indexbuffer is limited to 16 bits, and needed vertice amount has some overhead)!
     */
    static const int PATCHSIZE = 64;
    static const int PATCHSIZEPLUSONE = 65;

    /**
     * Creates a new terrain mesh from Model Terrain-data
     * @param pDevice LPDIRECT3DDEVICE9 to use for creation
     * @return HRESULT, S_OK if successfull, otherwise error code
     */
    HRESULT create(LPDIRECT3DDEVICE9 pDevice, Player* pCurrentPlayer);

    /**
     * Releases resources held by this instance
     */
    void release();

    /**
     * Renders the terrain
     * @param pDevice LPDIRECT3DDEVICE9 to use for rendering
     */
    void render(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Returns the normal at given position, or if multiple tiles, the average of normals within area
     * @param x X-position to get the normal from
     * @param y Y-position to get the normal from
     * @param tiles From how big area (tiles * tiles) around the position should normals be averaged
     * @return D3DXVECTOR3 representing the normal, or NULL if out of map
     */
    D3DXVECTOR3 getNormalAt(float x, float y, int tiles);

    /**
     * Calculates height of the map at given position in current detail level
     */
    float calculateTriangleHeightAt(float x, float y);

    /**
     * Creates a red/green texture based on passability values of map tiles
     * @param pDevice LPDIRECT3DDEVICE9 to use for texture creation
     */
    HRESULT createPassabilityTexture(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Creates a colormapped texture based on height values of map tiles
     * @param pDevice LPDIRECT3DDEVICE9 to use for texture creation     
     */
    HRESULT createColorMapTexture(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Updates a colormapped texture based on height values of map tiles and given fog
     * @param fogArray array of fog, null can be passed
     */
    HRESULT updateColorMapTexture(bool** fogArray);    

    /**
     * Gets the colormap texture of the terrain
     * @param LPDIRECT3DTEXTURE9 to texture
     */
    LPDIRECT3DTEXTURE9 getColorMapTexture() { return m_pPixelTexture; }

    /**
     * Sets the fillmode
     * @param fillMode D3DFILLMODE-enum value
     */
    inline void setFillMode(D3DFILLMODE fillMode)
    {
        m_FillMode = fillMode;
    }

    /**
     * Returns current fillmode
     * @return D3DFILLMODE-enum value
     */
    inline D3DFILLMODE getFillMode() { return m_FillMode; }
        
    /**
     * Sets the detail level for terrain and recalculates vertex- and indexbuffer
     * @param detailLevel Value from 0 upwards, 0 is the most detailed (2 triangles per 
     * grid square), on higher levels, 2^detailLevel squares per axis are combined to one
     */
    void setDetailLevel(unsigned char detailLevel);

    /**
     * Returns the detail level, smaller is better
     */
    inline unsigned char getDetailLevel() { return m_DetailLevel; }

    /**
     * Returns the minimap associated with the terrain
     * @return Pointer to MiniMap -instance
     */
    inline MiniMap* getMiniMap() { return &m_MiniMap; }

    /**
     * Returns the value of the changecounter, used by outside objects to
     * check if the countervalue has changed from what they have stored, so
     * they know to update themselves (on things related to uiterrain)
     * @return Countervalue, wraps from 65535 to 0
     */
    inline const unsigned short getChangeCounter() const { return m_ChangeCounter; }

    /**
     ' Increases change counter by one
     */
    inline void increaseChangeCounter() { ++m_ChangeCounter; }

    /**
     * Releases D3DPOOL_DEFAULT -resources
     * @return S_OK if releasing went fine, otherwise error code
     */
    inline HRESULT onLostDevice()
    {
        HRESULT hres = S_OK;

        if(m_pPixelTexture)
        {            
            hres = m_pPixelTexture->Release();
            if(FAILED(hres))
            {
                return hres;
            }
            m_pPixelTexture = NULL;
        }

        hres = m_MiniMap.onDeviceLost();
        if(FAILED(hres))
        {
            return hres;
        }

        return hres;
    }

    /**
     * Recreates D3DPOOL_DEFAULT -resources
     * @param pDevice Pointer to D3D-device
     * @return HRESULT telling if recreation was successful
     */
    inline HRESULT onRestore(LPDIRECT3DDEVICE9 pDevice)
    {
        if(m_pPixelTexture)
        {
            HRESULT hres = m_pPixelTexture->Release();
            if(FAILED(hres))
            {
                return hres;
            }
            m_pPixelTexture = NULL;
        }
        
        bool** fog = NULL;
        if (m_pCurrentPlayer)
            fog = m_pCurrentPlayer->getFog()->getFogArray();

        HRESULT hres = createColorMapTexture(pDevice);        
        if(FAILED(hres))
        {
            return hres;
        }
        hres = updateColorMapTexture(fog);
        if(FAILED(hres))
        {
            return hres;
        }

        hres = m_MiniMap.onRestore(pDevice);
        if(FAILED(hres))
        {
            return hres;
        }

        return hres; 
    }

    /**
     * updates fog
     */
    void updateFog(LPDIRECT3DDEVICE9 pDevice);

    inline Player* getCurrentPlayer() { return m_pCurrentPlayer; }

    /**
     * Force terrain update
     */
    void reCreate();

    /**
     * Checks if the model-side terrain has changed and updates as needed
     */     
    void update()
    {
        unsigned short value = Terrain::getInstance()->getChangeCounter();

        if(value != m_ModelTerrainCounter)
        {
            m_ModelTerrainCounter = value;
            reCreate();
        }
    }

private:
    
    /**
     * Constructor, not public because class is singleton
     */
    UITerrain();

    /**
     * Internal initialization
     * @param pDevice Pointer to D3D9 -device
     * @return S_OK, if everything went ok, otherwise error code
     */
    HRESULT initialize(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Internal initialization for multiple index/vertex buffers
     * @param pDevice Pointer to D3D9 -device
     * @return S_OK, if everything went ok, otherwise error code
     */
    HRESULT initializeMulti(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Sets the detail level for terrain and recalculates vertex- and indexbuffers for multi-
     * buffer rendering, called internally from setDetailLevel
     */
    void setDetailLevelMultiBuffer();

    /**
     * Calculates the normal for vertex in location x, y
     * @param x X-location of the vertex
     * @param y Y-location of the vertex
     * @return D3DXVECTOR3 containing normalized normal for the vertex
     */
    D3DXVECTOR3 calculateNormalForVertex(unsigned short x, unsigned short y);

    /**
     * Calculates triangle normals into the m_ppTriangleNormals-array for current 
     * detail level.
     */
    void calculateTriangleNormals();

    /**
     * Calculates average height for given vertex on current detail-level
     * @param x X-location of the vertex
     * @param y Y-location of the vertex
     */
    float calculateAverageHeightForVertex(unsigned short x, unsigned short y);

    /**
     * World matrix for positioning
     */
    D3DXMATRIX m_World;

    /**
     * Vertexbuffer for terrain mesh
     */
    LPDIRECT3DVERTEXBUFFER9 m_pVB;

    /**
     * Indexbuffer for terrain mesh patches
     */ 
    LPDIRECT3DINDEXBUFFER9 m_pIB;

    /**
     * Vertexbuffers for multibuffer-rendering
     */
    LPDIRECT3DVERTEXBUFFER9** m_pppVB;

    /**
     * Indexbuffers for multibuffer-rendering
     */ 
    LPDIRECT3DINDEXBUFFER9** m_pppIB;


    /**
     * Texture for terrain
     */
    LPDIRECT3DTEXTURE9 m_pTexture;

    /**
     * Pixel-texture (colormap)
     */
    LPDIRECT3DTEXTURE9 m_pPixelTexture;

    /**
     * Material
     */
    D3DMATERIAL9 m_Mat;

    /**
     * Number of vertices per terrain patch
     */
    UINT m_NumVertices;

    /**
     * Number of triangles per patch
     */
    UINT m_NumPrimitives;

    /**
     * Fillmode used for drawing (for debugging)
     */
    D3DFILLMODE m_FillMode;

    /**
     * How many times the texture repeats over terrain
     */
    float m_TextureRepeat;

    /**
     * Look-up table of triangle normals. NOTE: if other than highest detail level
     * (= level 0) is used, the data will NOT be placed according to Model-side
     * terrain squares
     */
    D3DXVECTOR3*** m_pppTriangleNormals;

    /**
     * Look-up table of patch axis-aligned bounding boxes (AABB), defined by two
     * D3DXVECTORS per box, indices are x and y position of the patches ([0][0], [2][1])
     * with Y as first dimension
     */
    D3DXVECTOR3*** m_pppPatchAABBs;

    /**
     * Size of terrain, m_Size * m_Size -tiles
     */
    unsigned short m_Size;

    /**
     * Number of terrain patches per side (total amount = m_Patches * m_Patches)
     */
    unsigned short m_Patches;

    /**
     * Global detail-level of terrain patches, how many tiles are represented by a quad
     */
    unsigned char m_DetailLevel;

    /**
     * For old videocards and laptops
     */
    bool m_MultiBufferRender;

    /**
     * MiniMap
     */
    MiniMap m_MiniMap;

    /**
     * Changecounter, used by outside classes to check if the UITerrain has been changed and
     * to react on said changes
     */
    unsigned short m_ChangeCounter;

    // used to check if fog has changed
    unsigned short m_FogChangeCounter;

    // player whose fog is showing
    Player* m_pCurrentPlayer;

    /**
     * Model-side terrain changecounter value, used to track if terrain has changed
     */
    unsigned short m_ModelTerrainCounter;
};

#endif // __UITERRAIN_H__