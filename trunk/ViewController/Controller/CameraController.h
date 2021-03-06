/**
 * Controls the current camera
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __CameraCONTROLLER_H__
#define __CameraCONTROLLER_H__

#include "IUIController.h"
#include "../Camera/Camera.h"
#include "../../Model/Player/Player.h"

#define KEYBOARD_CAMSPEED 60.0f
#define MOUSE_CAMSPEED 0.01f

class CameraController : public IUIController
{
public:

    CameraController();

    ~CameraController();

    /**
     * @see IUIController
     */
    void updateControls(const float frameTime);

    /**
     * @see IUIController
     */
    virtual void loadConfigurations();

    inline void setCurrentPlayer(Player* player) { m_pCurrentPlayer = player; }

private:

    int m_KeyCameraPanUp;
    int m_KeyCameraPanDown;
    int m_KeyCameraPanRight;
    int m_KeyCameraPanLeft;
    int m_KeyCameraZoomIn;
    int m_KeyCameraZoomOut;
    int m_KeyCameraReset;

    int m_KeyMousePanningButton;
    int m_KeyPanningModifier;
    int m_KeyMouseRotateButton;
    int m_KeyRotateModifier;
    int m_KeyMouseMinimapButton;

    float m_ModifyMouseDragX;
    float m_ModifyMouseDragY;
    float m_ModifyMouseRotationX;
    float m_ModifyMouseRotationY;
    float m_ModifyMouseZoom;

    Player* m_pCurrentPlayer;

};

#endif // __CameraCONTROLLER_H__
