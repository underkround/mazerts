/**
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "UnitCollection.h"
#include "Unit.h"

UnitCollection::UnitCollection(void)
{

}


UnitCollection::~UnitCollection(void)
{
}


Unit* UnitCollection::getUnitAt(unsigned short x, unsigned short y)
{
    return NULL; // TODO: not implemented
}


void UnitCollection::registerUnit(Unit* unit)
{
}


void UnitCollection::releaseUnit(Unit* unit)
{
}


char UnitCollection::updateAll(const float deltaT)
{
    std::vector<Unit*>::iterator it;
    Unit* u;
    for(it = m_Units.begin(); it != m_Units.end(); it++ )
    {
        u = *it;
        u->update(deltaT); // .update(deltaT);
    }
    return 0;
}


void UnitCollection::releaseAll(void)
{
    for(unsigned int i=0; i < m_Units.size(); i++)
        delete m_Units[i];
}


void UnitCollection::notifyNewPosition(Unit* unit, unsigned short oldPosX, unsigned short oldPosY)
{
}
