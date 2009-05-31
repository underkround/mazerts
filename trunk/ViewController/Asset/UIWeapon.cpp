#include "UIWeapon.h"

#include "UI3DObjectManager.h"
#include "../App/I3DObject.h"
#include <math.h>
#include "ParticleFactory.h"
#include "../Sound/SoundManager.h"
#include "../../Model/Weapon/Projectile.h"

UIWeapon::UIWeapon(IWeapon* pWeapon)
{
    m_pWeapon = pWeapon;
    m_pBarrel = NULL;
    m_Alive = true;        

    PDIRECT3DTEXTURE9 pTexture = UI3DObjectManager::getInstance()->getResourceContainer()->FindTexture(g_ppTextureNames[BALLTEXTURE]); 

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

    if(damage > 15 && damage < 1000)
    {
        m_EmitParams.fSize = damage * 0.033f;
        m_EmitParams.fSizeSpread = damage * 0.020f;
    }
    else if(damage > 999)
    {
        m_EmitParams.fSize = 30.0f;
        m_EmitParams.fSizeSpread = 1.50f;
    }
    else
    {
        m_EmitParams.fSize = 0.5f;
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

    float pitch = m_pWeapon->getBarrelPitch();

    //Pitch barrel
    if(m_OldPitch != pitch)
    {
        m_OldPitch = pitch;
        
        D3DXMATRIX& matr = m_pBarrel->GetMatrix();

        //Rotation zeroes location
        float x = matr._41;
        float y = matr._42;
        float z = matr._43;        
        D3DXMatrixRotationX(&matr, pitch);
        matr._41 = x;
        matr._42 = y;
        matr._43 = z;
    }

    return m_Alive;
}

void UIWeapon::callBack()
{
    if(m_pBarrel)
    {
        m_pFireEmitter->Emit(m_EmitParams);

        // Dirty hack
        switch (m_pWeapon->getHost()->getTypeTag())
        {
        case BUILDING_TYPE_SILO:
        case UNIT_TYPE_LAUNCHER:
        case UNIT_TYPE_NUKER:
            SoundManager::playSound(SOUND_MISSILE, 0.1f, *((D3DXVECTOR3*)&GetWorldMatrix()._41), true);
            break;
        case UNIT_TYPE_CAR:
            SoundManager::playSound(SOUND_GUN, 0.1f, *((D3DXVECTOR3*)&GetWorldMatrix()._41), true);
            break;
        case UNIT_TYPE_SUPER:
        case UNIT_TYPE_TANK:
        default:
            SoundManager::playSound(SOUND_CANNON, 0.1f, *((D3DXVECTOR3*)&GetWorldMatrix()._41), true);
            break;
        }
    }
}