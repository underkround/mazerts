/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __CameraCONTROLLER_H__
#define __CameraCONTROLLER_H__

#include "IUIController.h"
#include "../Camera/Camera.h"

#define KEYBOARD_CAMSPEED 60.0f
#define MOUSE_CAMSPEED 0.01f

class CameraController : public IUIController
{
public:

    CameraController(Camera* camera);

    ~CameraController();

    /**
     * @see IUIController
     */
    void updateControls(float frameTime);

    /**
     * @see IUIController
     */
    virtual void loadConfiguration();

private:

    Camera*       m_pCamera;

    int m_KeyCameraPanUp;
    int m_KeyCameraPanDown;
    int m_KeyCameraPanRight;
    int m_KeyCameraPanLeft;
    int m_KeyCameraZoomIn;
    int m_KeyCameraZoomOut;
    int m_KeyCameraReset;

    int m_KeyMouseDragButton;
    int m_KeyMousePickButton;
    int m_KeyMouseRotateButton;

    float m_ModifyMouseDragX;
    float m_ModifyMouseDragY;
    float m_ModifyMouseRotationX;
    float m_ModifyMouseRotationY;
    float m_ModifyMouseZoom;

};

#endif // __CameraCONTROLLER_H__
