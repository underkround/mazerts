#include "UI3DObjectManager.h"
#include "../App/XFileLoader.h"
#include "UIUnit.h"
#include "UIBuilding.h"
#include "UIProjectile.h"

#include "../../Model/Asset/IAsset.h"

#include "../../Model/Defs/DefManager.h"

#include "MeshFileNames.h"
#include "PlayerColors.h"
#include "HealthBlock.h"

void UI3DObjectManager::handleCreatedAsset(IAsset* pAsset)
{
    IAsset::Type assetType = pAsset->getAssetType();

    switch(assetType)
    {

    case IAsset::BUILDING:
        {
            createBuilding((Building*)pAsset);
            break;
        }

    case IAsset::UNIT:
        {
            createUnit((Unit*)pAsset);
            break;
        }
    }

}

void UI3DObjectManager::handleReleasedAsset(IAsset* pAsset)
{
    ListNode<UIAsset*>* pNode = m_AssetList.headNode();
    while(pNode)
    {
        if(pNode->item->getAsset() == pAsset)
        {
            //pNode->item->Release();
            //delete pNode->item;
            m_AssetList.remove(pNode);
            return;
        }
        pNode = pNode->next;
    }
}



void UI3DObjectManager::createBuilding(Building* pBuilding)
{
    UIBuilding* pUIBuilding = new UIBuilding(pBuilding);

    //Set bounding box-size, z-value defaults to 4
    pUIBuilding->setAABBSize(D3DXVECTOR3(pBuilding->getWidth(), pBuilding->getHeight(), 4.0f));

    //Get tag and find mesh-file from container
    int tag = pBuilding->getTypeTag()-51;

    CXFileLoader::Load(g_ppBuildingMeshNames[tag][0], m_ResourceContainer, pUIBuilding);

    // change base mesh color to player color
    pUIBuilding->setBaseMaterial(getPlayerMaterials(pBuilding->getOwner()->getIndex()));

    // creates healthblock (it automagically adds itself to asset)
    new HealthBlock(pUIBuilding, 0.5f, &m_ResourceContainer);

    m_RootObject.AddChild(pUIBuilding);
    m_AssetList.pushTail(pUIBuilding);
}

void UI3DObjectManager::createUnit(Unit *pUnit)
{    
    //Create UIUnit
    UIUnit* pUIUnit = new UIUnit(pUnit);
    

    //Set bounding box-size, z-value defaults to 4 (unless someone makes units to provide their depth)
    pUIUnit->setAABBSize(D3DXVECTOR3(pUnit->getWidth(), pUnit->getHeight(), 4.0f));

    //Get tag and find mesh-file from container
    int tag = pUnit->getTypeTag()-1;

    CXFileLoader::Load(g_ppUnitMeshNames[tag][0], m_ResourceContainer, pUIUnit);

    // change base mesh color to player color
    pUIUnit->setBaseMaterial(getPlayerMaterials(pUnit->getOwner()->getIndex()));

    // creates healthblock (it automagically adds itself to asset)
    new HealthBlock(pUIUnit, 0.5f, &m_ResourceContainer);

    //give health indicator to UI unit
    //HealthBar* hb = new HealthBar();
    //pUIUnit->setHealthBar(hb);

    m_RootObject.AddChild(pUIUnit);
    m_AssetList.pushTail(pUIUnit);

    if(pUnit->hasWeapon())
    {
        //Create weapon (turret), get mesh and offset accordingly
        UIWeapon* pUIWeapon = new UIWeapon(pUnit->getWeapon());
        CXFileLoader::Load(g_ppUnitMeshNames[tag][1], m_ResourceContainer, pUIWeapon);
        D3DXMATRIX& m = pUIWeapon->GetMatrix();
        m._41 = g_ppUnitMeshOffsets[tag][1][0];
        m._42 = g_ppUnitMeshOffsets[tag][1][1];
        m._43 = g_ppUnitMeshOffsets[tag][1][2];
        
        //SetUIWeapon takes care of parenting
        pUIUnit->setUIWeapon(pUIWeapon);
        //Firing callback for effects
        pUnit->getWeapon()->setFiringCallback((IUICallback*)pUIWeapon);

        if(g_ppUnitMeshNames[tag][2] != _T(""))
        {
            //Same as above for barrel
            C3DObject* pObject = new C3DObject();
            CXFileLoader::Load(g_ppUnitMeshNames[tag][2], m_ResourceContainer, pObject);
            D3DXMATRIX& m = pObject->GetMatrix();
            m._41 = g_ppUnitMeshOffsets[tag][2][0];
            m._42 = g_ppUnitMeshOffsets[tag][2][1];
            m._43 = g_ppUnitMeshOffsets[tag][2][2];
            pUIWeapon->setBarrel(pObject);
        }
    }
}


