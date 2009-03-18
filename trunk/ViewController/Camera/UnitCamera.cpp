/**
 *
 * TODO: do not update camera on every frame, check for changes
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "UnitCamera.h"

UnitCamera::UnitCamera() : Camera()
{
    m_pUIUnit = NULL;

    m_ModZ = 0;
    m_ModX = 0;
    m_ModY = 0;

    reset();
}


void UnitCamera::reset()
{
    m_HeightMod = 2.0f;
}


void UnitCamera::attach(UIUnit* unit)
{
    detach();
    reset();
    m_pUIUnit = unit;
    m_pUIUnit->AddChild(this);
    m_pUIUnit->getUnit()->registerListener(this);
}


void UnitCamera::detach()
{
    if(m_pUIUnit)
    {
        m_pUIUnit->getUnit()->unregisterListener(this);
        m_pUIUnit->RemoveChild(this);
        m_pUIUnit = NULL;
    }
}


UnitCamera::~UnitCamera()
{
    detach();
}


void UnitCamera::update()
{
    if (pDevice && m_pUIUnit)
    {

        D3DXMATRIX& umat = m_pUIUnit->GetMatrix();

        D3DXMatrixLookAtLH( &m_View,
                            &D3DXVECTOR3(umat._41,              umat._42,               umat._43 - m_HeightMod),
                            &D3DXVECTOR3(umat._41-(umat._31*5), umat._42-(umat._32)*5,  umat._43-(umat._33)-2.0f),
                            &D3DXVECTOR3(umat._21,              umat._22,               umat._23));

        // call upper-level class to handle setting and rebuilding frustum

        m_NeedsUpdate = true; // TODO: temporary, change..
        Camera::update();
    }
}


bool UnitCamera::Update(float fFrametime)
{
    I3DObject::Update(fFrametime);
    return true;
}


void UnitCamera::Render(LPDIRECT3DDEVICE9 pDevice)
{
}


void UnitCamera::move(const float depth, const float sideways, const float vertical)
{
    m_ModZ += vertical;
    m_ModY += depth;
    m_ModX += sideways;

    m_NeedsUpdate = true;
}

/*
void UnitCamera::rotate(const float yaw, const float pitch)
{
    m_Yaw += yaw;
    m_Pitch += pitch;

    if(m_Pitch < CAM_MINPITCH)
    {
        m_Pitch = CAM_MINPITCH;
    }
    else if(m_Pitch > CAM_MAXPITCH)
    {
        m_Pitch = CAM_MAXPITCH;
    }
    m_NeedsUpdate = true;
}

void UnitCamera::zoom(const float change)
{
    m_Zoom += change;

    if(m_Zoom < CAM_MINDISTANCE)
    {
        m_Zoom = CAM_MINDISTANCE;
    }
    m_NeedsUpdate = true;
}
*/

void UnitCamera::handleAssetStateChange(IAsset* pAsset, IAsset::State newState)
{
    //
}


void UnitCamera::handleAssetReleased(IAsset* pAsset)
{
    m_pUIUnit->RemoveChild(this);
    m_pUIUnit = NULL;
    // TODO: store the unit's last matrix into ours, or delete this camera
}
