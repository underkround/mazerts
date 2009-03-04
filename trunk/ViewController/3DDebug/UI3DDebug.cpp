#include "UI3DDebug.h"
#include <d3dx9.h>
#include "../App/C3DObject.h"
#include "../Unit/UI3DObjectManager.h"

LPDIRECT3DDEVICE9 UI3DDebug::pDevice = NULL;

C3DObject* UI3DDebug::addSphere(float x, float y, float z, float radius)
{
    LPD3DXMESH pMesh = NULL;
    
    HRESULT hres = D3DXCreateSphere(pDevice, radius, 16, 16, &pMesh, NULL);

    if(FAILED(hres))
    {
        return NULL;
    }

    C3DObject* pObject = new C3DObject();
    pObject->Create(pMesh);
    pObject->GetMatrix()._41 = x;
    pObject->GetMatrix()._42 = y;
    pObject->GetMatrix()._43 = z;
    UI3DObjectManager::getInstance()->getRootObject()->AddChild(pObject);

    return pObject;
}


C3DObject* UI3DDebug::addLine(float startX, float startY, float startZ, 
                              float endX, float endY, float endZ, float radius)
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


    C3DObject* pObject = new C3DObject();
    pObject->Create(pMesh);

    D3DXVECTOR3 forward(endX - startX, endY - startY, endZ - startZ);
    D3DXVec3Normalize(&forward, &forward);
    D3DXVECTOR3 up(forward.z, 0, -forward.x);
    D3DXVECTOR3 right;
    D3DXVec3Cross(&right, &up, &forward);
    D3DXVec3Cross(&up, &right, &forward);

    (*(D3DXVECTOR3*)&pObject->GetMatrix()._11) = right;
    (*(D3DXVECTOR3*)&pObject->GetMatrix()._21) = up;
    (*(D3DXVECTOR3*)&pObject->GetMatrix()._31) = forward;

    pObject->GetMatrix()._41 = startX;
    pObject->GetMatrix()._42 = startY;
    pObject->GetMatrix()._43 = startZ;

    UI3DObjectManager::getInstance()->getRootObject()->AddChild(pObject);

    return pObject;
}