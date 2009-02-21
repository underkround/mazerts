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
    m_Width = m_Height = 1;

    m_ComponentCount = 0;
    m_ComponentArraySize = DEFAULT_COMPONENT_ARRAY_SIZE;
    m_Components = new IComponent*[DEFAULT_COMPONENT_ARRAY_SIZE];
    for(int i=0; i<DEFAULT_COMPONENT_ARRAY_SIZE; i++)
        m_Components[i] = NULL;

    m_DestroyTimer = 1000;
    m_ParalyzeTimer = 0; // set when the paralization happens

    m_CanMove = false; // enabled by moving logic, if any
    m_CanFire = false; // enabled by weapon component, if any
    m_SyncHeightToTerrain = true; // by default this is a ground unit

    m_CellX = m_CellY = m_PreviousCellX = m_PreviousCellY = 0;

    m_Initialized = false;
    // now the user should:
    //  - position the unit
    //  - set the unit's parameters
    //  - add components to the unit
    //  - call it's create()
}


Unit::~Unit(void)
{
    // release and delete components
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
    // we should be here because unitcollection deleted us, so no further
    // actions
}


void Unit::create(void) {
    m_Initialized = true;
    UnitCollection::getInstance()->registerUnit(this);
}


void Unit::release(void) {
    UnitCollection::getInstance()->releaseUnit(this);
}


char Unit::update(const float deltaT)
{
    switch(m_State) {

        case STATE_PARALYZED:
            m_ParalyzeTimer -= deltaT;
            if(m_ParalyzeTimer <= 0)
                m_State = STATE_ACTIVE;
            return RESPONSE_OK;
            break;

        case STATE_DESTROYED:
            m_DestroyTimer -= deltaT;
            if(m_DestroyTimer <= 0)
                return RESPONSE_DESTROYME;
            return RESPONSE_OK;
            break;

        case STATE_BEING_BUILD:
            // TODO: grow our hitpoints until they reach max?
            // or should external source handle the growing?
            // ..or should states be objects and they could
            // handle the growing of the hitpoints?

            // TODO: DEBUG IMPLEMENTATION! can't be used later since this is not time independend
            if(m_Hitpoints >= m_MaxHitpoints)
                m_State = STATE_ACTIVE; // TODO: notification for unit being and entering active state?
            else
                m_Hitpoints++;

            return RESPONSE_OK;
            break;

        default:
        case STATE_ACTIVE:
            break;
    }

    // dispose update to components
    for(int i=0; i<m_ComponentCount; i++)
    {
        m_Components[i]->update(this, deltaT);
    }
    // @TODO: should another member position vector be used to store previous
    // position if the new position is illegal, or should it be the concern of
    // the moving logic only?

    // check for changes in the position
    if( (m_CellX != (int)m_Position.x) || (m_CellY != (int)m_Position.y) )
    {
        // @TODO check if the move was legal?
        // update cell coordinates
        // notify the collection
        UnitCollection::getInstance()->handlePositionChanged(this, m_PreviousCellX, m_PreviousCellY);
        // @TODO when the component container is implemented
        // check the height
        // @TODO
        if(m_SyncHeightToTerrain)
            Terrain::getInstance()->syncToTerrainLevel(&m_Position, 0.0f);
    }
    // TODO: add flag to terrain for weather the terrain has changed
    // and update the unit's height
    //Terrain::getInstance()->setToHeight(m_Position);
    return RESPONSE_OK;
}


bool Unit::forcePosition(unsigned const short x, unsigned const short y)
{
    if(!Terrain::getInstance()->isPassable(x, y, m_Width))
        return false; // invalid target
    m_PreviousCellX = m_CellX;
    m_PreviousCellY = m_CellY;
    m_CellX = x;
    m_CellY = y;
    m_Position.x = (float)x;
    m_Position.y = (float)y;
    // notify the collection
    if(m_Initialized)
        UnitCollection::getInstance()->handlePositionChanged(this, m_PreviousCellX, m_PreviousCellX);
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
