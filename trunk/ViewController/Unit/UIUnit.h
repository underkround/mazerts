/**
 * UIUnit
 *
 * Handles the ui 3d-representation of a single unit
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __UIUNIT_H__
#define __UIUNIT_H__

#include <d3dx9.h>
#include "../App/C3DObject.h"
#include "../../Model/Asset/Unit.h"
#include "../../Model/Asset/IAssetListener.h"
#include "UIWeapon.h"

// TODO: remove when implementing real marker object
#include "../3DDebug/C3DObjectDebug.h"

class UIUnit : public C3DObject, IAssetListener
{
public:

    /**
     * Constructor
     * @param pUnit Pointer to Model-side unit this UIUnit represents
     */
    UIUnit(Unit* pUnit)
    {
        m_pUnit = pUnit;
        m_HalfSize = pUnit->getWidth() * 0.5f;        
        m_Alive = true;
        m_Selected = false;
        m_SelectionMarker = NULL;
		m_pWeapon = NULL;
        //Register as listener to pUnit
        pUnit->registerListener(this);		
    }

    /**
     * Destructor
     */
    virtual ~UIUnit()
    {
    }

    /**
     * Update the unit
     * @param fFrametime Time elapsed in frame as seconds
     * @return False, if no action should take place, true
     *         if the child should be removed by parent
     */
    virtual bool Update(float fFrametime);

    /**
     * Render the unit
     * @param pDevice pointer to Direct3D-device to use
     */
    virtual void Render(LPDIRECT3DDEVICE9 pDevice);

    /**
     * This gets called when the target assets's state changes.
     * @param pAsset    The asset of which state has changed
     * @param newState  The new state of the asset
     */
    virtual void handleAssetStateChange(IAsset* pAsset, IAsset::State newState);

    /**
     * This gets called when the asset object is being released & destroyed.
     * This notification will also come through AssetCollection
     * to IAssetCollectionListeners, but if you only need to know
     * weather this specific unit is being destroyed, use this.
     * @param pAsset    Pointer to the unit that is to be released & deleted
     */
    virtual void handleAssetReleased(IAsset* pAsset)
    {
        m_Alive = false;
    };

    /**
     * Activate / Deactivate markers (visual + audio) for selection
     */
    void setSelected(const bool value);

    /**
     * Return selection state of this uiunit
     */
    inline const bool isSelected() { return m_Selected; }

    /**
     * Return "halfsize" (offset) of unit
     */
    inline const float getHalfSize() { return m_HalfSize; }

    /**
     * @return pointer to the model-size unit that this UiUnit represents
     */
    inline Unit* getUnit()
    {
        return m_pUnit;
    }

	/**
	 * Gets the weapon of the UIUnit
	 * @return Pointer to UIWeapon
	 */
	inline UIWeapon* getWeapon() { return m_pWeapon; }

	/**
	 * Sets the weapon of the UIUnit and connects the parent-child -relationship
	 * @param pWeapon Pointer to UIWeapon to set for this unit
	 */
	inline void setWeapon(UIWeapon* pWeapon) 
	{ 
		if(m_pWeapon)
		{			
			//If current weapon exists, it will be destroyed on next update
			m_pWeapon->setAlive(false);
		}
		m_pWeapon = pWeapon; 
		AddChild(m_pWeapon);
	}

protected:

    /**
     * Handles updating the unit-matrix and (if ground unit)
     * aligns the model to the terrain below it
     */
    void updatePosition();

// ===== Members

    /**
     * If set true, this unit is currently selected
     */
    bool            m_Selected;

    /**
     * TODO: change to regular object when implementing real marker
     * instead of debug-one
     */
    C3DObjectDebug* m_SelectionMarker;

    /**
     * Pointer to Model-side Unit-instance this is a representation of
     */
    Unit*           m_pUnit;

    /**
     * Half of the actual unit "grid-size", used to offset the model correctly
     */
    float           m_HalfSize;

    /**
     * Set to false by handleAssetReleased
     */
    bool            m_Alive;

	/**
	 * Weapon of the unit
	 */
	UIWeapon* m_pWeapon;
};

#endif //__UIUNIT_H__