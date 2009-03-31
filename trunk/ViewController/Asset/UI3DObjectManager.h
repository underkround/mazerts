/**
 * UI3DObjectManager
 *
 * Singleton manager that is registered as a listener with the asset-collection.
 * Handles the creation and destruction of the 3d-representations of Assets and
 * manages the object hierarchy.
 *
 * $Revision: 242 $
 * $Date: 2009-03-31 00:27:53 +0300 (ti, 31 maalis 2009) $
 * $Id: UI3DObjectManager.h 242 2009-03-30 21:27:53Z murgo $
 */

#ifndef __UI3DOBJECTMANAGER_H__
#define __UI3DOBJECTMANAGER_H__

#include "../App/C3DObject.h"
#include "../App/C3DResourceContainer.h"
#include "../../Model/Asset/AssetCollection.h"
#include "../../Model/Asset/IAssetCollectionListener.h"
#include "../../Model/Common/Config.h"

//FORWARD DECLARATIONS
class UIAsset;

class UI3DObjectManager : public IAssetCollectionListener
{
public:

    /**
     * Creates the instance and resource container for object manager
     * @param pDevice Pointer to Direct3D9-device
     */
    inline static void create(LPDIRECT3DDEVICE9 pDevice)
    {        
        getInstance()->m_ResourceContainer.Create(pDevice);
        getInstance()->loadMeshes(pDevice);
        Config::getInstance()->setFilename("meshes.ini");
        Config::getInstance()->readFile();
        getInstance()->createPlayerMaterials();
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
     * Gets the resource container of the object manager
     * @return Pointer to C3DResourceContainer
     */
    inline C3DResourceContainer* getResourceContainer() { return &m_ResourceContainer; }

    /**
     * Gets the list of assets within the object manager
     * @return Pointer to asset list
     */
    inline DoubleLinkedList<UIAsset*>* getAssetList() { return &m_AssetList; }

    /** 
     * Releases the children of root-object
     */ 
    inline void release()
    {
        //Release children
        m_RootObject.Release();
    }

    /**
     * Picks the assets with given ray and returns the picked Asset (if any)
     * @param rayOrigin  Origin of the ray (world-space)
     * @param rayDir     Direction of the ray (world-space)
     * @Return UIAsset-pointer to picked asset or NULL if no asset was hit
     */
    static UIAsset* pickAsset(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir);

//IAssetCollectionListener
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
     * Returns player material (color)
     * @param playerIndex   index of the player
     */
    D3DMATERIAL9* getPlayerMaterials(const unsigned int playerIndex);

    /**
     * Creates player materials
     */
    void createPlayerMaterials();

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
     * Load meshes defined in configs into container
     * @param pDevice Pointer to Direct3D-device
     */
    void loadMeshes(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Creates (if needed) and returns mesh for health block
     */
    LPD3DXMESH getHealthBlockMesh(const float radius);

    /**
     * Root of the C3DObject-hierarchy
     */
    C3DObject m_RootObject;

    /**
     * Resource container
     */
    C3DResourceContainer m_ResourceContainer;

    /**
     * List of assets
     */
    DoubleLinkedList<UIAsset*> m_AssetList;

};

#endif //__UI3DOBJECTMANAGER_H__
