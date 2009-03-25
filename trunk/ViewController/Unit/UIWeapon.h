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

class UIWeapon : public C3DObject
{
public:

    /**
     * Constructor
	 * @param pWeapon Pointer to IWeapon this object represents
     */
    UIWeapon(IWeapon* pWeapon)
    {
        m_pWeapon = pWeapon;
        m_Alive = true;
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
};

#endif //__UIWEAPON_H__