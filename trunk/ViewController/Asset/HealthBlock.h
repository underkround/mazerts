#pragma once
#include <d3dx9.h>
#include <tchar.h>
#include "../App/C3DObject.h"
#include "../App/C3DResourceContainer.h"
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
    HealthBlock(UIAsset* pAsset, float yOffset, C3DResourceContainer* pContainer);
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

    /**
     * Creates (if needed) and returns mesh for health block
     */
    LPD3DXMESH getHealthBlockMesh(C3DResourceContainer* pContainer, const float radius);

private:
    IAsset*         m_pAsset; // what's being monitored
};
