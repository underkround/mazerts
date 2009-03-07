#include "UIUnit.h"
#include "../Terrain/UITerrain.h"
#include "../App/I3DObject.h"
#include "UI3DObjectManager.h"


bool UIUnit::Update(float fFrameTime) 
{
    alignToTerrain();
    I3DObject::Update(fFrameTime);
    return m_Alive;
}

void UIUnit::alignToTerrain()
{
    //Terrain normal
    D3DXVECTOR3 normal = UITerrain::getInstance()->getNormalAt(
                                    m_pUnit->getPosition()->x, 
                                    m_pUnit->getPosition()->y, 
                                    m_pUnit->getWidth(), 
                                    m_pUnit->getWidth()
                                    );

    D3DXVECTOR3* dir = (D3DXVECTOR3*)m_pUnit->getDirection();

    //Get "right"-vector from crossproduct of direction (around z-axis) and normal    
    D3DXVECTOR3 right;
    D3DXVec3Normalize(dir, dir);
    D3DXVec3Cross(&right, &normal, dir);

    //Find new "forward"-vector based on normal and right vector
    D3DXVECTOR3 forward;
    D3DXVec3Cross(&forward, &normal, &right);
    
    D3DXMatrixIdentity(&m_mLocal);

    m_mLocal._11 = right.x;
    m_mLocal._21 = normal.x;
    m_mLocal._31 = forward.x;

    m_mLocal._12 = right.y;
    m_mLocal._22 = normal.y;
    m_mLocal._32 = forward.y;

    m_mLocal._13 = right.z;
    m_mLocal._23 = normal.z;
    m_mLocal._33 = forward.z;
    
    D3DXMATRIX scale;
    D3DXMatrixIdentity(&scale);
    D3DXMatrixScaling(&scale, UI3DObjectManager::globalScale, UI3DObjectManager::globalScale, UI3DObjectManager::globalScale);
    D3DXMatrixMultiply(&m_mLocal, &m_mLocal, &scale);

    m_mLocal._41 = m_pUnit->getPosition()->x + m_HalfSize;
    m_mLocal._42 = m_pUnit->getPosition()->y + m_HalfSize;
    m_mLocal._43 = UITerrain::getInstance()->calculateTriangleHeightAt(m_mLocal._41, m_mLocal._42);

}

void UIUnit::handleAssetStateChange(IAsset* pAsset, IAsset::State newState)
{
    //TODO: if needed
}