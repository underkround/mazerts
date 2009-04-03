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
#include "../../Model/Weapon/IWeapon.h"
#include "../../Model/Asset/IAsset.h"
#include "../../Model/Common/IUICallback.h"

class UIWeapon : public C3DObject, IUICallback
{
public:

    /**
     * Constructor
     * @param pWeapon Pointer to IWeapon this object represents
     */
    UIWeapon(IWeapon* pWeapon)
    {
        m_pWeapon = pWeapon;
        m_pBarrel = NULL;
        m_Alive = true;
        m_Tag = pWeapon->getHost()->getTypeTag();
    }

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
     * Tag of the unit to get the correct barrel length for firing effect
     */
    int m_Tag;
};

#endif //__UIWEAPON_H__