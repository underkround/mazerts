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

        //Register as listener to pUnit
        pUnit->registerListener(this);
    }

    /**
     * Update the unit
     * @param fFrametime Time elapsed in frame as seconds
     * @return False, if no action should take place, true
     *         if the child should be removed by parent
     */
    virtual bool Update(float fFrametime);

    /**
     * Destructor
     */
    virtual ~UIUnit()
    {
    }


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

protected:
    
    /**
     * Aligns the model to the terrain below it
     */
    void alignToTerrain();

    /**
     * Pointer to Model-side Unit-instance this is a representation of
     */
    Unit* m_pUnit;

    /**
     * Half of the actual unit "grid-size", used to offset the model correctly
     */
    float m_HalfSize;

    /**
     * Set to false by handleAssetReleased
     */
    bool m_Alive;

};

#endif //__UIUNIT_H__