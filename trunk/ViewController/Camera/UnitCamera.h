/**
 * Camera-object that is attached to UiUnit
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __UNITCAMERA_H__
#define __UNITCAMERA_H__

#include "Camera.h"
#include "d3dx9.h"

#include "../../Model/Asset/IAssetListener.h"
#include "../Unit/UIUnit.h"
#include "../App/I3DObject.h"

class UnitCamera : public Camera, public I3DObject, public IAssetListener
{
public:

    UnitCamera();
    ~UnitCamera();

    void reset();

    void attach(UIUnit* unit);

    const bool isAttached() const { return (m_pUIUnit) ? true : false; }

    void detach();

// ===== Camera

    virtual void update();

    virtual void move(const float depth, const float sideways, const float vertical);

//    virtual void zoom(const float change);

//    virtual void rotate(const float yaw, const float pitch);


// ===== I3DObject

    virtual bool Update(float fFrametime);

    virtual void Render(LPDIRECT3DDEVICE9 pDevice);

// ===== Listener functionality

    virtual void handleAssetStateChange(IAsset* pAsset, IAsset::State newState);

    virtual void handleAssetReleased(IAsset* pAsset);

protected:

    // unit we are following
    UIUnit*     m_pUIUnit;

    float m_HeightMod;
    float m_ModX;
    float m_ModY;
    float m_ModZ;

};

#endif // __UNITCAMERA_H__
