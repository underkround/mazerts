/**
 * C3DObject.h header file
 *
 * concrete 3d object that is capable of rendering
 * direct3d mesh objects
 */

#ifndef __C3DOBJECT_H__
#define __C3DOBJECT_H__

#include "I3Dobject.h"

#include <vector>
using namespace std;


class C3DObject : public I3DObject
{
public:
    // struct for storing the object materials and
    // textures
    struct MESHDATA
    {
        D3DMATERIAL9*           pMaterial;
        LPDIRECT3DTEXTURE9      pTexture;
    };

    C3DObject(void);
    virtual ~C3DObject(void);

    /**
     * Create
     * initialise the 3d object
     * @param pMesh the mesh to use with object
     */
    void Create(LPD3DXMESH pMesh);

    /**
     * Release
     * release the 3d object
     */
    virtual void Release(void);

    /**
     * Render
     * render the mesh object
     * @param pDevice device to render with
     */
    virtual void Render(LPDIRECT3DDEVICE9 pDevice);

    /**
     * AddMeshData
     * add new meshdata structure to object
     * @param meshdata reference to new meshdata
     */
    void AddMeshData(MESHDATA& meshdata);



    /**
     * GetMesh
     * @return 3d object mesh
     */
    inline LPD3DXMESH GetMesh(void) { return m_pMesh; }

    /**
     * GetMeshDataArray
     * @return reference to array of mesh data structures
     */
    inline vector<MESHDATA>& GetMeshDataArray(void) { return m_arrMeshData; }

    /**
     * Sets the axis-aligned bounding box size
     * @param size D3DXVECTOR3 representing the box-size from 0, 0, 0 to size.x, size.y, size.z     
     */
    inline void setAABBSize(D3DXVECTOR3 size)
    {
        m_AABBMin = size * -0.5f;
        m_AABBMax = size * 0.5f;
    }

    /**
     * Gets the vectors defining the unit AABB at its current location
     * @param pAABBMin Pointer to fill
     */
    inline void getAABBVectors(D3DXVECTOR3* pAABBMin, D3DXVECTOR3* pAABBMax) const
    {
        pAABBMin->x = m_AABBMin.x + m_mLocal._41;
        pAABBMin->y = m_AABBMin.y + m_mLocal._42;
        pAABBMin->z = m_AABBMin.z + m_mLocal._43;

        pAABBMax->x = m_AABBMax.x + m_mLocal._41;
        pAABBMax->y = m_AABBMax.y + m_mLocal._42;
        pAABBMax->z = m_AABBMax.z + m_mLocal._43;
    }

protected:
    // an array of mesh data structures
    vector<MESHDATA>        m_arrMeshData;

    // object mesh
    LPD3DXMESH              m_pMesh;

    /**
     * Axis-aligned bounding box
     */
    D3DXVECTOR3 m_AABBMin;
    D3DXVECTOR3 m_AABBMax;
};

#endif    // __C3DOBJECT_H__

