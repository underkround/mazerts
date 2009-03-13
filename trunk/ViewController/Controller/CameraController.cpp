/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#include "CameraController.h"
#include "../Input/KeyboardState.h"
#include "../Input/MouseState.h"

#include "../../Model/Common/Config.h"

CameraController::CameraController(Camera* camera)
{
    m_pCamera = camera;
}


CameraController::~CameraController()
{
}


void CameraController::loadConfiguration()
{
    Config & c = * Config::getInstance();
    // Assume that the caller of this method (GameState) has loaded the file
    //c.setFilename("controls.ini");
    //c.readFile();
    m_KeyCameraPanUp = c.getValueAsInt("camera pan up");
    m_KeyCameraPanDown = c.getValueAsInt("camera pan down");
    m_KeyCameraPanRight = c.getValueAsInt("camera pan right");
    m_KeyCameraPanLeft = c.getValueAsInt("camera pan left");
    m_KeyCameraZoomIn = c.getValueAsInt("camera zoom in");
    m_KeyCameraZoomOut = c.getValueAsInt("camera zoom out");
    m_KeyCameraReset = c.getValueAsInt("camera reset");

    //please note the global MOUSE_CAMSPEED modifier!
    m_ModifyMouseDragX = c.getValueAsInt("modify mouse panning horizontal") * MOUSE_CAMSPEED;
    m_ModifyMouseDragY = c.getValueAsInt("modify mouse panning vertical") * MOUSE_CAMSPEED;
    m_ModifyMouseRotationX = c.getValueAsInt("modify mouse rotation horizontal") * MOUSE_CAMSPEED;
    m_ModifyMouseRotationY = c.getValueAsInt("modify mouse rotation vertical") * MOUSE_CAMSPEED;
    m_ModifyMouseZoom = c.getValueAsInt("modify mouse zoom") * MOUSE_CAMSPEED;

    m_KeyMouseDragButton = c.getValueAsInt("mouse drag button");
    m_KeyMousePickButton = c.getValueAsInt("mouse pick button");
    m_KeyMouseRotateButton = c.getValueAsInt("mouse rotate button");
}


void CameraController::updateControls(float frameTime)
{

    // ===== Keyboard

    if (KeyboardState::keyDown[m_KeyCameraPanUp])
    {
        m_pCamera->move(KEYBOARD_CAMSPEED * frameTime, 0, 0);
    }
    else if (KeyboardState::keyDown[m_KeyCameraPanDown])
    {
        m_pCamera->move(-KEYBOARD_CAMSPEED * frameTime, 0, 0);
    }
    
    if (KeyboardState::keyDown[m_KeyCameraPanRight])
    {
        m_pCamera->move(0, KEYBOARD_CAMSPEED * frameTime, 0);
    }
    else if (KeyboardState::keyDown[m_KeyCameraPanLeft])
    {
        m_pCamera->move(0, -KEYBOARD_CAMSPEED * frameTime, 0);
    }
    
    if (KeyboardState::keyDown[m_KeyCameraZoomIn])
    {
        m_pCamera->move(0, 0, KEYBOARD_CAMSPEED * frameTime);
    }
    else if (KeyboardState::keyDown[m_KeyCameraZoomOut])
    {
        m_pCamera->move(0, 0, -KEYBOARD_CAMSPEED * frameTime);
    }

    //Reset camera
    if(KeyboardState::keyDown[m_KeyCameraReset])
    {
        m_pCamera->setZoom(100.0f);
        m_pCamera->setRotation(0.5f * D3DX_PI, 0.9f);
    }

    // ===== Mouse

    //mouse zoom
    if(MouseState::mouseZSpeed)
    {
        m_pCamera->zoom(-MouseState::mouseZSpeed * m_ModifyMouseZoom);
    }

    //camera mouse panning
    if(MouseState::mouseButton[m_KeyMouseDragButton])
    {
        m_pCamera->move(MouseState::mouseYSpeed * m_ModifyMouseDragY, -MouseState::mouseXSpeed * m_ModifyMouseDragX, 0);
    }

    //Camera pitching
    if(MouseState::mouseButton[m_KeyMouseRotateButton])
    {
        m_pCamera->rotate(MouseState::mouseXSpeed * 0.05f * m_ModifyMouseRotationX, MouseState::mouseYSpeed * 0.01f * m_ModifyMouseRotationY);
    }

    if(KeyboardState::keyReleased[28])
    {
        m_pCamera->m_Test = !m_pCamera->m_Test;
    }

}
