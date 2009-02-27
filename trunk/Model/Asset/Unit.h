/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __UNIT_H__
#define __UNIT_H__

#include "IAsset.h"
//#include "AssetCollection.h"

#include "../Command/Target.h"

class Unit : public IAsset
{
public:

    Unit(int unitType = 0);
    virtual ~Unit();

// =====

    virtual void create();

    virtual void release();

    virtual char update(const float deltaT);

private:

    int         m_UnitType;

};

#endif // __UNIT_H__
