/**
 * Basic Camera-object
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

//TODO: Camera rotation, zoom...

#include "d3dx9.h"

class Camera
{
public:

    Camera();

    virtual ~Camera() {}

    /**
     * Prepares the cameraobject for usage with given device
     * @param pDevice Direct3D-Device to use
     */
    static void create(LPDIRECT3DDEVICE9 pDevice);

    virtual inline void setPosition(const float x, const float y, const float z)
    {
        m_Position.x = x;
        m_Position.y = y;
        m_Position.z = z;
        m_NeedsUpdate = true;
        m_Test = true;
    }
    
    virtual inline void setPosition(const D3DXVECTOR3& pos)
    {
        m_Position = pos;
        m_NeedsUpdate = true;
    }

    /**
     * Moves the camera based on current orientation
     * @param depth Movement in depth (forwards/backwards)
     * @param sideways Sideway movement (left/right)
     * @param vertical Vertical movement (up/down)
     */
    virtual void move(const float depth, const float sideways, const float vertical);

    /**
     * Rotates the camera
     * @param yaw Change in yaw
     * @param pitch Change in pitch
     */
    virtual void rotate(const float yaw, const float pitch);

    /**
     * Sets the rotation for camera
     * @param yaw New yaw
     * @param pitch New pitch
     */
    virtual void setRotation(const float yaw, const float pitch);

    /** 
     * Changes camera zoom
     * @param change Change in zoom-value
     */
    inline virtual void zoom(const float change)
    {
        m_NeedsUpdate = true;
    }

    /**
     * Sets the zoom of the camera
     * @param zoom The new zoom-balue
     */
    inline virtual void setZoom(const float zoom)
    {
        m_Zoom = zoom;
        m_NeedsUpdate = true;
    }

    /**
     * Forces camera update (for device restore, default view matrix
     * will otherwise be used)
     */
    inline void forceUpdate()
    {
        m_NeedsUpdate = true;
        update();
    }

    /**
     * Sets view-matrix and updates frustum-planes
     */
    virtual void update();

    /**
     * Sets the view-matrix to use
     * @param view View-matrix
     */
    inline void setMatrix(const D3DXMATRIX& view)
    {
        m_View = view;
    }

    inline D3DXVECTOR3& getPosition() { return m_Position; }

    /**
     * Returns the current view-matrix
     * @return the view-matrix
     */
    inline D3DXMATRIX& getMatrix()
    {
        return m_View;
    }
    bool m_Test;
protected:

    /**
     * View-matrix built by camera settings
     */
    D3DXMATRIX m_View;

    /**
     * Camera location
     */
    D3DXVECTOR3 m_Position;
    
    /**
     * Rotation
     */
    float m_Yaw;
    float m_Pitch;
    
    /**
     * Zoom factor
     */
    float m_Zoom;
    
    static LPDIRECT3DDEVICE9 pDevice;

    /**
     * Used to control view-matrix and frustum rebuilding
     * (done only when necessary)
     */
    bool m_NeedsUpdate;


};

#endif //__CAMERA_H__