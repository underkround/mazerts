#include "UIUnit.h"
#include "UI3DObjectManager.h"
#include "../Terrain/UITerrain.h"
#include "../App/I3DObject.h"
#include "../Camera/FrustumCull.h"
#include "../../Model/Common/Vector3.h"

#include "../3DDebug/UI3DDebug.h"

bool UIUnit::Update(float fFrameTime) 
{
    I3DObject::Update(fFrameTime);
    return m_Alive;
}

void UIUnit::updatePosition()
{

    //TODO: Unit-flags:  ground / air (/ water?)
    //if(m_pUnit->getMoveType() == UNIT_GROUND)
    {
        //Get heights at four points around the unit
        Vector3* unitPos = m_pUnit->getPosition();
        Vector3* unitDir = m_pUnit->getDirection();
        
        float sizeOffset = m_pUnit->getWidth() * 0.5f;
        float xOffset = unitDir->x;
        float yOffset = unitDir->y;

        Vector3 pos;
        pos.x = unitPos->x + sizeOffset;
        pos.y = unitPos->y + sizeOffset;

        UITerrain* pTerrain = UITerrain::getInstance();

        
        D3DXVECTOR3 normal(0, 0, 0);
        D3DXVECTOR3 right;
        D3DXVECTOR3 left;
        D3DXVECTOR3 up;
        D3DXVECTOR3 down;
        D3DXVECTOR3 result1;
        D3DXVECTOR3 result2;

        right.x = yOffset - xOffset;
        right.y = -yOffset - xOffset;

        left.x = -yOffset + xOffset;
        left.y = yOffset + xOffset;
                
        up.x = right.y;
        up.y = -right.x;

        down.x = left.y;
        down.y = -left.x;

        //TODO: viimeistele
        //Right
        right.z = pTerrain->calculateTriangleHeightAt(pos.x + right.x, pos.y + right.y);
        //Left
        left.z = pTerrain->calculateTriangleHeightAt(pos.x + left.x, pos.y + left.y);
        //Up
        up.z = pTerrain->calculateTriangleHeightAt(pos.x + right.y, pos.y - right.x);
        //Down
        down.z = pTerrain->calculateTriangleHeightAt(pos.x + left.y, pos.y - left.x);

        UI3DDebug::addSphere(pos.x + right.x, pos.y + right.y, right.z, 0.1f, 0.4f);
        UI3DDebug::addSphere(pos.x + left.x, pos.y + left.y, left.z, 0.1f, 0.4f);
        UI3DDebug::addSphere(pos.x + right.y, pos.y - right.x, right.z, 0.1f, 0.4f);
        UI3DDebug::addSphere(pos.x + left.y, pos.y - left.x, left.z, 0.1f, 0.4f);

        D3DXVec3Subtract(&result1, &right, &up);
        D3DXVec3Subtract(&result2, &down, &up);
        //Re-use right as temporary result
        D3DXVec3Cross(&right, &result2, &result1);

        normal += right;

        D3DXVec3Subtract(&result1, &left, &down);
        D3DXVec3Subtract(&result2, &up, &down);
        //Re-use right as temporary result
        D3DXVec3Cross(&right, &result2, &result1);
        
        normal += right;
        D3DXVec3Normalize(&normal, &normal);

        //Terrain normal
        /*D3DXVECTOR3 normal = UITerrain::getInstance()->getNormalAt(
                                        m_pUnit->getPosition()->x,
                                        m_pUnit->getPosition()->y,
                                        m_pUnit->getWidth(),
                                        m_pUnit->getWidth()
                                        );*/

        D3DXVECTOR3* dir = (D3DXVECTOR3*)m_pUnit->getDirection();

        //Get "right"-vector from crossproduct of direction (around z-axis) and normal    
        //D3DXVECTOR3 right;
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
    }

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
        //Position is updated here so frustum culling won't bug
        m_mLocal._41 = m_pUnit->getPosition()->x + m_HalfSize;
        m_mLocal._42 = m_pUnit->getPosition()->y + m_HalfSize;
        m_mLocal._43 = UITerrain::getInstance()->calculateTriangleHeightAt(m_mLocal._41, m_mLocal._42);

        //Frustum culling
        D3DXVECTOR3 AABBMin(m_AABBMin.x + m_mWorld._41,
                            m_AABBMin.y + m_mWorld._42,
                            m_AABBMin.z + m_mWorld._43);

        D3DXVECTOR3 AABBMax(m_AABBMax.x + m_mWorld._41,
                            m_AABBMax.y + m_mWorld._42,
                            m_AABBMax.z + m_mWorld._43);
        
        if(FrustumCull::cullAABB(AABBMin, AABBMax))
        {
            updatePosition();

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