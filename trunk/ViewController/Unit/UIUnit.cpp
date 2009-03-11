#include "UIUnit.h"
#include "UI3DObjectManager.h"
#include "../Terrain/UITerrain.h"
#include "../App/I3DObject.h"
#include "../Camera/FrustumCull.h"

bool UIUnit::Update(float fFrameTime) 
{
    I3DObject::Update(fFrameTime);
    return m_Alive;
}

void UIUnit::alignToTerrain()
{

    //Terrain normal
    D3DXVECTOR3 normal = UITerrain::getInstance()->getNormalAt(
                                    m_pUnit->getPosition()->x,
                                    m_pUnit->getPosition()->y,
                                    m_pUnit->getWidth(),
                                    m_pUnit->getWidth()
                                    );

    D3DXVECTOR3* dir = (D3DXVECTOR3*)m_pUnit->getDirection();

    //Get "right"-vector from crossproduct of direction (around z-axis) and normal    
    D3DXVECTOR3 right;
    D3DXVec3Normalize(dir, dir);
    D3DXVec3Cross(&right, &normal, dir);

    //Find new "forward"-vector based on normal and right vector
    D3DXVECTOR3 forward;
    D3DXVec3Cross(&forward, &normal, &right);
    
    D3DXMatrixIdentity(&m_mLocal);

    m_mLocal._11 = right.x;
    m_mLocal._21 = normal.x;
    m_mLocal._31 = forward.x;

    m_mLocal._12 = right.y;
    m_mLocal._22 = normal.y;
    m_mLocal._32 = forward.y;

    m_mLocal._13 = right.z;
    m_mLocal._23 = normal.z;
    m_mLocal._33 = forward.z;
    
    D3DXMATRIX scale;
    D3DXMatrixIdentity(&scale);
    D3DXMatrixScaling(&scale, UI3DObjectManager::globalScale, UI3DObjectManager::globalScale, UI3DObjectManager::globalScale);
    D3DXMatrixMultiply(&m_mLocal, &m_mLocal, &scale);

    m_mLocal._41 = m_pUnit->getPosition()->x + m_HalfSize;
    m_mLocal._42 = m_pUnit->getPosition()->y + m_HalfSize;
    m_mLocal._43 = UITerrain::getInstance()->calculateTriangleHeightAt(m_mLocal._41, m_mLocal._42);

}

//Overridden here to do alignToTerrain only for visible units,
//otherwise same as in C3DObject
void UIUnit::Render(LPDIRECT3DDEVICE9 pDevice)
{
    if (IsVisible() && m_pMesh)
    {
        //Frustum culling
        D3DXVECTOR3 AABBMin(m_AABBMin.x + m_mWorld._41,
                            m_AABBMin.y + m_mWorld._42,
                            m_AABBMin.z + m_mWorld._43);

        D3DXVECTOR3 AABBMax(m_AABBMax.x + m_mWorld._41,
                            m_AABBMax.y + m_mWorld._42,
                            m_AABBMax.z + m_mWorld._43);
        
        if(FrustumCull::cullAABB(AABBMin, AABBMax))
        {
            alignToTerrain();

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
    }

    // render the children
    ListNode<I3DObject*>* node = m_arrChildren.headNode();    
    
    while(node)
    {
        node->item->Render(pDevice);
        node = node->next;
    }
}



void UIUnit::handleAssetStateChange(IAsset* pAsset, IAsset::State newState)
{
    //TODO: if needed
}