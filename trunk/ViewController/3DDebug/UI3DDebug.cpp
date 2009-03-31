#include "UI3DDebug.h"
#include <d3dx9.h>
#include "C3DObjectDebug.h"
#include "../Asset/UI3DObjectManager.h"

LPDIRECT3DDEVICE9 UI3DDebug::pDevice = NULL;

C3DObjectDebug* UI3DDebug::addSphere(float x, float y, float z, float radius, float lifeTime)
{
    LPD3DXMESH pMesh = NULL;
    
    HRESULT hres = D3DXCreateSphere(pDevice, radius, 16, 16, &pMesh, NULL);

    if(FAILED(hres))
    {
        return NULL;
    }

    C3DObjectDebug* pObject = NULL;
    
    if(lifeTime == 0.0f)
    {
        pObject = new C3DObjectDebug();
    }
    else
    {
        pObject = new C3DObjectDebug(lifeTime);
    }

    pObject->Create(pMesh);
    pObject->GetMatrix()._41 = x;
    pObject->GetMatrix()._42 = y;
    pObject->GetMatrix()._43 = z;

    addObject(pObject);

    return pObject;
}


C3DObjectDebug* UI3DDebug::addLine(float startX, float startY, float startZ, 
                              float endX, float endY, float endZ, float radius,
                              float lifeTime)
{
    LPD3DXMESH pMesh = NULL;
        
    const float squared = (endX - startX) * (endX - startX) +
                          (endY - startY) * (endY - startY) +
                          (endZ - startZ) * (endZ - startZ);
    const float len = sqrt(squared);
    HRESULT hres = D3DXCreateCylinder(pDevice, radius,
                            radius,
                            len,
                            6,
                            1,
                            &pMesh,
                            NULL);
    if (FAILED(hres))
    {
        return NULL;
    }

    DWORD i;
    BYTE* pVertices = NULL;
    pMesh->LockVertexBuffer(0, (void**)&pVertices);
    DWORD dwStride = pMesh->GetNumBytesPerVertex();

    for (i=0; i<pMesh->GetNumVertices(); i++)
    {
        //Get vertex position
        D3DXVECTOR3* pVertexPosition = (D3DXVECTOR3*)pVertices;

        //Move all vertices forward by half
        pVertexPosition->z += len * 0.5f;
        
        // move vertex pointer to next vertex
        pVertices += dwStride;
    }

    pMesh->UnlockVertexBuffer();


    C3DObjectDebug* pObject = NULL;
    
    if(lifeTime == 0.0f)
    {
        pObject = new C3DObjectDebug();
    }
    else
    {
           pObject = new C3DObjectDebug(lifeTime);
    }

    pObject->Create(pMesh);

    D3DXVECTOR3 forward(endX - startX, endY - startY, endZ - startZ);
    D3DXVec3Normalize(&forward, &forward);
    D3DXVECTOR3 up(forward.z, forward.x, -forward.y);
    D3DXVECTOR3 right;
    D3DXVec3Cross(&right, &up, &forward);
    D3DXVec3Cross(&up, &right, &forward);

    (*(D3DXVECTOR3*)&pObject->GetMatrix()._11) = right;
    (*(D3DXVECTOR3*)&pObject->GetMatrix()._21) = up;
    (*(D3DXVECTOR3*)&pObject->GetMatrix()._31) = forward;

    pObject->GetMatrix()._41 = startX;
    pObject->GetMatrix()._42 = startY;
    pObject->GetMatrix()._43 = startZ;

    addObject(pObject);
    
    return pObject;
}

void UI3DDebug::addObject(C3DObjectDebug* pObject)
{
    D3DMATERIAL9* pMaterial = new D3DMATERIAL9();
    pMaterial->Emissive = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

    C3DObject::MESHDATA data;
    data.pMaterial = pMaterial;
    data.pTexture = NULL;
    pObject->AddMeshData(data);

    UI3DObjectManager::getInstance()->getRootObject()->AddChild(pObject);
}