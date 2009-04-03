#include "UIWeapon.h"

#include "UI3DObjectManager.h"
#include "../App/I3DObject.h"
#include <math.h>
#include "ParticleFactory.h"
#include "../Sound/SoundManager.h"


UIWeapon::UIWeapon(IWeapon* pWeapon)
{
    m_pWeapon = pWeapon;
    m_pBarrel = NULL;
    m_Alive = true;        

    PDIRECT3DTEXTURE9 pTexture = UI3DObjectManager::getInstance()->getResourceContainer()->FindTexture(g_pTextureNames[BALLTEXTURE]); 

    m_pFireEmitter = new C3DParticleEmitter(0);
    m_pFireEmitter->Create(1, &pTexture, C3DParticleEmitter::ePARTICLETYPE_BILLBOARD);            

    ::memset(&m_EmitParams, 0, sizeof(C3DParticleEmitter::EMIT_PARAMS));

    // position
    m_EmitParams.vPosition.x = 0;
    m_EmitParams.vPosition.y = 0;
    m_EmitParams.vPosition.z = -g_pUnitBarrelLength[pWeapon->getHost()->getTypeTag() - 1];
    m_EmitParams.vPositionSpread = D3DXVECTOR3(0, 0, 0);

    // direction
    m_EmitParams.vDirection = D3DXVECTOR3(0.0f, 0.0f, -5.0f);
    m_EmitParams.vDirectionSpread = D3DXVECTOR3(0.75f, 0.75f, 4.0f);

    // color
    m_EmitParams.cColor = D3DXCOLOR(0.9f, 0.75f, 0.0f, 1.0f);
    m_EmitParams.cColorSpread = D3DXCOLOR(0.1f, 0.5f, 0.0f, 0.0f);

    float rof = pWeapon->getDef()->rof;

    if(rof > 0.1f)
    {
        m_EmitParams.fLifeTime = rof * 0.75f;
        m_EmitParams.fLifeTimeSpread =  rof * 0.25f;
    }
    else
    {
        m_EmitParams.fLifeTime = 0.075f;
        m_EmitParams.fLifeTimeSpread =  0.020f;
    }

    int damage = pWeapon->getDef()->damage;

    if(damage > 10)
    {
        m_EmitParams.fSize = damage * 0.033f;
        m_EmitParams.fSizeSpread = damage * 0.020f;
    }
    else
    {
        m_EmitParams.fSize = 0.33f;
        m_EmitParams.fSizeSpread = 0.20f;
    }

    m_EmitParams.fWeight = 10.0f;
    m_EmitParams.fWeightSpread = 3.0f;

    m_EmitParams.dwCount = 10;

    m_pFireEmitter->SetFade(TRUE);    
    m_pFireEmitter->SetScale(TRUE);
    
}


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
        /*D3DXVECTOR3* pPos = (D3DXVECTOR3*)(&m_pBarrel->GetWorldMatrix()._41);
        D3DXVECTOR3* pDir = (D3DXVECTOR3*)(&m_pBarrel->GetWorldMatrix()._31);        
        pPos->x -= pDir->x * g_pUnitBarrelLength[m_Tag];
        pPos->z += pDir->z * g_pUnitBarrelLength[m_Tag];
        pPos->y -= pDir->y * g_pUnitBarrelLength[m_Tag];
        *pDir *= -20.0f;
        ParticleFactory::createFlame(*pPos, *pDir, 0.1f);*/
        m_pFireEmitter->Emit(m_EmitParams);

        SoundManager::playSound(SOUND_SHOOT, 0.1f, *((D3DXVECTOR3*)&GetMatrix()._41), true);
    }
}