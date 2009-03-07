/**
 * UI3DObjectManager
 *
 * Singleton manager that is registered as a listener with the asset-collection.
 * Handles the creation and destruction of the 3d-representations of Assets and
 * manages the object hierarchy.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __UI3DOBJECTMANAGER_H__
#define __UI3DOBJECTMANAGER_H__

#include "../App/C3DObject.h"
#include "../App/C3DResourceContainer.h"
#include "../../Model/Asset/AssetCollection.h"
#include "../../Model/Asset/IAssetCollectionListener.h"

class UI3DObjectManager : public IAssetCollectionListener
{
public:

    /**
     * THIS HAS TO BE CALLED PRIOR TO USING MANAGER
     * @param pDevice Pointer to Direct3D9-device
     */
    inline static void setDevice(LPDIRECT3DDEVICE9 pDevice)
    {        
        getInstance()->m_ResourceContainer.Create(pDevice);
    }

    /**
     * Gets the UI3DObjectManager-instance
     * @return Pointer to UI3DObjectManager
     */
    inline static UI3DObjectManager* getInstance() 
    { 
        static UI3DObjectManager instance;
        return &instance; 
    }

    /**
     * Gets the root-object of the C3DObject-hierarchy
     * @return Pointer to C3DObject representing the root of the hierarchy
     */
    inline C3DObject* getRootObject() { return &m_RootObject; }

    /**
     * Handle notification of new asset that has just been created to the game.
     * @param pAsset  pointer to created new asset
     */
    virtual void handleCreatedAsset(IAsset* pAsset);

    /**
     * Called when asset object is being destroyed. Handles removing the associated
     * C3DObject from the hierarchy
     * @param pAsset  pointer to asset instance to be deteled
     */
    virtual void handleReleasedAsset(IAsset* pAsset);

    /**
     * Global scaling value
     */
    static float globalScale;

private:

    /**
     * Constructor, handles registering this singleton manager to the Model-side AssetCollection
     * and initializing the root-object
     */
    UI3DObjectManager()
    {
	    m_RootObject.Create(NULL);
	    m_RootObject.SetActive(FALSE);
	    m_RootObject.SetVisible(FALSE);
        AssetCollection::registerListener(this);
    }

    /**
     * Creates a new C3DObject from the given Building-asset and adds to hierarchy
     * @param pBuilding Pointer to Building-instance used in creation
     */
    void createBuilding(Building* pBuilding);

    /**
     * Creates a new C3DObject from the given Unit-asset and adds to hierarchy
     * @param pUnit Pointer to Unit-instance used in creation
     */
    void createUnit(Unit* pUnit);

    /**
     * Root of the C3DObject-hierarchy
     */
    C3DObject m_RootObject;

    /**
     * Resource container
     */
    C3DResourceContainer m_ResourceContainer;

};

#endif //__UI3DOBJECTMANAGER_H__