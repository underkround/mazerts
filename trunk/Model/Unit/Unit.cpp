/**
 * $Revision$
 * $Date$
 * $Id$
 */

#include "Unit.h"

#include "../Terrain/Terrain.h"

Unit::Unit(int typeId)
{
    m_State = STATE_BEING_BUILD;
    m_TypeId = typeId;

    m_Level = 1;

    // by default set unit size to 1*1 cell
    m_Width = 1;
    m_Height = 1;

    m_ComponentCount = 0;
    m_ComponentArraySize = DEFAULT_COMPONENT_ARRAY_SIZE;
    m_Components = new IComponent*[DEFAULT_COMPONENT_ARRAY_SIZE];
    for(int i=0; i<DEFAULT_COMPONENT_ARRAY_SIZE; i++)
        m_Components[i] = NULL;

    m_DestroyTimer = 1000;
    m_ParalyzeTimer = 0; // set when the paralization happens

    m_CanMove = false; // enabled by moving logic, if any
    m_CanFire = false; // enabled by weapon component, if any
    m_IsGroundUnit = true; // weather to sync the position z to terrain height

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
    switch(m_State) {

        case STATE_PARALYZED:
            m_ParalyzeTimer -= deltaT;
            if(m_ParalyzeTimer <= 0)
                m_State = STATE_ACTIVE;
            return 0;
            break;

        case STATE_DESTROYED:
            m_DestroyTimer -= deltaT;
            if(m_DestroyTimer <= 0)
                return RESPONSE_DESTROYME;
            return RESPONSE_OK;
            break;

        case STATE_BEING_BUILD:
            return STATE_ACTIVE;
            break;

        default:
        case STATE_ACTIVE:
            break;
    }

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
    if(!Terrain::getInstance()->isPassable(x, y, m_Width))
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


void Unit::increaseComponentCapacity()
{
    IComponent** tmpHolder = new IComponent*[m_ComponentCount];
    // this could possibly be done with some fancy memory copy -function
    // but i'm the lame java-guy!
    for(int i=0; i<m_ComponentCount; i++)
        tmpHolder[i] = m_Components[i];
    delete [] m_Components;
    m_Components = new IComponent*[m_ComponentCount + DEFAULT_COMPONENT_ARRAY_SIZE];
    for(int i=0; i<m_ComponentCount; i++) {
        if(i < m_ComponentCount)
            m_Components[i] = tmpHolder[i];
        else
            m_Components[i] = NULL;
    }
    delete [] tmpHolder;
}
