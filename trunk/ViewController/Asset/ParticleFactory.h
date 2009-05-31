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

    /**
     * Create an explosion at given position with given size
     * @param pos Reference to D3DXVECTOR3 which contains the position
     * @param size Explosion size
     */
    static void createExplosion(const D3DXVECTOR3& pos, int size);

    /**
     * Small directional flame for muzzle flashes etc
     * @param pos Reference to D3DXVECTOR3 which contains the position
     * @param dir Reference to D3DXVECTOR3 which contains the direction
     * @param lifeTime Lifetime of emitter in seconds
     */
    static void createFlame(const D3DXVECTOR3& pos, const D3DXVECTOR3& dir, float lifeTime);
    static C3DParticleEmitter* getFlame(const D3DXVECTOR3& pos, const D3DXVECTOR3& dir, float lifeTime);

private:

    static LPDIRECT3DDEVICE9 pDevice;
    static UI3DObjectManager* pManager;
};

#endif //__PARTICLEFACTORY_H__