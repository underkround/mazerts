/**
 * Basic Camera-object
 *
 * NOTE after static-camera stuff:
 *  it's assumed that the m_DefaultCamera is always set!
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

//TODO: Camera rotation, zoom...

#include "../../Model/Common/DoubleLinkedList.h"
#include "d3dx9.h"

class SphereCamera; // as default camera

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

// =====

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
     * Get the pitch-value (up/down-rotation)
     */
    inline float getPitch() { return m_Pitch; }
    
    /** 
     * Get the yaw-value (sideways rotation)
     */
    inline float getYaw() { return m_Yaw; }

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

// ===== Static camera -stuff

    /**
     * Set default fallback camera that is used if there is no overriding
     * cameras in the pile. The push/pop -functionality does not apply to this
     * camera. If there is already a default camera, it will be released and
     * deleted.
     * UPDATE:  disabled since it's crusial to have working default camera
     *          (that is not null)
     */
    //static void setDefault(Camera* camera);

    /**
     * Return the default camera, which may or may not be the current
     */
    static inline Camera* getDefault()
    {
        return (Camera*)&m_DefaultCamera;
    }

    /**
     * Get pointer to currently active camera, or NULL if none set (stack is
     * empty and no default camera is set).
     */
    static inline Camera* getCurrent()
    {
        return current;
    }

    /**
     * Push new camera to the top of the camera-stack and set it as current.
     */
    static void pushTop(Camera* camera);

    /**
     * Push new camera to the bottom of the camera-stack. The current camera
     * is the top of the stack, or default if the stack is empty.
     */
    static void pushBack(Camera* camera);

    /**
     * Remove and delete camera on top of the stack, if any. Updates the
     * current camera.
     * @return  true, if there was camera in the stack and it was deleted,
     *          false if the stack was already empty
     * @deprecated - removed as dangerous (cameras could be automatic
     *               and deletion will fail)
     */
//    static bool releaseTop();
//    static bool releaseBack();

    /**
     * Pop given camera from the stack, if it's found from there
     * @return true, if camera was in the stack and was popped out
     */
    static bool pop(Camera* camera);

    /**
     * Pop top (current) camera out of the stack and set previous (if any)
     * or the default camera as current
     * The popped camera-object will not be deleted.
     * @return  pointer to removed camera
     */
    static Camera* popTop();

    /**
     * Pop the last camera out of the stack.
     * The popped camera-object will be deleted, do it with returned
     * pointer.
     */
    static Camera* popBack();

    /**
     * @return  number of cameras in the stack, including the default camera not
     *          in the stack
     */
    static inline int countStack()
    {
        return m_Cameras.count();
    }

    // Pointer to current camera, which is either the default or top
    // from the stack. Keeping this just to reduce overhead for checking
    // which is the default ;)
    //
    // NOTE: this is set to public only to reduce overhead. DO NOT modify
    // this, use as read-only ;P
    static Camera*                     current;

protected:

    // Default camera that is used if there is no overriding cameras
    // Set separately from other cameras to use as default fallback
//    static Camera*                     m_DefaultCamera;
    static SphereCamera                 m_DefaultCamera;

    // Overriding cameras
    static DoubleLinkedList<Camera*>    m_Cameras;

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
