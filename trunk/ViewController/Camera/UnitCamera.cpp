/**
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
    m_mLocal._41 = 0.0f;
    m_mLocal._42 = 2.0f;
    m_mLocal._43 = -5.0f;
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
//    if(m_pUnit)
//        m_pUnit->getUnit()->unregisterListener(this);
}


void UnitCamera::update()
{
    /*
    pDevice->SetTransform(D3DTS_VIEW, &m_View);

    D3DXMATRIX proj;
    pDevice->GetTransform(D3DTS_PROJECTION, &proj);
    
    //TEST
    if(m_Test)
    {
        FrustumCull::rebuildFrustum(&m_View, &proj);
    }
    */
    if (pDevice)
    {
        D3DXMATRIX mInverted;
        D3DXMatrixInverse(  &mInverted,
                            NULL,
                            &m_mWorld);
        pDevice->SetTransform(D3DTS_VIEW, &mInverted);
    }
}


bool UnitCamera::Update(float fFrametime)
{
    I3DObject::Update(fFrametime);
/*    if (pDevice)
    {
        D3DXMATRIX mInverted;
        D3DXMatrixInverse(  &mInverted,
                            NULL,
                            &m_mWorld);
        pDevice->SetTransform(D3DTS_VIEW, &mInverted);
    }*/
    return true;
}


void UnitCamera::Render(LPDIRECT3DDEVICE9 pDevice)
{
}


void UnitCamera::move(const float depth, const float sideways, const float vertical)
{
/*
    float cosYaw = cos(m_Yaw);
    float sinYaw = sin(m_Yaw);

    m_LookAt.x += cosYaw * depth + sinYaw * sideways;
    m_LookAt.y += sin(m_Yaw) * depth - cosYaw * sideways;
    m_LookAt.z += vertical;

    /*float cosYaw = cos(m_Yaw);
    float sinYaw = sin(m_Yaw);
    m_Position.x += cosYaw * depth + sinYaw * sideways;
    m_Position.y += sin(m_Yaw) * depth - cosYaw * sideways;*/
/*
    m_NeedsUpdate = true;

    m_ModZ += vertical;
    m_ModY += depth;
    m_ModX += sideways;
*/
    m_mLocal._41 += depth;
    m_mLocal._42 += sideways;
    m_mLocal._43 += vertical;
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

#include "FrustumCull.h"
void UnitCamera::update()
{
    //D3DXMATRIX& umat = m_pUnit->GetWorldMatrix();
    D3DXMATRIX* umat = &m_pUnit->GetWorldMatrix();

//    m_Position.x = m_LookAt.x - m_Zoom * cos(m_Yaw) * cos(m_Pitch);
//    m_Position.y = m_LookAt.y + m_Zoom * -sin(m_Yaw) * cos(m_Pitch);
//    m_Position.z = m_LookAt.z - m_Zoom * sin(m_Pitch);

//    D3DXMATRIX* mat = &m_pUnit->GetWorldMatrix();
//    D3DXVECTOR3* pos    = new D3DXVECTOR3(mat->_41, mat->_42, mat->_43 - 1.0f);
/*
    m_Position.x = umat->_41;
    m_Position.y = umat->_42;
    m_Position.z = umat->_43 - 5.0f;

    D3DXVECTOR3* lookat = new D3DXVECTOR3(1 + m_ModX, 0 + m_ModY, m_Position.z + m_ModZ);

    D3DXVec3TransformNormal(lookat, lookat, umat);

    D3DXMatrixLookAtLH( &m_View,
                        &m_Position,
                        lookat,
                        &D3DXVECTOR3(0, 0, -1));

    // call upper-level class to handle setting and rebuilding frustum

    m_NeedsUpdate = true; // TODO: temporary, change..
    Camera::update();
*/
/*
        pDevice->SetTransform(D3DTS_VIEW, umat);

        D3DXMATRIX proj;
        pDevice->GetTransform(D3DTS_PROJECTION, &proj);
        
        //TEST
        if(m_Test)
        {
            FrustumCull::rebuildFrustum(&m_View, &proj);
        }
}
*/

void UnitCamera::handleAssetStateChange(IAsset* pAsset, IAsset::State newState)
{
    //
}


void UnitCamera::handleAssetReleased(IAsset* pAsset)
{
    detach();
    // TODO: store the unit's last matrix into ours, or delete this camera
}
