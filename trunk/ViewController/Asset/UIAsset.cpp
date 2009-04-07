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
    //TODO: if needed
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