UIAsset* UI3DObjectManager::pickAsset(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir)
{
//http://www.toymaker.info/Games/html/picking.html

    UI3DObjectManager* pManager = getInstance();

    ListNode<UIAsset*>* node = pManager->m_AssetList.headNode();
    
    UIAsset* result = NULL;

    float dist;
    float smallestDist = 1000000.0f;
    D3DXVECTOR3 AABBMin;
    D3DXVECTOR3 AABBMax;    
    D3DXMATRIX inverseMat;
    D3DXVECTOR3 rayObjOrigin;
    D3DXVECTOR3 rayObjDir;

    while(node)
    {
        BOOL hit = false;

        UIAsset* pAsset = node->item;

        pAsset->getAABBVectors(&AABBMin, &AABBMax);

        //AABB-culling
        if(D3DXBoxBoundProbe(&AABBMin, &AABBMax, &rayOrigin, &rayDir))
        {
            //Intersection test
            D3DXMatrixInverse(&inverseMat, NULL, &pAsset->GetMatrix());

            D3DXVec3TransformCoord(&rayObjOrigin, &rayOrigin, &inverseMat);
            D3DXVec3TransformNormal(&rayObjDir, &rayDir, &inverseMat);
            D3DXVec3Normalize(&rayObjDir, &rayObjDir);

            D3DXIntersect(pAsset->GetMesh(), &rayObjOrigin, &rayObjDir, &hit, NULL, NULL, NULL, &dist, NULL, NULL);

            if(hit && dist < smallestDist)
            {
                smallestDist = dist;
                result = node->item;
            }
        }

        node = node->next;
    }


    return result;
}

void UI3DObjectManager::loadMeshes(LPDIRECT3DDEVICE9 pDevice)
{
    //Hardcoded filenames
    for(int i = 0; i < NUMBER_OF_UNITS_WITH_MESHES; i++)
    {
        CXFileLoader::Load(g_ppUnitMeshNames[i][0], m_ResourceContainer, NULL);
        CXFileLoader::Load(g_ppUnitMeshNames[i][1], m_ResourceContainer, NULL);
        CXFileLoader::Load(g_ppUnitMeshNames[i][2], m_ResourceContainer, NULL);
    }    

    for(int i = 0; i < NUMBER_OF_BUILDINGS_WITH_MESHES; i++)
    {
        CXFileLoader::Load(g_ppBuildingMeshNames[i][0], m_ResourceContainer, NULL);
        CXFileLoader::Load(g_ppBuildingMeshNames[i][1], m_ResourceContainer, NULL);
        CXFileLoader::Load(g_ppBuildingMeshNames[i][2], m_ResourceContainer, NULL);
    }    

    //Textures
    LPDIRECT3DTEXTURE9 pTexture = NULL;

    for(int i = 0; i < NUMBER_OF_TEXTURES; i++)
    {
        D3DXCreateTextureFromFile(pDevice, g_ppTextureNames[i], &pTexture);
        m_ResourceContainer.AddResource(g_ppTextureNames[i], pTexture);
    }

    //Miscellanous objects
    LPD3DXMESH pMesh = NULL;
    D3DXCreateSphere(getInstance()->getResourceContainer()->GetDevice(),
            0.3f, 6, 6, &pMesh, NULL);
    m_ResourceContainer.AddResource(_T("Shell"), pMesh);

    D3DMATERIAL9 material;
    memset(&material, 0, sizeof(D3DMATERIAL9));
    material.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
    material.Diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);    
    m_ResourceContainer.AddResource(_T("ShellMaterial"), material);
}

void UI3DObjectManager::createPlayerMaterials()
{
    for (int i = 0; i < MAXPLAYERCOUNT; i++)
    {
        D3DMATERIAL9 material;
        material.Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
        material.Diffuse = D3DXCOLOR(PLAYERCOLORS[i]);
        material.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
        material.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
        material.Power = 0.0f;
        m_ResourceContainer.AddResource(g_ppPlayerMaterialNames[i], material);
    }
}

D3DMATERIAL9* UI3DObjectManager::getPlayerMaterials(const unsigned int playerIndex)
{
    return m_ResourceContainer.FindMaterial(g_ppPlayerMaterialNames[playerIndex]);
}


void UI3DObjectManager::createProjectile(Projectile* pProjectile)
{    
    UIProjectile* pUIProj = new UIProjectile(pProjectile);
    pUIProj->Create(getInstance()->getResourceContainer()->FindMesh(_T("Shell")));

    C3DObject::MESHDATA meshdata;
    meshdata.pTexture = NULL;
    meshdata.pMaterial = getInstance()->getResourceContainer()->FindMaterial(_T("ShellMaterial"));

    pUIProj->AddMeshData(meshdata);

    getInstance()->getRootObject()->AddChild(pUIProj);
}