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
     * @param pUnit Pointer to Model-side unit this UIUnit represents
     */
    UIUnit(Unit* pUnit)
    {
        m_pUnit = pUnit;
        m_HalfSize = pUnit->getWidth() * 0.5f;
    }

    /**
     * Update the unit
     * @param fFrametime Time elapsed in frame as seconds
     */
    virtual void Update(float fFrametime);

    /**
     * Destructor
     */
    virtual ~UIUnit()
    {
    }


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

    static float Scale;
};

#endif //__UIUNIT_H__