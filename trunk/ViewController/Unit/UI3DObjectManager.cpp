#include "UI3DObjectManager.h"
#include "../App/XFileLoader.h"
#include "UIUnit.h"

#include "../../Model/Asset/IAsset.h"

UI3DObjectManager* UI3DObjectManager::pInstance = new UI3DObjectManager();


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
    //TODO: Create UIUnit from the pUnit-data and add to hierarchy

    UIUnit* pUIUnit = new UIUnit(pUnit);

    CXFileLoader::Load(_T("superTank.x"), m_ResourceContainer, pUIUnit);

    m_RootObject.AddChild(pUIUnit);
}