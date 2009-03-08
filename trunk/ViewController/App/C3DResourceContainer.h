/**
 * C3DResourceContainer.h header file
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 19.2.2009
 * 
 * This file declares 3d resource and its container
 * classes
 */

#ifndef __3DRESOURCECONTAINER_H__
#define __3DRESOURCECONTAINER_H__

#include <tchar.h>
#include <d3dx9.h>

// include STL vector as a dynamic array
#include <vector>
using namespace std;


class C3DResourceContainer
{
public:
    enum TYPE
    {
        eTYPE_TEXTURE = 0,
        eTYPE_VB,
        eTYPE_IB,
        eTYPE_MESH,
        eTYPE_MATERIAL,
        eTYPE_LIGHT,
        eTYPE_END=0xffffffff
    };
    struct RESOURCE
    {
        void*                        pData;
        TCHAR*                        strName;
        C3DResourceContainer::TYPE    eType;
    };

    C3DResourceContainer(void);
    virtual ~C3DResourceContainer(void);

    /**
     * Create
     * initialise the resource container
     * @param pDevice initialised 3d device object
     * @return S_OK or an error code
     */
    HRESULT Create(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Release
     * release all resources in container
     */
    void Release(void);

    /**
     * AddResource
     * add new resource into the conteiner, but only if
     * resource with same name doesn't already exist.
     * @param strName name of the new resource
     * @param pData/material/light reseouce to add to container
     * @return    S_OK if resource was added to container
     *            E_ACCESSDENIED if resource with same name already exists
     *            E_OUTOFMEMORY if out of memory
     */
    HRESULT AddResource(const TCHAR* strName, LPDIRECT3DTEXTURE9 pTexture);
    HRESULT AddResource(const TCHAR* strName, LPDIRECT3DVERTEXBUFFER9 pVB);
    HRESULT AddResource(const TCHAR* strName, LPDIRECT3DINDEXBUFFER9 pIB);
    HRESULT AddResource(const TCHAR* strName, LPD3DXMESH pMesh);
    HRESULT AddResource(const TCHAR* strName, const D3DMATERIAL9& material);
    HRESULT AddResource(const TCHAR* strName, const D3DLIGHT9& light);

    /**
     * FindResource
     * find any resource with name strName
     * @param strName name of the resource to search
     * @return pointer to resource or NULL of not found
     */
    const RESOURCE* FindResource(const TCHAR* strName) const;

    /**
     * FindTexture
     * find resource with specified name
     * @param strName name of the resource to find
     * @return found resource or NULL if not found
     */
    LPDIRECT3DTEXTURE9 FindTexture(const TCHAR* strName) const;

    /**
     * FindVB
     * find resource with specified name
     * @param strName name of the resource to find
     * @return found resource or NULL if not found
     */
    LPDIRECT3DVERTEXBUFFER9 FindVB(const TCHAR* strName) const;

    /**
     * FindIB
     * find resource with specified name
     * @param strName name of the resource to find
     * @return found resource or NULL if not found
     */
    LPDIRECT3DINDEXBUFFER9 FindIB(const TCHAR* strName) const;

    /**
     * FindMesh
     * find resource with specified name
     * @param strName name of the resource to find
     * @return found resource or NULL if not found
     */
    LPD3DXMESH FindMesh(const TCHAR* strName) const;

    /**
     * FindMaterial
     * find resource with specified name
     * @param strName name of the resource to find
     * @return found resource or NULL if not found
     */
    D3DMATERIAL9* FindMaterial(const TCHAR* strName) const;

    /**
     * FindLight
     * find resource with specified name
     * @param strName name of the resource to find
     * @return found resource or NULL if not found
     */
    D3DLIGHT9* FindLight(const TCHAR* strName) const;

    /**
     * GetResources
     * @return reference to resource array
     */
    inline vector<RESOURCE*>& GetResources(void) { return m_arrResources; }

    /**
     * GetDevice
     * @return pointer to 3d device used to initialise the container
     */
    inline LPDIRECT3DDEVICE9 GetDevice(void) { return m_pDevice; }

private:
    /**
     * AddResource
     * internal resource adding function
     */
    HRESULT AddResource(const TCHAR* strName, TYPE eType, void* pData);

    /**
     * ReleaseResource
     * release the resource data
     */
    void ReleaseResource(RESOURCE& resource) const;

    LPDIRECT3DDEVICE9                m_pDevice;
    vector<RESOURCE*>                m_arrResources;
};

#endif    // __3DRESOURCECONTAINER_H__

