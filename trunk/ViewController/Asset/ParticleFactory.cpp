#include "ParticleFactory.h"
#include "MeshFileNames.h"
#include "../Terrain/UITerrain.h"
#include "../App/IApplication.h"

LPDIRECT3DDEVICE9 ParticleFactory::pDevice = NULL;
UI3DObjectManager* ParticleFactory::pManager = NULL;

void ParticleFactory::createExplosion(Explosion* pExplosion)
{
    if (!UITerrain::getInstance()->getCurrentPlayer()->getFog()->isVisibleCoord(pExplosion->getX(), pExplosion->getY())) 
    {
        // explosion is not visible, so don't fire up the emitter
        return;
    }

    //TODO: Should textures be cached somewhere? (FindTexture iterates through the vector EVERY time a unit fires, an explosion occurs etc...)
    LPDIRECT3DTEXTURE9 pTexture = pManager->getResourceContainer()->FindTexture(g_ppTextureNames[BALLTEXTURE]); 

    C3DParticleEmitter* pEmitter = new C3DParticleEmitter(1.0f);
    pEmitter->Create(1, &pTexture, C3DParticleEmitter::ePARTICLETYPE_BILLBOARD);    

    D3DXMATRIX& mat = pEmitter->GetWorldMatrix();
    mat._41 = pExplosion->getX();
    mat._42 = pExplosion->getY();
    mat._43 = UITerrain::getInstance()->calculateTriangleHeightAt(pExplosion->getX(), pExplosion->getY());
    ::memcpy(&pEmitter->GetMatrix()._41, &mat._41, 3 * sizeof(float));

    C3DParticleEmitter::EMIT_PARAMS params;
	::memset(&params, 0, sizeof(C3DParticleEmitter::EMIT_PARAMS));

	// position
    params.vPosition.x = IApplication::RandFloat(-1.0f, 1.0f);
	params.vPosition.y = IApplication::RandFloat(-1.0f, 1.0f);
    params.vPosition.z = -1.0f;
    params.vPositionSpread = D3DXVECTOR3((FLOAT)pExplosion->getRadius(), (FLOAT)pExplosion->getRadius(), 0);

	// direction
	params.vDirection = D3DXVECTOR3(0.0f, 0.0f, -3.0f);
	params.vDirectionSpread = D3DXVECTOR3(4.0f, 4.0f, 1.0f);

	// color
	params.cColor = D3DXCOLOR(0.75f, 0.5f, 0.0f, 1.0f);
	params.cColorSpread = D3DXCOLOR(0.25f, 0.5f, 0.0f, 0.0f);

	params.fLifeTime = 0.75f;
	params.fLifeTimeSpread = 0.25f;

	params.fSize = 1.0f;
	params.fSizeSpread = 0.25f;

	params.fWeight = 10.0f;
	params.fWeightSpread = 3.0f;

    params.dwCount = (int)(pExplosion->getDamage()) >> 1;

    pEmitter->SetFade(TRUE);    
    pEmitter->SetScale(TRUE);
    pEmitter->Emit(params);

    pManager->getRootObject()->AddChild(pEmitter);
}

void ParticleFactory::createExplosion(const D3DXVECTOR3& pos, int size)
{
    if (!UITerrain::getInstance()->getCurrentPlayer()->getFog()->isVisibleCoord((int)pos.x, (int)pos.y)) 
    {
        // explosion is not visible, so don't fire up the emitter
        return;
    }

    //TODO: Should textures be cached somewhere? (FindTexture iterates through the vector EVERY time a unit fires, an explosion occurs etc...)
    LPDIRECT3DTEXTURE9 pTexture = pManager->getResourceContainer()->FindTexture(g_ppTextureNames[BALLTEXTURE]); 

    C3DParticleEmitter* pEmitter = new C3DParticleEmitter(1.0f);
    pEmitter->Create(1, &pTexture, C3DParticleEmitter::ePARTICLETYPE_BILLBOARD);    

    D3DXMATRIX& mat = pEmitter->GetWorldMatrix();
    mat._41 = pos.x;
    mat._42 = pos.y;
    mat._43 = pos.z;
    ::memcpy(&pEmitter->GetMatrix()._41, &mat._41, 3 * sizeof(float));

    C3DParticleEmitter::EMIT_PARAMS params;
	::memset(&params, 0, sizeof(C3DParticleEmitter::EMIT_PARAMS));

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
    if (!UITerrain::getInstance()->getCurrentPlayer()->getFog()->isVisibleCoord((int)pos.x, (int)pos.y)) 
    {
        // flame is not visible, so don't fire up the emitter
        return;
    }

    LPDIRECT3DTEXTURE9 pTexture = pManager->getResourceContainer()->FindTexture(g_ppTextureNames[BALLTEXTURE]); 

    C3DParticleEmitter* pEmitter = new C3DParticleEmitter(lifeTime);
    pEmitter->Create(1, &pTexture, C3DParticleEmitter::ePARTICLETYPE_BILLBOARD);

    D3DXMATRIX& mat = pEmitter->GetWorldMatrix();
    mat._41 = pos.x;
    mat._42 = pos.y;
    mat._43 = pos.z;
    ::memcpy(&pEmitter->GetMatrix()._41, &mat._41, 3 * sizeof(float));

    C3DParticleEmitter::EMIT_PARAMS params;
	::memset(&params, 0, sizeof(C3DParticleEmitter::EMIT_PARAMS));

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