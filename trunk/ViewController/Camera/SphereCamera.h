/**
 * Camera-object that moves in "sphere" around the given point
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __SPHERECAMERA_H__
#define __SPHERECAMERA_H__

#include "Camera.h"
#include "d3dx9.h"

class SphereCamera : public Camera
{
public:

    static const float CAM_MINDISTANCE;
    static const float CAM_MINPITCH;
    static const float CAM_MAXPITCH;


    SphereCamera();
    virtual ~SphereCamera() {}

    /**
     * Positions the look at-point (camera will "jump" there automatically on update)
     * z-value won't matter
     */
    virtual inline void setPosition(const float x, const float y, const float z)
    {
        m_LookAt.x = x;
        m_LookAt.y = y;
        m_LookAt.z = z;
        m_NeedsUpdate = true;
        m_Test = true;
    }
    
    virtual inline void setPosition(const D3DXVECTOR3& pos)
    {
        m_LookAt.x = pos.x;
        m_LookAt.y = pos.y;
        m_LookAt.z = pos.z;
        m_NeedsUpdate = true;
        m_Test = true;
    }

    virtual inline D3DXVECTOR3& getPosition() { return m_LookAt; }

    /**
     * Moves the look at -point based on current direction
     * @param depth Movement in depth (forwards/backwards)
     * @param sideways Sideway movement (left/right)
     * @param vertical Vertical movement (up/down)
      */
    virtual void move(const float depth, const float sideways, const float vertical);
    
    /** 
     * Changes the camera distance from look at -point and 
     * limits the minimum value
     * @param change Change in distance
     */
    virtual void zoom(const float change);

    /**
     * Changes the camera rotation around the look at-point
     * and limits the pitch-value
     */
    virtual void rotate(const float yaw, const float pitch);

    /**
     * Rebuilds view matrix based on current camera settings and
     * calls upper-level update to handle setting & frustum building
     */
    virtual void update();

private:

    /**
     * The point camera is looking at
     */
    D3DXVECTOR3 m_LookAt;
};

#endif