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
    UIBuilding(Building* pBuilding);

    /**
     * Set base material
     * Used for player colors
     */
    virtual void setBaseMaterial(D3DMATERIAL9* pBaseMaterial, D3DMATERIAL9* pDisabledMaterial);

    void handleAssetStateChange(IAsset* pAsset, IAsset::State newState);

    virtual ~UIBuilding();

private:

    /**
     * Model-side building this object represents
     */
    Building* m_pBuilding;

    bool            m_Powered; // just a flag to remember whether we are disabled or not
    D3DMATERIAL9*   m_pDisabledMaterial; // pointer to "disabled" -material
    D3DMATERIAL9**  m_ppOriginalMaterials;

};

#endif //__UIBUILDING_H__