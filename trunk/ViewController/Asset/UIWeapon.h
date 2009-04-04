/**
 * UIWeapon
 *
 * Handles the ui 3d-representation of a single unit weapon
 *
 * $Revision: 183 $
 * $Date: 2009-03-18 13:48:39 +0200 (ke, 18 maalis 2009) $
 * $Id: UIUnit.h 183 2009-03-18 11:48:39Z zem $
 */

#ifndef __UIWEAPON_H__
#define __UIWEAPON_H__

#include <d3dx9.h>
#include "../App/C3DObject.h"
#include "../App/C3DParticleEmitter.h"
#include "../../Model/Weapon/IWeapon.h"
#include "../../Model/Asset/IAsset.h"
#include "../../Model/Common/IUICallback.h"
#include "UI3DObjectManager.h"
#include "MeshFileNames.h"

class UIWeapon : public C3DObject, IUICallback
{
public:

    /**
     * Constructor
     * @param pWeapon Pointer to IWeapon this object represents
     */
    UIWeapon(IWeapon* pWeapon);

    /**
     * Destructor
     */
    virtual ~UIWeapon()
    {
    }

    /**
     * Update the weapon
     * @param fFrametime Time elapsed in frame as seconds
     * @return False, if no action should take place, true
     *         if the child should be removed by parent
     */
    virtual bool Update(float fFrametime);
    
    /**
     * Sets the alive-state of the weapon
     * @param alive Set to false to get the weapon destroyed on next update
     */
    inline void setAlive(bool alive) { m_Alive = alive; }

    /**
     * Set the object acting as weapon barrel, handles parenting
     * @param pBarrel Pointer to C3DObject acting as barrel
     */
    inline void setBarrel(C3DObject* pBarrel)
    {
        m_pBarrel = pBarrel;
        AddChild(pBarrel);
        pBarrel->AddChild(m_pFireEmitter);
    }

    /**
     * Callback-method for firing
     */
    virtual void callBack(void);

protected:

// ===== Members

    /**
     * Set to false to get the object deleted on next update
     */
    bool            m_Alive;

    /**
     * Pointer to model-side weapon this object represents
     */
    IWeapon*        m_pWeapon;

    /**
     * Old weapon heading, used to check if updating matrix is necessary
     */
    D3DXVECTOR3 m_OldHeading;

    /**
     * Pointer to object acting as weapon barrel
     */
    C3DObject* m_pBarrel;

    /**
     * Old pitch for barrel, used to check if matrix update is necessary
     */
    float m_OldPitch;

    /**
     * Particle emitter for muzzle-flares
     */
    C3DParticleEmitter* m_pFireEmitter;
    C3DParticleEmitter::EMIT_PARAMS m_EmitParams;

};

#endif //__UIWEAPON_H__