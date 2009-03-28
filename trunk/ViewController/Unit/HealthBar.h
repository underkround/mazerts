#pragma once

#include <d3dx9.h>
#include "../App/Vertices.h"
#include "../App/C3DObject.h"

class HealthBar : public C3DObject //, IAssetListener
{
public:
    HealthBar(void);
    virtual ~HealthBar(void);

    HRESULT create(LPDIRECT3DDEVICE9 pDevice);

    void release();

    /**
     * Update the health bar
     * @param fFrametime Time elapsed in frame as seconds
     * @return False, if no action should take place, true
     *         if the child should be removed by parent
     */
    virtual bool Update(float fFrametime);

    /**
     * Render the health bar
     * @param pDevice pointer to Direct3D-device to use
     */
    virtual void Render(LPDIRECT3DDEVICE9 pDevice);

private:

    LPDIRECT3DVERTEXBUFFER9 m_pVertBuf;

};
