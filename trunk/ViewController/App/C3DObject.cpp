/**
 * C3DObject.cpp source file
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 19.2.2009
 * 
 * concrete 3d object that is capable of rendering
 * direct3d mesh objects
 */

#include "C3DObject.h"

C3DObject::C3DObject(void)
{
	m_pMesh = NULL;
}


C3DObject::~C3DObject(void)
{
}


void C3DObject::Create(LPD3DXMESH pMesh)
{
	m_pMesh = pMesh;
}


void C3DObject::Release(void)
{
	m_arrMeshData.clear();
	I3DObject::Release();    
	m_pMesh = NULL;
}


void C3DObject::Render(LPDIRECT3DDEVICE9 pDevice)
{
	if (IsVisible() && m_pMesh)
	{
		// first set the world matrix to device
		pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);

		// render the mesh subsets
		const DWORD numsubsets = m_arrMeshData.size();

		if (numsubsets)
		{
			// loop thru materials and render the subsets
			DWORD i;
			for (i=0; i<numsubsets; i++)
			{
				MESHDATA& data = m_arrMeshData[i];

				// set the texture
				pDevice->SetTexture(0, data.pTexture);

				// set material
				if (data.pMaterial)
				{
					pDevice->SetMaterial(data.pMaterial);
				}

				// render the mesh
				m_pMesh->DrawSubset(i);
			}
		}
		else
		{
			// render the mesh anyways
			m_pMesh->DrawSubset(0);
		}
	}

	// render the children
    ListNode<I3DObject*>* node = m_arrChildren.headNode();    
    
    while(node)
    {
        node->item->Render(pDevice);
        node = node->next;
    }
}


void C3DObject::AddMeshData(MESHDATA& meshdata)
{
	m_arrMeshData.push_back(meshdata);
}
