/**
 * ParticleFactory
 *
 * Factory-class for creating particle-effects
 *
 * $Revision:$
 * $Date:$
 * $Id:$
 */

#ifndef __PARTICLEFACTORY_H__
#define __PARTICLEFACTORY_H__

#include <d3dx9.h>
#include "UI3DObjectManager.h"
#include "../App/C3DParticleEmitter.h"
#include "../../Model/Weapon/Explosion.h"

class ParticleFactory
{
public:

    static void create(LPDIRECT3DDEVICE9 pDevice)
    {
        ParticleFactory::pDevice = pDevice;
        ParticleFactory::pManager = UI3DObjectManager::getInstance();
    }

    /**
     * Create an explosion- particle-emitter based on Explosion-data
     * @param pExplosion Pointer to Explosion
     */
    static void createExplosion(Explosion* pExplosion);

private:

    static LPDIRECT3DDEVICE9 pDevice;
    static UI3DObjectManager* pManager;
};

#endif //__PARTICLEFACTORY_H__