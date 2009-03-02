/**
 * UIUnit
 *
 * Handles the ui 3d-representation of a single unit
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __UIUNIT_H__
#define __UIUNIT_H__

#include <d3dx9.h>
#include "../App/C3DObject.h"
#include "../../Model/Asset/Unit.h"

class UIUnit : public C3DObject
{
public:
    /**
     * Constructor
     */
    UIUnit();

    /**
     * Destructor
     */
    virtual ~UIUnit();


protected:
    
    /**
     * Aligns the model to the terrain below it
     */
    void alignToTerrain();

    /**
     * Pointer to Model-side Unit-instance this is a representation of
     */
    Unit* m_pUnit;

    /**
     * Half of the actual unit "grid-size", used to offset the model correctly
     */
    float m_HalfSize;
};

#endif //__UIUNIT_H__