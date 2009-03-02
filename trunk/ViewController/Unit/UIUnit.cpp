#include "UIUnit.h"
#include "../Terrain/UITerrain.h"
UIUnit::UIUnit()
{
}


UIUnit::~UIUnit()
{
}

void UIUnit::alignToTerrain()
{
    //Terrain normal
    /*D3DXVECTOR3 normal = UITerrain::getInstance()->getNormalAt(m_UnitX, m_UnitY, m_UnitSize, m_UnitSize);

    //Get "right"-vector from crossproduct of direction (around z-axis) and normal
    D3DXVECTOR3 right;
    D3DXVec3Normalize(&m_Direction, &m_Direction);
    D3DXVec3Cross(&right, &normal, &m_Direction);
    //D3DXVec3Normalize(&right, &right);

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

    m_mLocal._41 = m_UnitX + m_HalfSize;
    m_mLocal._42 = m_UnitY + m_HalfSize;
    m_mLocal._43 = terrain.calculateTriangleHeightAt(m_mLocal._41, m_mLocal._42);
*/
}