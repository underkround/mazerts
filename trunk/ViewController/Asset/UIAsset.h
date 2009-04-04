/**
 * UIAsset
 *
 * Base for UIUnit/UIBuilding
 *
 * $Revision: 242 $
 * $Date: 2009-03-31 00:27:53 +0300 (ti, 31 maalis 2009) $
 * $Id: UIUnit.h 242 2009-03-30 21:27:53Z murgo $
 */

#ifndef __UIASSET_H__
#define __UIASSET_H__

#include <d3dx9.h>
#include "../App/C3DObject.h"
#include "../../Model/Asset/IAsset.h"
#include "../../Model/Asset/IAssetListener.h"

#include "ParticleFactory.h"
#include "HealthBlock.h"
#include "UiWeapon.h"

class UIAsset : public C3DObject, IAssetListener
{
public:

    /**
     * Constructor
     * @param pAsset Pointer to Model-side unit this UIAsset represents
     */
    UIAsset(IAsset* pAsset)
    {
        m_pAsset = pAsset;
        m_HalfSize = pAsset->getWidth() * 0.5f;        
        m_Alive = true;
        m_Selected = false;
        m_pHealthBlock = NULL;
        //Register as listener to pUnit
        pAsset->registerListener(this);        
    }

    /**
     * Destructor
     */
    virtual ~UIAsset()
    {
    }

    /**
     * Update the asset
     * @param fFrametime Time elapsed in frame as seconds
     * @return False, if no action should take place, true
     *         if the child should be removed by parent
     */
    virtual bool Update(float fFrametime);

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
     * wether this specific unit is being destroyed, use this.
     * @param pAsset    Pointer to the unit that is to be released & deleted
     */
    virtual void handleAssetReleased(IAsset* pAsset)
    {
        m_Alive = false;
        ParticleFactory::createExplosion((D3DXVECTOR3&)m_mWorld._41, (int)(m_HalfSize) * 3);
        m_pHealthBlock->kill();
    }

    /**
     * Activate / Deactivate markers (visual + audio) for selection
     */
    void setSelected(const bool value);

    /**
     * Return selection state of this uiunit
     */
    inline const bool isSelected() { return m_Selected; }

    /**
     * Return alive-state of the UIAsset (represents the state of the model Asset)
     */
    inline const bool isAlive() { return m_Alive; }

    /**
     * Return "halfsize" (offset) of asset
     */
    inline const float getHalfSize() { return m_HalfSize; }

    /**
     * @return pointer to the model-size unit that this UiUnit represents
     */
    inline IAsset* getAsset()
    {
        return m_pAsset;
    }

    /**
     * Gets the health indicator of the UIUnit
     * @return Pointer to HealthBar
     */
    /*
    inline HealthBar* getHealthBar() { return m_pHealthBar; }

    inline void setHealthBar(HealthBar* pHealthBar)
    {
        if(!m_pHealthBar)
        {
            m_pHealthBar = pHealthBar;
            //m_pHealthBar->Create(
            AddChild(m_pHealthBar);
        }
    }
    */

    /**
     * Set base material
     * Used for player colors
     */
    inline void setBaseMaterial(D3DMATERIAL9* pMaterial)
    {
        GetMeshDataArray()[0].pMaterial = pMaterial;
    }

    /**
     * HealthBlock calls this internally
     */
    inline void setHealthBlock(HealthBlock* pHealthBlock)
    {
        if (m_pHealthBlock)
        {
            // this might be buggy (doesn't remove from hierarchy), then again
            // you're not supposed to come here
            m_pHealthBlock->Release();
            delete m_pHealthBlock;
        }
        m_pHealthBlock = pHealthBlock;
        AddChild(pHealthBlock);
    }

    inline HealthBlock* getHealthBlock() { return m_pHealthBlock; }

    /**
     * Gets the weapon of the UIAsset
     * @return Pointer to UIWeapon
     */
    inline UIWeapon* getUIWeapon() { return m_pUIWeapon; }

    /**
     * Sets the weapon of the UIUnit and connects the parent-child -relationship
     * @param pWeapon Pointer to UIWeapon to set for this unit
     */
    inline void setUIWeapon(UIWeapon* pUIWeapon) 
    { 
        if(m_pUIWeapon)
        {            
            //If current weapon exists, it will be destroyed on next update
            m_pUIWeapon->setAlive(false);
        }
        m_pUIWeapon = pUIWeapon; 
        AddChild(m_pUIWeapon);
    }

    /**
     * Sets the alive-state of the asset
     */ 
    inline void setAlive(bool alive) { m_Alive = alive; }

protected:

// ===== Members

    /**
     * If set true, this asset is currently selected
     */
    bool            m_Selected;

    /**
     * Pointer to Model-side IAsset-instance this is a representation of
     */
    IAsset*         m_pAsset;

    /**
     * Half of the actual unit "grid-size", used to offset the model correctly
     */
    float           m_HalfSize;

    /**
     * Set to false by handleAssetReleased
     */
    bool            m_Alive;

    /**
     * pointer to unit's health status indicator
     */
    //HealthBar* m_pHealthBar;
    HealthBlock* m_pHealthBlock;

    /**
     * Pointer to UIWeapon of this asset
     */
    UIWeapon* m_pUIWeapon;

};

#endif //__UIASSET_H__
