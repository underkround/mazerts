#include "Camera.h"
#include "FrustumCull.h"

LPDIRECT3DDEVICE9 Camera::pDevice = NULL;


Camera::Camera()
{
    m_Position.x = 0.0f;
    m_Position.y = 0.0f;
    m_Position.z = 0.0f;
    m_Pitch = 0.0f;
    m_Yaw = 0.0f;
    m_Zoom = 1.0f;

    m_NeedsUpdate = true;
}

void Camera::create(LPDIRECT3DDEVICE9 pDevice)
{
    Camera::pDevice = pDevice;
}

void Camera::move(const float depth, const float sideways, const float vertical)
{
    //TODO:
    m_NeedsUpdate = true;
}

void Camera::rotate(const float yaw, const float pitch)
{
    m_Yaw += yaw;
    m_Pitch += pitch;
    m_NeedsUpdate = true;
}

void Camera::setRotation(const float yaw, const float pitch)
{
    m_Yaw = yaw;
    m_Pitch = pitch;
    m_NeedsUpdate = true;
}


void Camera::update()
{
    if(m_NeedsUpdate)
    {
        pDevice->SetTransform(D3DTS_VIEW, &m_View);

        D3DXMATRIX proj;
        pDevice->GetTransform(D3DTS_PROJECTION, &proj);
        FrustumCull::rebuildFrustum(&m_View, &proj);

        m_NeedsUpdate = false;
    }
}