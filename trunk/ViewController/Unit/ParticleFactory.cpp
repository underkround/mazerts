#include "ParticleFactory.h"
#include "MeshFileNames.h"
#include "../Terrain/UITerrain.h"

LPDIRECT3DDEVICE9 ParticleFactory::pDevice = NULL;
UI3DObjectManager* ParticleFactory::pManager = NULL;

void ParticleFactory::createExplosion(Explosion* pExplosion)
{
    LPDIRECT3DTEXTURE9 pTexture = pManager->getResourceContainer()->FindTexture(g_pTextureNames[BALLTEXTURE]); 

    C3DParticleEmitter* pEmitter = new C3DParticleEmitter(1.0f);
    pEmitter->Create(1, &pTexture, C3DParticleEmitter::ePARTICLETYPE_BILLBOARD);    

    C3DParticleEmitter::EMIT_PARAMS params;
	::memset(&params, 0, sizeof(C3DParticleEmitter::EMIT_PARAMS));

	// position
    params.vPosition.x = pExplosion->getX();
	params.vPosition.y = pExplosion->getY();
    params.vPosition.z = UITerrain::getInstance()->calculateTriangleHeightAt(pExplosion->getX(), pExplosion->getY()) - 1.0f;
    params.vPositionSpread = D3DXVECTOR3(pExplosion->getRadius() - 1, pExplosion->getRadius() - 1, 0);

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

    params.dwCount = 50 * pExplosion->getRadius();

    pEmitter->SetFade(TRUE);    
    pEmitter->SetScale(TRUE);
    pEmitter->Emit(params);

    pManager->getRootObject()->AddChild(pEmitter);
}