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

//for minimap clicks
#include "../Terrain/UITerrain.h"

CameraController::CameraController()
{
    // Default settings
    m_KeyCameraPanUp    = 200;
    m_KeyCameraPanDown  = 208;
    m_KeyCameraPanRight = 205;
    m_KeyCameraPanLeft  = 203;
    m_KeyCameraZoomIn   = 30;
    m_KeyCameraZoomOut  = 44;
    m_KeyCameraReset    = 14;

    m_ModifyMouseDragX      = 10;
    m_ModifyMouseDragY      = 10;
    m_ModifyMouseRotationX  = -10;
    m_ModifyMouseRotationY  = 10;
    m_ModifyMouseZoom       = 10;

    m_KeyMousePanningButton = 1;
    m_KeyPanningModifier    = 0;
    m_KeyMouseRotateButton  = 2;
    m_KeyRotateModifier     = 0;
    m_KeyMouseMinimapButton = 0;
}


CameraController::~CameraController()
{
}


void CameraController::loadConfiguration(const bool confFileLoaded)
{
    Config & c = * Config::getInstance();
    if(!confFileLoaded)
    {
        c.setFilename("controls.ini");
        c.readFile();
    }

    c.updateInt("camera pan up",    m_KeyCameraPanUp);
    c.updateInt("camera pan down",  m_KeyCameraPanDown);
    c.updateInt("camera pan right", m_KeyCameraPanRight);
    c.updateInt("camera pan left",  m_KeyCameraPanLeft);
    c.updateInt("camera zoom in",   m_KeyCameraZoomIn);
    c.updateInt("camera zoom out",  m_KeyCameraZoomOut);
    c.updateInt("camera reset",     m_KeyCameraReset);

    //please note the global MOUSE_CAMSPEED modifier!
    m_ModifyMouseDragX      = c.getValueAsInt("modify mouse panning horizontal",    (int)m_ModifyMouseDragX) * MOUSE_CAMSPEED;
    m_ModifyMouseDragY      = c.getValueAsInt("modify mouse panning vertical",      (int)m_ModifyMouseDragY) * MOUSE_CAMSPEED;
    m_ModifyMouseRotationX  = c.getValueAsInt("modify mouse rotation horizontal",   (int)m_ModifyMouseRotationX) * MOUSE_CAMSPEED;
    m_ModifyMouseRotationY  = c.getValueAsInt("modify mouse rotation vertical",     (int)m_ModifyMouseRotationY) * MOUSE_CAMSPEED;
    m_ModifyMouseZoom       = c.getValueAsInt("modify mouse zoom",                  (int)m_ModifyMouseZoom) * MOUSE_CAMSPEED;

    c.updateInt("mouse panning button",         m_KeyMousePanningButton);
    c.updateInt("mouse panning modifier key",   m_KeyPanningModifier);
    c.updateInt("mouse rotate button",          m_KeyMouseRotateButton);
    c.updateInt("mouse rotate modifier key",    m_KeyRotateModifier);
    c.updateInt("mouse pick button",            m_KeyMouseMinimapButton);
}


void CameraController::updateControls(const float frameTime)
{
    // TODO: Move camera-handling from Camera::current to Camera::getCurrent()
    // if there is no real overhead from the getter..

    // ===== Keyboard
    Camera* camera = Camera::getCurrent();

    if (KeyboardState::keyDown[m_KeyCameraPanUp])
    {
        camera->move(KEYBOARD_CAMSPEED * frameTime, 0, 0);
    }
    else if (KeyboardState::keyDown[m_KeyCameraPanDown])
    {
        camera->move(-KEYBOARD_CAMSPEED * frameTime, 0, 0);
    }
    
    if (KeyboardState::keyDown[m_KeyCameraPanRight])
    {
        camera->move(0, KEYBOARD_CAMSPEED * frameTime, 0);
    }
    else if (KeyboardState::keyDown[m_KeyCameraPanLeft])
    {
        camera->move(0, -KEYBOARD_CAMSPEED * frameTime, 0);
    }
    
    if (KeyboardState::keyDown[m_KeyCameraZoomIn])
    {
        camera->move(0, 0, KEYBOARD_CAMSPEED * frameTime);
    }
    else if (KeyboardState::keyDown[m_KeyCameraZoomOut])
    {
        camera->move(0, 0, -KEYBOARD_CAMSPEED * frameTime);
    }

    //Reset camera
    if(KeyboardState::keyDown[m_KeyCameraReset])
    {
        camera->setZoom(100.0f);
        camera->setRotation(0.5f * D3DX_PI, 0.9f);
    }

    // ===== Mouse

    //move camera by minimap clicks
    if(MouseState::mouseButtonReleased[m_KeyMouseMinimapButton])
    {
        UITerrain::getInstance()->getMiniMap()->moveCameraOnClick();
    }


    //mouse zoom
    if(MouseState::mouseZSpeed)
    {
        camera->zoom(-MouseState::mouseZSpeed * m_ModifyMouseZoom);
    }

    //camera mouse panning
    if(MouseState::mouseButton[m_KeyMousePanningButton] && (!m_KeyPanningModifier || KeyboardState::keyDown[m_KeyPanningModifier]))
    {
        camera->move(MouseState::mouseYSpeed * m_ModifyMouseDragY, -MouseState::mouseXSpeed * m_ModifyMouseDragX, 0);
    }

    //Camera pitching
    if(MouseState::mouseButton[m_KeyMouseRotateButton] && (!m_KeyRotateModifier || KeyboardState::keyDown[m_KeyRotateModifier]))
    {
        camera->rotate(MouseState::mouseXSpeed * 0.05f * m_ModifyMouseRotationX, MouseState::mouseYSpeed * 0.01f * m_ModifyMouseRotationY);
    }

    if(KeyboardState::keyReleased[28])
    {
        camera->m_Test = !camera->m_Test;
    }

}
