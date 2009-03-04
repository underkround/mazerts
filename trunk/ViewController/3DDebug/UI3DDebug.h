/**
 * UI3DDebug
 *
 * Helper methods to add different type of 3d-markers etc.
 * setDevice must be called before using!
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __UI3DDEBUG_H__
#define __UI3DDEBUG_H__

#include <d3dx9.h>

class C3DObject;

class UI3DDebug
{
public:
    
    static void setDevice(LPDIRECT3DDEVICE9 pD3DDevice)
    {
        pDevice = pD3DDevice;
    }

    /**
     * Adds a sphere to given location with given size
     * @param x X-coordinate
     * @param y Y-coordinate
     * @param z Z-coordinate
     * @param radius Radius of the sphere     
     * @return Pointer to created C3DObject or NULL if failed
     */
    static C3DObject* addSphere(float x, float y, float z, float radius);

    /**
     * Adds a "line" (cylinder) from given location to another given location 
     * with given size
     * @param startPos Position of the sphere
     * @param radius Radius of the sphere     
     * @return Pointer to created C3DObject or NULL if failed
     */
    static C3DObject* addLine(float startX, float startY, float startZ, 
                              float endX, float endY, float endZ, float radius);


private:
    UI3DDebug() {};
    ~UI3DDebug() {};

    /**
     * Device to use
     */
    static LPDIRECT3DDEVICE9 pDevice;
};

#endif //__3DDEBUG_H__