#include "ParticleFactory.h"
#include "MeshFileNames.h"
#include "../Terrain/UITerrain.h"

LPDIRECT3DDEVICE9 ParticleFactory::pDevice = NULL;
UI3DObjectManager* ParticleFactory::pManager = NULL;

void ParticleFactory::createExplosion(Explosion* pExplosion)
{
    //TODO: Should textures be cached somewhere? (FindTexture iterates through the vector EVERY time a unit fires, an explosion occurs etc...)
    LPDIRECT3DTEXTURE9 pTexture = pManager->getResourceContainer()->FindTexture(g_pTextureNames[BALLTEXTURE]); 

    C3DParticleEmitter* pEmitter = new C3DParticleEmitter(1.0f);
    pEmitter->Create(1, &pTexture, C3DParticleEmitter::ePARTICLETYPE_BILLBOARD);    

    C3DParticleEmitter::EMIT_PARAMS params;
	::memset(&params, 0, sizeof(C3DParticleEmitter::EMIT_PARAMS));

	// position
    params.vPosition.x = pExplosion->getX();
	params.vPosition.y = pExplosion->getY();
    params.vPosition.z = UITerrain::getInstance()->calculateTriangleHeightAt(pExplosion->getX(), pExplosion->getY()) - 1.0f;
    params.vPositionSpread = D3DXVECTOR3((FLOAT)pExplosion->getRadius(), (FLOAT)pExplosion->getRadius(), 0);

	// direction
	params.vDirection = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	params.vDirectionSpread = D3DXVECTOR3(2.0f, 2.0f, 1.0f);

	// color
	params.cColor = D3DXCOLOR(0.75f, 0.5f, 0.0f, 1.0f);
	params.cColorSpread = D3DXCOLOR(0.25f, 0.5f, 0.0f, 0.0f);

	params.fLifeTime = 0.75f;
	params.fLifeTimeSpread = 0.25f;

	params.fSize = 1.0f;
	params.fSizeSpread = 0.25f;

	params.fWeight = 10.0f;
	params.fWeightSpread = 3.0f;

    params.dwCount = 50 * (pExplosion->getRadius() + 1);

    pEmitter->SetFade(TRUE);    
    pEmitter->SetScale(TRUE);
    pEmitter->Emit(params);

    pManager->getRootObject()->AddChild(pEmitter);
}

void ParticleFactory::createExplosion(const D3DXVECTOR3& pos, int size)
{
    //TODO: Should textures be cached somewhere? (FindTexture iterates through the vector EVERY time a unit fires, an explosion occurs etc...)
    LPDIRECT3DTEXTURE9 pTexture = pManager->getResourceContainer()->FindTexture(g_pTextureNames[BALLTEXTURE]); 

    C3DParticleEmitter* pEmitter = new C3DParticleEmitter(1.0f);
    pEmitter->Create(1, &pTexture, C3DParticleEmitter::ePARTICLETYPE_BILLBOARD);    

    C3DParticleEmitter::EMIT_PARAMS params;
	::memset(&params, 0, sizeof(C3DParticleEmitter::EMIT_PARAMS));

	// position
    params.vPosition.x = pos.x;
	params.vPosition.y = pos.y;
    params.vPosition.z = pos.z;
    params.vPositionSpread = D3DXVECTOR3(0, 0, 0);

	// direction
	params.vDirection = D3DXVECTOR3(0.0f, 0.0f, -5.0f);
	params.vDirectionSpread = D3DXVECTOR3(10.0f, 10.0f, 5.0f);

	// color
	params.cColor = D3DXCOLOR(0.9f, 0.75f, 0.0f, 1.0f);
	params.cColorSpread = D3DXCOLOR(0.1f, 0.5f, 0.0f, 0.0f);

	params.fLifeTime = 0.5f;
	params.fLifeTimeSpread = 0.25f;

	params.fSize = 3.0f;
	params.fSizeSpread = 1.0f;

	params.fWeight = 10.0f;
	params.fWeightSpread = 3.0f;

    params.dwCount = 20 * size;

    pEmitter->SetFade(TRUE);    
    pEmitter->SetScale(TRUE);
    pEmitter->Emit(params);

    pManager->getRootObject()->AddChild(pEmitter);
}

void ParticleFactory::createFlame(const D3DXVECTOR3& pos, const D3DXVECTOR3& dir, float lifeTime)
{
    LPDIRECT3DTEXTURE9 pTexture = pManager->getResourceContainer()->FindTexture(g_pTextureNames[BALLTEXTURE]); 

    C3DParticleEmitter* pEmitter = new C3DParticleEmitter(lifeTime);
    pEmitter->Create(1, &pTexture, C3DParticleEmitter::ePARTICLETYPE_BILLBOARD);

    C3DParticleEmitter::EMIT_PARAMS params;
	::memset(&params, 0, sizeof(C3DParticleEmitter::EMIT_PARAMS));

	// position
    params.vPosition.x = pos.x;
	params.vPosition.y = pos.y;
    params.vPosition.z = pos.z;
    params.vPositionSpread = D3DXVECTOR3(0, 0, 0);

	// direction
	params.vDirection = dir;    
	params.vDirectionSpread = D3DXVECTOR3(dir.x * 0.5f, dir.y * 0.5f, dir.z * 0.5f);

	// color
	params.cColor = D3DXCOLOR(0.75f, 0.5f, 0.0f, 1.0f);
	params.cColorSpread = D3DXCOLOR(0.25f, 0.5f, 0.0f, 0.0f);

	params.fLifeTime = lifeTime;
	params.fLifeTimeSpread = lifeTime * 0.3f;

	params.fSize = 0.5f;
	params.fSizeSpread = 0.25f;

	params.fWeight = 10.0f;
	params.fWeightSpread = 3.0f;

    params.dwCount = 10;

    pEmitter->SetFade(TRUE);    
    //pEmitter->SetScale(TRUE);
    pEmitter->Emit(params);

    pManager->getRootObject()->AddChild(pEmitter);    
}