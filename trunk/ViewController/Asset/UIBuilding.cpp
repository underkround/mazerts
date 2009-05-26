#include "UIBuilding.h"
#include "../Sound/SoundManager.h"

UIBuilding::UIBuilding(Building* pBuilding) : UIAsset(pBuilding)
{
    m_pBuilding = pBuilding;

    D3DXMatrixRotationX(&m_mLocal, D3DX_PI * 0.5f);
    m_mLocal._11 = -m_mLocal._11;    
    m_mLocal._12 = -m_mLocal._12;
    m_mLocal._13 = -m_mLocal._13;
    m_mLocal._21 = -m_mLocal._21;
    m_mLocal._22 = -m_mLocal._22;
    m_mLocal._23 = -m_mLocal._23;
    m_mLocal._31 = -m_mLocal._31;
    m_mLocal._32 = -m_mLocal._32;
    m_mLocal._33 = -m_mLocal._33;

    //Update position
    m_mLocal._41 = pBuilding->getGridX() + (pBuilding->getWidth() / 2.0f);
    m_mLocal._42 = pBuilding->getGridY() + (pBuilding->getHeight() / 2.0f);
    //m_mLocal._43 = UITerrain::getInstance()->calculateTriangleHeightAt(m_mLocal._41, m_mLocal._42);
    m_mLocal._43 = Terrain::getInstance()->getTerrainHeight((const short)m_mLocal._41, (const short)m_mLocal._42) * -UITerrain::HEIGHTFACTOR;

    m_Powered = true;

    int i = GetMeshDataArray().size();
    m_ppOriginalMaterials = new D3DMATERIAL9*[i];
}

UIBuilding::~UIBuilding()
{
    /*
    if (m_ppOriginalMaterials) {
        delete [] m_ppOriginalMaterials;
        m_ppOriginalMaterials = NULL;
    }
    */
}

void UIBuilding::setBaseMaterial(D3DMATERIAL9* pBaseMaterial, D3DMATERIAL9* pDisabledMaterial)
{
    // do not set base material if def is negative
    if (m_pBuilding->getDef()->basematerial >= 0)
    {
        GetMeshDataArray()[m_pBuilding->getDef()->basematerial].pMaterial = pBaseMaterial;
        m_pDisabledMaterial = pDisabledMaterial;
    }
}

void UIBuilding::handleAssetStateChange(IAsset* pAsset, IAsset::State newState)
{
    if (newState == IAsset::STATE_DISABLED && m_Powered)
    {
        m_Powered = false;
        // change into disabled material
        for (unsigned int i = 0; i < GetMeshDataArray().size(); i++)
        {
            m_ppOriginalMaterials[i] = GetMeshDataArray()[i].pMaterial;
            GetMeshDataArray()[i].pMaterial = m_pDisabledMaterial;
        }
    }
    else if (newState == IAsset::STATE_ACTIVE && !m_Powered)
    {
        // enable power
        m_Powered = true;
        // change into enabled material
        for (unsigned int i = 0; i < GetMeshDataArray().size(); i++)
        {
            GetMeshDataArray()[i].pMaterial = m_ppOriginalMaterials[i];
        }
    }

    if (newState == IAsset::STATE_ACTIVE)
    {
        if (UITerrain::getInstance()->getCurrentPlayer() == pAsset->getOwner())
            SoundManager::playSound(SOUND_CONSTRUCTION_COMPLETE, 0.01f, *(D3DXVECTOR3*)pAsset->getPosition(), false);
    }
}

