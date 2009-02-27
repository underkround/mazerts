/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "IAsset.h"
#include "AssetCollection.h"

class Building : public IAsset
{
public:

    Building(int buildingType = 0);
    virtual ~Building();

// =====

    virtual void create();

    virtual void release();

    virtual char update(const float deltaT);

private:

    int         m_BuildingType;

};

#endif // __BUILDING_H__
