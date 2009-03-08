/**
 * C3DResourceContainer.cpp source file
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 19.2.2009
 * 
 * This file declares 3d resource and its container
 * classes
 */

#include "C3DResourceContainer.h"


C3DResourceContainer::C3DResourceContainer(void)
{
    m_pDevice = NULL;
}

C3DResourceContainer::~C3DResourceContainer(void)
{
}


HRESULT C3DResourceContainer::Create(LPDIRECT3DDEVICE9 pDevice)
{
    m_pDevice = pDevice;
    return S_OK;
}


void C3DResourceContainer::Release(void)
{
    // release all resources in container
    DWORD i;
    for (i=0; i<m_arrResources.size(); i++)
    {
        RESOURCE* pResource = m_arrResources[i];
        ReleaseResource(*pResource);
        delete pResource;
    }
    m_arrResources.clear();
}


void C3DResourceContainer::ReleaseResource(RESOURCE& resource) const
{
    if (resource.pData)
    {
        // release the contained resource
        switch (resource.eType)
        {
        case eTYPE_TEXTURE:
        case eTYPE_VB:
        case eTYPE_IB:
        case eTYPE_MESH:
            {
                // release direct 3d resource
                IUnknown* pObject = (IUnknown*)resource.pData;
                pObject->Release();
            }
            break;

        case eTYPE_MATERIAL:
            {
                // delete material
                D3DMATERIAL9* pMaterial = (D3DMATERIAL9*)resource.pData;
                delete pMaterial;
            }
            break;

        case eTYPE_LIGHT:
            {
                // delete light
                D3DLIGHT9* pLight = (D3DLIGHT9*)resource.pData;
                delete pLight;
            }
            break;

        default:
            // unknown resource type
            // shold neverever come here!
            break;
        }

        resource.pData = NULL;
    }

    // release the name
    if (resource.strName)
    {
        delete [] resource.strName;
        resource.strName = NULL;
    }

    resource.eType = eTYPE_END;
}


HRESULT C3DResourceContainer::AddResource(    const TCHAR* strName,
                                            C3DResourceContainer::TYPE eType,
                                            void* pData)
{
    // check if resource with same name already exists
    if (FindResource(strName))
    {
        // we already have resource with same name, do not add
        return E_ACCESSDENIED;
    }

    // create new resource
    RESOURCE* newresource = new RESOURCE;
    if (newresource == NULL)
    {
        return E_OUTOFMEMORY;
    }

    newresource->pData = pData;
    newresource->eType = eType;

    const int length = _tcslen(strName) + 1;
    newresource->strName = new TCHAR[length];
    if (newresource->strName == NULL)
    {
        return E_OUTOFMEMORY;
    }
    _tcscpy_s(newresource->strName, length, strName);

    // add new resource to array
    m_arrResources.push_back(newresource);
    return S_OK;
}





HRESULT C3DResourceContainer::AddResource(const TCHAR* strName, LPDIRECT3DTEXTURE9 pTexture)
{
    return AddResource(strName, eTYPE_TEXTURE, pTexture);
}

HRESULT C3DResourceContainer::AddResource(const TCHAR* strName, LPDIRECT3DVERTEXBUFFER9 pVB)
{
    return AddResource(strName, eTYPE_VB, pVB);
}

HRESULT C3DResourceContainer::AddResource(const TCHAR* strName, LPDIRECT3DINDEXBUFFER9 pIB)
{
    return AddResource(strName, eTYPE_IB, pIB);
}

HRESULT C3DResourceContainer::AddResource(const TCHAR* strName, LPD3DXMESH pMesh)
{
    return AddResource(strName, eTYPE_MESH, pMesh);
}

HRESULT C3DResourceContainer::AddResource(const TCHAR* strName, const D3DMATERIAL9& material)
{
    // create copy of the material
    D3DMATERIAL9* pMaterial = new D3DMATERIAL9;
    if (pMaterial == NULL)
    {
        return E_OUTOFMEMORY;
    }
    ::memcpy(pMaterial, &material, sizeof(D3DMATERIAL9));
    return AddResource(strName, eTYPE_MATERIAL, pMaterial);
}

HRESULT C3DResourceContainer::AddResource(const TCHAR* strName, const D3DLIGHT9& light)
{
    // create copy of the light
    D3DLIGHT9* pLight = new D3DLIGHT9;
    if (pLight == NULL)
    {
        return E_OUTOFMEMORY;
    }
    ::memcpy(pLight, &light, sizeof(D3DLIGHT9));
    return AddResource(strName, eTYPE_LIGHT, pLight);
}




const C3DResourceContainer::RESOURCE* C3DResourceContainer::FindResource(const TCHAR* strName) const
{
    // find any resource with given name
    DWORD i;
    for (i=0; i<m_arrResources.size(); i++)
    {
        RESOURCE* pResource = m_arrResources[i];
        if (_tcscmp(pResource->strName, strName) == 0)
        {
            return pResource;
        }
    }

    return NULL;
}


LPDIRECT3DTEXTURE9 C3DResourceContainer::FindTexture(const TCHAR* strName) const
{
    const RESOURCE* pResource = FindResource(strName);
    if (pResource)
    {
        if (pResource->eType == eTYPE_TEXTURE)
        {
            return (LPDIRECT3DTEXTURE9)pResource->pData;
        }
    }
    return NULL;
}


LPDIRECT3DVERTEXBUFFER9 C3DResourceContainer::FindVB(const TCHAR* strName) const
{
    const RESOURCE* pResource = FindResource(strName);
    if (pResource)
    {
        if (pResource->eType == eTYPE_VB)
        {
            return (LPDIRECT3DVERTEXBUFFER9)pResource->pData;
        }
    }
    return NULL;
}

LPDIRECT3DINDEXBUFFER9 C3DResourceContainer::FindIB(const TCHAR* strName) const
{
    const RESOURCE* pResource = FindResource(strName);
    if (pResource)
    {
        if (pResource->eType == eTYPE_IB)
        {
            return (LPDIRECT3DINDEXBUFFER9)pResource->pData;
        }
    }
    return NULL;
}

LPD3DXMESH C3DResourceContainer::FindMesh(const TCHAR* strName) const
{
    const RESOURCE* pResource = FindResource(strName);
    if (pResource)
    {
        if (pResource->eType == eTYPE_MESH)
        {
            return (LPD3DXMESH)pResource->pData;
        }
    }
    return NULL;
}

D3DMATERIAL9* C3DResourceContainer::FindMaterial(const TCHAR* strName) const
{
    const RESOURCE* pResource = FindResource(strName);
    if (pResource)
    {
        if (pResource->eType == eTYPE_MATERIAL)
        {
            return (D3DMATERIAL9*)pResource->pData;
        }
    }
    return NULL;
}


D3DLIGHT9* C3DResourceContainer::FindLight(const TCHAR* strName) const
{
    const RESOURCE* pResource = FindResource(strName);
    if (pResource)
    {
        if (pResource->eType == eTYPE_LIGHT)
        {
            return (D3DLIGHT9*)pResource->pData;
        }
    }
    return NULL;
}


