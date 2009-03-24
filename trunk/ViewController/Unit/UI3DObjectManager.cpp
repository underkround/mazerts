#include "UI3DObjectManager.h"
#include "../App/XFileLoader.h"
#include "UIUnit.h"

#include "../../Model/Asset/IAsset.h"

#include "../../Model/Defs/DefManager.h"

#include "MeshFileNames.h"


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
}



void UI3DObjectManager::createBuilding(Building* pBuilding)
{
    //TODO: Create UIBuilding from the pBuilding-data and add to hierarchy
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

    m_RootObject.AddChild(pUIUnit);
    m_UnitList.pushTail(pUIUnit);

	if(pUnit->hasWeapon())
	{
		//TODO: move loading & offsetting to method
		UIWeapon* pUIWeapon = new UIWeapon();
		CXFileLoader::Load(g_ppUnitMeshNames[tag][1], m_ResourceContainer, pUIWeapon);
		D3DXMATRIX& m = pUIWeapon->GetMatrix();
		m._41 = g_ppUnitMeshOffsets[tag][1][0];
		m._42 = g_ppUnitMeshOffsets[tag][1][1];
		m._43 = g_ppUnitMeshOffsets[tag][1][2];
		pUIUnit->setWeapon(pUIWeapon);

		if(g_ppUnitMeshNames[tag][2] != _T(""))
		{
			C3DObject* pObject = new C3DObject();
			CXFileLoader::Load(g_ppUnitMeshNames[tag][2], m_ResourceContainer, pObject);
			D3DXMATRIX& m = pObject->GetMatrix();
			m._41 = g_ppUnitMeshOffsets[tag][2][0];
			m._42 = g_ppUnitMeshOffsets[tag][2][1];
			m._43 = g_ppUnitMeshOffsets[tag][2][2];
			pUIWeapon->AddChild(pObject);
		}
	}
}


UIUnit* UI3DObjectManager::pickUnit(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir)
{
//http://www.toymaker.info/Games/html/picking.html

    UI3DObjectManager* pManager = getInstance();

    ListNode<UIUnit*>* node = pManager->m_UnitList.headNode();
    
    UIUnit* result = NULL;

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

        UIUnit* pUnit = node->item;

        pUnit->getAABBVectors(&AABBMin, &AABBMax);

        //AABB-culling
        if(D3DXBoxBoundProbe(&AABBMin, &AABBMax, &rayOrigin, &rayDir))
        {
            //Intersection test
            D3DXMatrixInverse(&inverseMat, NULL, &pUnit->GetMatrix());

            D3DXVec3TransformCoord(&rayObjOrigin, &rayOrigin, &inverseMat);
            D3DXVec3TransformNormal(&rayObjDir, &rayDir, &inverseMat);
            D3DXVec3Normalize(&rayObjDir, &rayObjDir);

            D3DXIntersect(pUnit->GetMesh(), &rayObjOrigin, &rayObjDir, &hit, NULL, NULL, NULL, &dist, NULL, NULL);

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

void UI3DObjectManager::loadMeshes(void)
{

	//Hardcoded filenames
	for(int i = 0; i < NUMBER_OF_UNITS_WITH_MESHES; i++)
	{
		CXFileLoader::Load(g_ppUnitMeshNames[i][0], m_ResourceContainer, NULL);
		CXFileLoader::Load(g_ppUnitMeshNames[i][1], m_ResourceContainer, NULL);
		CXFileLoader::Load(g_ppUnitMeshNames[i][2], m_ResourceContainer, NULL);
	}	
}