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

    Building(AssetDef& def);
    virtual ~Building();

// =====

    virtual void create();

    virtual void release();

    virtual char update(const float deltaT);

    void setPower(bool active);

private:

};

#endif // __BUILDING_H__
