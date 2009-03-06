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

class UITerrain
{
public:

    /**
     * Friend class for intersection tests
     */
    friend class TerrainIntersection;

    /**
     * Returns the instance of terrain, note that create needs to be called first
     * @return UITerrain-pointer or NULL, if create() hasn't been called
     */
    static UITerrain* getInstance();

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
     */
    static const int PATCHSIZE = 32;

    /**
     * Creates a new terrain mesh from Model Terrain-data, MUST BE CALLED BEFORE getInstance IS USED
     * @param pDevice LPDIRECT3DDEVICE9 to use for creation
     * @return HRESULT, S_OK if successfull, otherwise error code
     */
    static HRESULT create(LPDIRECT3DDEVICE9 pDevice);

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
     * Returns the normal at given position
     * @param x X-position to get the normal from
     * @param y Y-position to get the normal from
     * @return D3DXVECTOR3 representing the normal, or NULL if out of map
     */
    D3DXVECTOR3 getNormalAt(float x, float y, int j, int k);

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
     * Sets the detail level for terrain and recalculates indexbuffers
     */
    void setDetailLevel(unsigned char detailLevel);

    /**
     * Sets the detail level for terrain and recalculates vertex- and indexbuffers
     * @param detailLevel Value from 0 upwards, 0 is the most detailed (2 triangles per 
     * grid square)
     */
    void setDetailLevel2(unsigned char detailLevel);

    /**
     * Returns the detail level, smaller is better
     */
    inline unsigned char getDetailLevel() { return m_DetailLevel; }

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
     * UITerrain singleton instance
     */
    static UITerrain* pInstance;

    /**
     * World matrix for positioning
     */
    D3DXMATRIX m_World;

    /**
     * Vertexbuffers for terrain mesh patches
     */
    LPDIRECT3DVERTEXBUFFER9** m_pppVB;

    /**
     * Indexbuffers for terrain mesh patches
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
};

#endif // __UITERRAIN_H__