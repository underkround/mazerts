/*
 * 3D-representation of model-side building
 *
 * $Revision: 217 $
 * $Date: 2009-03-26 17:14:33 +0200 (to, 26 maalis 2009) $
 * $Id: Building.h 217 2009-03-26 15:14:33Z zem $
 */

#ifndef __UIBUILDING_H__
#define __UIBUILDING_H__

#include "UIAsset.h"
#include "../Terrain/UITerrain.h"
#include "../../Model/Asset/Building.h"

#include <d3dx9.h>

class UIBuilding : public UIAsset
{
public:
    /**
     * Constructor
     * @param pBuilding Pointer to model-side building this
     *       object represents
     */
    UIBuilding(Building* pBuilding) : UIAsset(pBuilding)
    {
        m_pBuilding = pBuilding;

        D3DXMatrixRotationX(&m_mLocal, D3DX_PI * -0.5f);

        //Update position
        m_mLocal._41 = pBuilding->getGridX() + (pBuilding->getWidth() / 2.0f);
        m_mLocal._42 = pBuilding->getGridY() + (pBuilding->getHeight() / 2.0f);
        //TODO: Should height be got from model-size?
        m_mLocal._43 = UITerrain::getInstance()->calculateTriangleHeightAt(m_mLocal._41, m_mLocal._42);
    }

    virtual ~UIBuilding() {}

private:

    /**
     * Model-side building this object represents
     */
    Building* m_pBuilding;

};

#endif //__UIBUILDING_H__