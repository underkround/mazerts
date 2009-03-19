#include "Camera.h"
#include "FrustumCull.h"

#include "SphereCamera.h"

LPDIRECT3DDEVICE9 Camera::pDevice = NULL;
Camera* Camera::current = NULL;
SphereCamera Camera::m_DefaultCamera;
DoubleLinkedList<Camera*> Camera::m_Cameras;

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
    if(!current)
        current = &m_DefaultCamera;
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
        
        //TEST
        if(m_Test)
        {
            FrustumCull::rebuildFrustum(&m_View, &proj);
        }

        m_NeedsUpdate = false;
    }
}

// ===== Static camera -stuff

/*
 * Disabled, since it's crusial to have working default camera, so now
 * there is SphereCamera as automatic default camera
void Camera::setDefault(Camera* camera)
{
    if(m_DefaultCamera)
        delete m_DefaultCamera;
    m_DefaultCamera = camera;
    if(m_Cameras.empty())
        current = camera;
    current->forceUpdate();
}
*/

void Camera::pushTop(Camera* camera)
{
    if(!camera)
        return;
    current = camera;
    m_Cameras.pushHead(camera);
    current->forceUpdate();
}

void Camera::pushBack(Camera* camera)
{
    if(!camera)
        return;
    if(m_Cameras.empty())
        current = camera;
    m_Cameras.pushTail(camera);
    current->forceUpdate();
}

bool Camera::pop(Camera* camera)
{
    if(m_Cameras.remove(camera))
    {
        if(camera == current)
        {
            if(m_Cameras.empty())
                current = &m_DefaultCamera;
            else
                current = m_Cameras.peekHead();
            current->forceUpdate();
        }
        return true;
    }
    return false; // TODO
}

Camera* Camera::popTop()
{
    if(m_Cameras.empty())
        return NULL;
    Camera* cam = m_Cameras.popHead();
    current = (m_Cameras.empty()) ? &m_DefaultCamera : m_Cameras.peekHead();
    current->forceUpdate();
    return cam;
}

Camera* Camera::popBack()
{
    if(m_Cameras.empty())
        return NULL;
    Camera* cam = m_Cameras.popTail();
    if(m_Cameras.empty())
        current = &m_DefaultCamera;
    current->forceUpdate();
    return cam;
}

/*
 * Removed as unsave, not all cameras can safely be
 * deleted - make sure to remove cameras that you set
 *
bool Camera::releaseTop()
{
    if(m_Cameras.empty()) return false;
    delete m_Cameras.popHead();
    current = (m_Cameras.empty()) ? &m_DefaultCamera : m_Cameras.peekHead();
    current->forceUpdate();
    return true;
}

bool Camera::releaseBack()
{
    if(m_Cameras.empty()) return false;
    delete m_Cameras.popTail();
    current = (m_Cameras.empty()) ? &m_DefaultCamera : m_Cameras.peekHead();
    current->forceUpdate();
    return true;
}
*/
