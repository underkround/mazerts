#include "SphereCamera.h"
#include "../Terrain/UITerrain.h"
#include "../../Model/Terrain/Terrain.h"

const float SphereCamera::CAM_MINDISTANCE = 10.0f;
const float SphereCamera::CAM_MINPITCH = 0.4f;
const float SphereCamera::CAM_MAXPITCH = 0.47f * D3DX_PI;

SphereCamera::SphereCamera() : Camera()
{
    m_LookAt.x = 100.0f;
    m_LookAt.y = 100.0f;
    m_LookAt.z = 0.0f;

    m_Zoom = 100.0f;
    m_Pitch = 0.9f;
    m_Yaw = 0.5f * D3DX_PI;
}

void SphereCamera::move(const float depth, const float sideways, const float vertical)
{
    float cosYaw = cos(m_Yaw);
    float sinYaw = sin(m_Yaw);

    m_LookAt.x += cosYaw * depth + sinYaw * sideways;
    m_LookAt.y += sin(m_Yaw) * depth - cosYaw * sideways;
    m_LookAt.z += vertical;

    /*float cosYaw = cos(m_Yaw);
    float sinYaw = sin(m_Yaw);
    m_Position.x += cosYaw * depth + sinYaw * sideways;
    m_Position.y += sin(m_Yaw) * depth - cosYaw * sideways;    */
    m_NeedsUpdate = true;
}

void SphereCamera::rotate(const float yaw, const float pitch)
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

void SphereCamera::zoom(const float change)
{
    m_Zoom += change;

    if(m_Zoom < CAM_MINDISTANCE)
    {
        m_Zoom = CAM_MINDISTANCE;
    }
    m_NeedsUpdate = true;
}



void SphereCamera::update()
{
    //Camera position
    m_Position.x = m_LookAt.x - m_Zoom * cos(m_Yaw) * cos(m_Pitch);
    m_Position.y = m_LookAt.y + m_Zoom * -sin(m_Yaw) * cos(m_Pitch);
    m_Position.z = m_LookAt.z - m_Zoom * sin(m_Pitch);

    if(m_Position.x >= 0.0f && m_Position.x < Terrain::getInstance()->getSize() && m_Position.y >= 0.0f && m_Position.y < Terrain::getInstance()->getSize())
    {
        float height = UITerrain::getInstance()->calculateTriangleHeightAt(m_Position.x, m_Position.y);
        if(m_Position.z > height - 5.0f)
        {
            m_Position.z = height - 5.0f;
        }
    }

    //Build view matrix
    D3DXMatrixLookAtLH( &m_View,                        
                        &m_Position,
                        &m_LookAt,
                        &D3DXVECTOR3(0.0f, 0.0f, -1.0f));  
    
    //Call upper-level class to handle setting and rebuilding frustum
    Camera::update();
}
