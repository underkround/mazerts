#include "UIAsset.h"
#include "UI3DObjectManager.h"
#include "../App/I3DObject.h"
#include "../Camera/FrustumCull.h"
#include "../../Model/Common/Vector3.h"

#include "../Sound/SoundManager.h"

#include "../3DDebug/UI3DDebug.h"

bool UIAsset::Update(float fFrameTime) 
{
    I3DObject::Update(fFrameTime);
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