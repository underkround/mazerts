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

class C3DObjectDebug;

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
     * @param lifeTime Time in seconds, if you want the object to self-destruct
     *        after that amount of time (default = 0, does not destroy itself)
     * @return Pointer to created C3DObjectDebug or NULL if failed
     */
    static C3DObjectDebug* addSphere(float x, float y, float z, float radius, float lifeTime = 0.0f);

    /**
     * Adds a "line" (cylinder) from given location to another given location 
     * with given size
     * @param startX X-coordinate the line starts from
     * @param startY Y-coordinate the line starts from
     * @param startZ Z-coordinate the line starts from
     * @param endX X-coordinate the line ends to
     * @param endY Y-coordinate the line ends to
     * @param endZ Z-coordinate the line ends to
     * @param radius Radius of the line
     * @param lifeTime Time in seconds, if you want the object to self-destruct
     *        after that amount of time (default = 0, does not destroy itself)
     * @return Pointer to created C3DObjectDebug or NULL if failed
     */
    static C3DObjectDebug* addLine(float startX, float startY, float startZ, 
                              float endX, float endY, float endZ, float radius, 
                              float lifeTime = 0.0f);


private:
    UI3DDebug() {};
    ~UI3DDebug() {};

    /**
     * Sets full bright material and adds the object to container
     * @param pObject Pointer to C3DObjectDebug to add
     */
    static void addObject(C3DObjectDebug* pObject);

    /**
     * Device to use
     */
    static LPDIRECT3DDEVICE9 pDevice;
};

#endif //__3DDEBUG_H__