#pragma once

#include <d3dx9.h>
#include "../App/Vertices.h"
#include "../App/I3DObject.h"
#include "../App/C3DObject.h"

class HealthBar : public C3DObject //, IAssetListener
{
public:
    HealthBar(void);
    virtual ~HealthBar(void);

    virtual HRESULT Create(LPDIRECT3DDEVICE9 pDevice);

    virtual void Release();

    /**
     * Update the health bar
     * @param fFrametime Time elapsed in frame as seconds
     * @return true, if no action should take place, false
     *         if the child should be removed by parent
     */
    virtual bool Update(float fFrametime);

    /**
     * Render the health bar
     * @param pDevice pointer to Direct3D-device to use
     */
    virtual void Render(LPDIRECT3DDEVICE9 pDevice);

private:

    VERTEX      			m_pVertices[4];
    LPDIRECT3DVERTEXBUFFER9 m_pVertBuf;

    //health bar's actual location on screen
    D3DXVECTOR2             m_Position;

};
