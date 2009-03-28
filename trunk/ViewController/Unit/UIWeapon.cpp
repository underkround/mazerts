#include "UIWeapon.h"

#include "UI3DObjectManager.h"
#include "../App/I3DObject.h"
#include <math.h>
#include "ParticleFactory.h"

bool UIWeapon::Update(float fFrameTime) 
{
    I3DObject::Update(fFrameTime);

    D3DXVECTOR3* pDir = (D3DXVECTOR3*)m_pWeapon->getDirection();

    //Check if weapon heading has changed
    if(m_OldHeading.x != pDir->x || m_OldHeading.y != pDir->y)
    {
        //Store heading
        m_OldHeading.x = pDir->x;
        m_OldHeading.y = pDir->y;

        //Get direction as radians
        float dir = atan2(pDir->y, pDir->x);

        //Rotation zeroes location
        float x = m_mLocal._41;
        float y = m_mLocal._42;
        float z = m_mLocal._43;        
        D3DXMatrixRotationY(&m_mLocal, dir);
        m_mLocal._41 = x;
        m_mLocal._42 = y;
        m_mLocal._43 = z;
    }

    return m_Alive;
}

void UIWeapon::callBack()
{
    if(m_pBarrel)
    {
        D3DXVECTOR3* pPos = (D3DXVECTOR3*)(&m_pBarrel->GetWorldMatrix()._41);
        D3DXVECTOR3* pDir = (D3DXVECTOR3*)(&m_pBarrel->GetWorldMatrix()._31);

        //TODO: Where to get barrel-length? (The factor here)
        pPos->x -= pDir->x * 2.0f;
        pPos->z += pDir->z * 2.0f;
        pPos->y -= pDir->y * 2.0f;
        *pDir *= -20.0f;
        ParticleFactory::createFlame(*pPos, *pDir, 0.1f);
    }
}