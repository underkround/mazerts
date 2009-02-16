#include "Unit.h"

#include "../Terrain/Terrain.h"

Unit::Unit(void)
{
    // by default set unit size to 1*1 cell
    m_Width = 1;
    m_Height = 1;
    m_ComponentCount = 0;
    m_ComponentArraySize = DEFAULT_COMPONENT_ARRAY_SIZE;
    m_Components = new IComponent*[DEFAULT_COMPONENT_ARRAY_SIZE];
    for(int i=0; i<DEFAULT_COMPONENT_ARRAY_SIZE; i++)
        m_Components[i] = NULL;

    // weather to keep the unit's position vector in sync with the terrain
    // height at the unit's current position
    m_SyncHeightToTerrain = true; // for ground units (for now, that is)

    // TODO: notify the collection
}


Unit::~Unit(void)
{
    // TODO: release and delete components
    if(m_ComponentCount)
    {
        for(int i=0; i<m_ComponentCount; i++)
        {
            if(m_Components[i]->detach(this))
            {
                delete m_Components[i];
            }
        }
        delete [] m_Components;
    }
    // TODO: notify the collection
}


char Unit::update(const float deltaT)
{
    // dispose update to components
    for(int i=0; i<m_ComponentCount; i++)
    {
        m_Components[i]->update(deltaT);
    }
    // @TODO: should another member position vector be used to store previous
    // position if the new position is illegal, or should it be the concern of
    // the moving logic only?

    // check for changes in the position
    if( (m_CellX != (int)m_Position.x) || (m_CellY != (int)m_Position.y) )
    {
        // @TODO check if the move was legal
        // update cell coordinates
        // notify the collection
        // @TODO when the component container is implemented
        // check the height
        // @TODO
        //Terrain::getInstance()->setToHeight(m_Position);
    }
    // TODO: add flag to terrain for weather the terrain has changed
    // and update the unit's height
    //Terrain::getInstance()->setToHeight(m_Position);
    return 0;
}


bool Unit::forcePosition(unsigned const short x, unsigned const short y)
{
    if(!Terrain::getInstance()->isPassable(x, y))
        return false; // invalid target
    m_CellX = x;
    m_CellY = y;
    m_Position.x = (float)x;
    m_Position.y = (float)y;
    // notify the collection
    // @TODO when unit collection implemented
    return true;
}


bool Unit::addComponent(IComponent* component)
{
    if(m_ComponentCount >= m_ComponentArraySize)
        increaseComponentCapacity();
    component->attach(this);
    m_Components[m_ComponentCount] = component;
    ++m_ComponentCount;
    return true;
}