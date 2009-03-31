#pragma once
#include <d3dx9.h>
#include <tchar.h>
#include "../App/C3DObject.h"
#include "../../Model/Asset/IAsset.h"

class UIAsset; // forward declaration

static const TCHAR HEALTHBLOCK_MESHNAME[256] = _T("HealthBlockMesh");

class HealthBlock : public C3DObject
{
public:
    /**
     * Constructor
     * Tells UIUnit to set this to healthblock & add this as child.
     * @param pAsset    what asset's health this is supposed to monitor
     * @param yOffset   what height is this situated
     */
    HealthBlock(UIAsset* pAsset, float yOffset);
    virtual ~HealthBlock(void);

    /**
     * Create
     * Creates material (in addition to C3DObject stuff)
     */
    virtual void Create(LPD3DXMESH pMesh);

    /**
     * Releases material
     */
    virtual void Release();

    /**
     * Updates the health block (calculates new color, if visible)
     * @param fFrametime Time elapsed in frame as seconds
     * @return true all the time :P
     */
    virtual bool Update(float fFrametime);

    /**
     * Render
     * Currently only calls parent's render
     */
    virtual void Render(LPDIRECT3DDEVICE9 pDevice);

private:
    IAsset*         m_pAsset; // what's being monitored
};
