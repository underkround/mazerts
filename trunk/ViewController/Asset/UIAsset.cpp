#include "UIAsset.h"
#include "UI3DObjectManager.h"
#include "../App/I3DObject.h"
#include "../Camera/FrustumCull.h"
#include "../../Model/Common/Vector3.h"
#include "../Terrain/UITerrain.h"

#include "../Sound/SoundManager.h"

#include "../3DDebug/UI3DDebug.h"

bool UIAsset::Update(float fFrameTime) 
{
    if(m_Alive)
    {
        I3DObject::Update(fFrameTime);

        switch(m_pAsset->getState())
        {
            case IAsset::STATE_ACTIVE:
                break;
            
            case IAsset::STATE_BEING_BUILT:
                //Building progress
                m_mLocal._23 = -(m_pAsset->getHitpoints() / (float)m_pAsset->getHitpointsMax());
                break;
            
            case IAsset::STATE_DESTROYED:
                break;
            
            case IAsset::STATE_PARALYZED:
                break;
        }

        SetVisible(UITerrain::getInstance()->getCurrentPlayer()->getFog()->isAssetVisible(getAsset()));
    }

    return m_Alive;
}

void UIAsset::handleAssetStateChange(IAsset* pAsset, IAsset::State newState)
{
    if (pAsset->getAssetType() == IAsset::BUILDING)
    {
        if (newState == IAsset::STATE_ACTIVE)
            if (UITerrain::getInstance()->getCurrentPlayer() == pAsset->getOwner())
                SoundManager::playSound(SOUND_CONSTRUCTION_COMPLETE, 0.01f, *(D3DXVECTOR3*)pAsset->getPosition(), false);
    }
}


void UIAsset::setSelected(bool value)
{
    // Obsolete?

    // remove old marker object, if any
    /*
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
    */

    m_Selected = value;

    if(m_pHealthBlock)
    {
        if (value)
        {
            // set health block visible
            m_pHealthBlock->SetVisible(true);
        }
        else
        {
            m_pHealthBlock->SetVisible(false);
        }
    }
}

void UIAsset::Render(LPDIRECT3DDEVICE9 pDevice)
{
    // same as in c3dobject, but doesn't call child objects if they're not visible
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

                    //Set the texture, if it is different than current
                    if(data.pTexture != pCurrentTexture)
                    {
                        pDevice->SetTexture(0, data.pTexture);
                        pCurrentTexture = data.pTexture;
                    }

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
}
