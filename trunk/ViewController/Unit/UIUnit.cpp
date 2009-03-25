#include "UIUnit.h"
#include "UI3DObjectManager.h"
#include "../Terrain/UITerrain.h"
#include "../App/I3DObject.h"
#include "../Camera/FrustumCull.h"
#include "../../Model/Common/Vector3.h"

#include "../Sound/SoundManager.h"

#include "../3DDebug/UI3DDebug.h"

bool UIUnit::Update(float fFrameTime) 
{
    I3DObject::Update(fFrameTime);
    return m_Alive;
}

void UIUnit::updatePosition()
{
    UITerrain* pTerrain = UITerrain::getInstance();
    
    D3DXVECTOR3* unitPos = (D3DXVECTOR3*)m_pUnit->getPosition();
    
    //TODO: Unit-flags:  ground / air (/ water?)
    //if(m_pUnit->getMoveType() == UNIT_GROUND)
    {
        //Check if unit has moved or terrain has changed
        if(m_UITerrain_ChangeCounter != pTerrain->getChangeCounter() || unitPos->x != m_OldPosition.x || 
            unitPos->y != m_OldPosition.y || unitPos->z != m_OldPosition.z)
        {
            //Store old position and terrain change counter
            m_UITerrain_ChangeCounter = pTerrain->getChangeCounter();
            m_OldPosition.x = unitPos->x;
            m_OldPosition.y = unitPos->y;
            m_OldPosition.z = unitPos->z;

            //Get heights at four points around the unit
            D3DXVECTOR3 pos;
            pos.x = unitPos->x + m_HalfSize;
            pos.y = unitPos->y + m_HalfSize;

            D3DXVECTOR3* dir = (D3DXVECTOR3*)m_pUnit->getDirection();
            D3DXVec3Normalize(dir, dir);

            D3DXVECTOR3 normal(0, 0, 0);
            D3DXVECTOR3 p1;
            D3DXVECTOR3 p2;
            D3DXVECTOR3 p3;
            D3DXVECTOR3 p4;
            D3DXVECTOR3 result1;
            D3DXVECTOR3 result2;

            p1.x = dir->y - dir->x;
            p1.y = -dir->y - dir->x;

            p2.x = -dir->y + dir->x;
            p2.y = dir->y + dir->x;

            p3.x = p1.y;
            p3.y = -p1.x;

            p4.x = p2.y;
            p4.y = -p2.x;

            //Forward right
            p1.z = pTerrain->calculateTriangleHeightAt(pos.x + p1.x, pos.y + p1.y);
            //Forward left
            p2.z = pTerrain->calculateTriangleHeightAt(pos.x + p2.x, pos.y + p2.y);
            //backward right
            p3.z = pTerrain->calculateTriangleHeightAt(pos.x + p1.y, pos.y - p1.x);
            //backward left
            p4.z = pTerrain->calculateTriangleHeightAt(pos.x + p2.y, pos.y - p2.x);

            //Debug-spheres to show where the height values are taken
            //UI3DDebug::addSphere(pos.x + p1.x, pos.y + p1.y, p1.z, 0.1f, 0.4f);
            //UI3DDebug::addSphere(pos.x + p2.x, pos.y + p2.y, p2.z, 0.1f, 0.4f);
            //UI3DDebug::addSphere(pos.x + p1.y, pos.y - p1.x, p1.z, 0.1f, 0.4f);
            //UI3DDebug::addSphere(pos.x + p2.y, pos.y - p2.x, p2.z, 0.1f, 0.4f);

            //Find two normals of points
            D3DXVec3Subtract(&result1, &p1, &p3);
            D3DXVec3Subtract(&result2, &p4, &p3);
            D3DXVec3Cross(&result1, &result2, &result1);

            normal += result1;

            D3DXVec3Subtract(&result1, &p2, &p4);
            D3DXVec3Subtract(&result2, &p3, &p4);
            D3DXVec3Cross(&result1, &result2, &result1);

            normal += result1;

            D3DXVec3Normalize(&normal, &normal);

            //Update unit matrix
            //Get "right"-vector from crossproduct of direction (around z-axis) and normal, reuse p1 as result
            D3DXVec3Cross(&p1, &normal, dir);

            //Find new "forward"-vector based on normal and right vector (fixes the z-value), reuse p2 as result
            D3DXVec3Cross(&p2, &normal, &p1);

            D3DXMatrixIdentity(&m_mLocal);

            m_mLocal._11 = p1.x;
            m_mLocal._21 = normal.x;
            m_mLocal._31 = p2.x;

            m_mLocal._12 = p1.y;
            m_mLocal._22 = normal.y;
            m_mLocal._32 = p2.y;

            m_mLocal._13 = p1.z;
            m_mLocal._23 = normal.z;
            m_mLocal._33 = p2.z;
        
            m_mLocal._41 = m_pUnit->getPosition()->x + m_HalfSize;
            m_mLocal._42 = m_pUnit->getPosition()->y + m_HalfSize;
            m_mLocal._43 = UITerrain::getInstance()->calculateTriangleHeightAt(m_mLocal._41, m_mLocal._42);
        }
    }
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


void UIUnit::setSelected(bool value)
{
    // remove old marker object, if any
    if(m_SelectionMarker)
    {
        m_SelectionMarker->selfDestruct();
        m_SelectionMarker = NULL;
    }
    m_Selected = value;
    // select-action
    if(value)
    {
        m_SelectionMarker = UI3DDebug::addSphere(0, 2.0f, 0.0f, 1.0f);
        //Debug-object, added automatically to root
        UI3DObjectManager::getInstance()->getRootObject()->RemoveChild((C3DObject*)m_SelectionMarker);
        this->AddChild((C3DObject*)m_SelectionMarker);

        // play sound for selection
        SoundManager::playSound(SOUND_READY, 0.1f, (D3DXVECTOR3)&GetMatrix()._41);
    }
}
