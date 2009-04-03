/**
 * UIUnit
 *
 * Handles the ui 3d-representation of a single unit
 *
 * $Revision: 242 $
 * $Date: 2009-03-31 00:27:53 +0300 (ti, 31 maalis 2009) $
 * $Id: UIUnit.h 242 2009-03-30 21:27:53Z murgo $
 */

#ifndef __UIUNIT_H__
#define __UIUNIT_H__

#include <d3dx9.h>
#include "UIAsset.h"
#include "../../Model/Asset/Unit.h"
#include "UIWeapon.h"

// TODO: remove when implementing real marker object
#include "../3DDebug/C3DObjectDebug.h"

class UIUnit : public UIAsset
{
public:

    /**
     * Constructor
     * @param pUnit Pointer to Model-side unit this UIUnit represents
     */
    UIUnit(Unit* pUnit) : UIAsset(pUnit)
    {
        m_pUnit = pUnit;
        m_pUIWeapon = NULL;
    }

    /**
     * Destructor
     */
    virtual ~UIUnit()
    {
    }

    /**
     * Render the unit
     * @param pDevice pointer to Direct3D-device to use
     */
    virtual void Render(LPDIRECT3DDEVICE9 pDevice);

    /**
     * @return pointer to the model-size unit that this UiUnit represents
     */
    inline Unit* getUnit()
    {
        return m_pUnit;
    }

protected:

    /**
     * Handles updating the unit-matrix and (if ground unit)
     * aligns the model to the terrain below it
     */
    void updatePosition();

// ===== Members

    /**
     * Pointer to Model-side Unit-instance this is a representation of
     */
    Unit*           m_pUnit;

    /**
     * Used to keep track of terrain changes
     */
    unsigned short m_UITerrain_ChangeCounter;

    /**
     * Used to keep track if unit has moved
     */
    D3DXVECTOR3 m_OldPosition;

    /**
     * Used to keep track if unit has turned
     */
    D3DXVECTOR3 m_OldDirection;

};

#endif //__UIUNIT_H__
