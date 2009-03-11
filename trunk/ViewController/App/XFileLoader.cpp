#include "XFileLoader.h"

CXFileLoader::CXFileLoader(void)
{
}

CXFileLoader::~CXFileLoader(void)
{
}

HRESULT CXFileLoader::Load(    LPCTSTR strFilename,
                               C3DResourceContainer& container,
                               C3DObject* pObject)
{
    DWORD              i;
    HRESULT            hres;
    LPD3DXBUFFER       pMaterialBuffer = NULL;
    DWORD              dwNumMaterials = 0;
    LPD3DXMESH         pMesh = NULL;
    TCHAR              resname[256];


    // load the x-file
    hres = D3DXLoadMeshFromX(    strFilename,
                                D3DXMESH_MANAGED,
                                container.GetDevice(),
                                NULL,
                                &pMaterialBuffer,
                                NULL,
                                &dwNumMaterials,
                                &pMesh);
    if (FAILED(hres))
    {
        // failed to load, maybe file was not found,
        // or invalid type
        return hres;
    }


    // add loaded mesh into the resource container
    if (FAILED(container.AddResource(strFilename, pMesh)))
    {
        // failed to add, maybe it is already there
        pMesh->Release();
        pMesh = container.FindMesh(strFilename);

        if (pMesh == NULL)
        {
            // failed to add mesh to container, but
            // it doesn't have mesh with same name...
            return E_FAIL;
        }
    }


    // init the given 3d object
    if (pObject)
    {
        pObject->Create(pMesh);
    }


    // parse materials and textures
    D3DXMATERIAL* pMaterial =
        (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();

    for (i=0; i<dwNumMaterials; i++)
    {
        // copy material from file object
        D3DMATERIAL9 material = pMaterial[i].MatD3D;

        // set the material ambient. Ambient is not
        // stored in x-files
        material.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);

        // add material to container
        _stprintf_s(resname, _T("%s_material_%02d"), strFilename, i);
        container.AddResource(resname, material);


        // build the 3d object mesh data
        C3DObject::MESHDATA data;
        data.pMaterial = container.FindMaterial(resname);
        data.pTexture = NULL;


        // load the material texture
        LPDIRECT3DTEXTURE9 pTexture = NULL;
        hres = D3DXCreateTextureFromFileA(  container.GetDevice(),
                                            pMaterial[i].pTextureFilename,
                                            &pTexture);
        if (FAILED(hres))
        {
            // failed to load texture
            #ifdef _DEBUG
            if (pMaterial[i].pTextureFilename)
            {
                ::OutputDebugString(_T("Failed to load texture: "));
                ::OutputDebugStringA(pMaterial[i].pTextureFilename);
                ::OutputDebugString(_T("\r\n"));
            }
            #endif
        }
        else
        {
            // handle loaded texture, try to add to container
            
            // convert texture filename to TCHARs
            int j = 0;
            int resnamepos = 0;
            while (pMaterial[i].pTextureFilename[j])
            {
                resname[resnamepos++] =
                    (TCHAR)pMaterial[i].pTextureFilename[j];
                j++;
            }
            resname[resnamepos] = 0;

            if (FAILED(container.AddResource(resname, pTexture)))
            {
                // failed to add texture, maybe it already
                // exists
                pTexture->Release();
                pTexture = container.FindTexture(resname);

                #ifdef _DEBUG
                ::OutputDebugString(_T("CXFileLoader: Skipping texture "));
                ::OutputDebugString(resname);
                ::OutputDebugString(_T("\r\n"));
                #endif
            }

            data.pTexture = pTexture;
        }

        if (pObject)
        {
            pObject->AddMeshData(data);
        }
    }
    
    pMaterialBuffer->Release();
    return S_OK;
}


