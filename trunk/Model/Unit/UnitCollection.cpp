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
    m_Head = m_Tail = NULL;
}


UnitCollection::~UnitCollection(void)
{
    releaseAll();
}


Unit* UnitCollection::getUnitAt(unsigned short x, unsigned short y)
{
    return NULL; // TODO: not implemented
}


void UnitCollection::registerUnit(Unit* unit)
{
    UNITNODE* temp;
    if(m_Head) {
        temp = m_Head;
        m_Head = new UNITNODE();
        m_Head->unit = unit;
        m_Head->prev = temp;
        m_Head->next = NULL;
    }
    else {
        // this is the first node
        m_Head = new UNITNODE();
        m_Head->unit = unit;
        m_Head->next = NULL;
        m_Head->prev = NULL;
        m_Tail = m_Head;
    }
    // TODO: notify addition
}


bool UnitCollection::releaseUnit(Unit* unit)
{
    UNITNODE* node = m_Tail;
    // find unit
    while(node) {
        if(node->unit == unit) {
            // fix links
            node->prev->next = node->next;
            node->next->prev = node->prev;
            delete node->unit;
            delete node;
            return true;
        }
        node = node->next;
    }
    // unit not found in list.. delete anyway?
    delete unit;
    return false; // should return true cause unit was deleted?
}


char UnitCollection::updateAll(const float deltaT)
{
    UNITNODE* node = m_Tail;
    char response;
    while(node)
    {
        // update and check for deletion
        response = node->unit->update(deltaT);
        if(response == IUpdatable::RESPONSE_DESTROYME)
        {
            // unit object is to be deleted
            UNITNODE* toBeGone;
            // link previous to next
            toBeGone = node;
            node = toBeGone->next;
            toBeGone->prev->next = toBeGone->next;
            toBeGone->next->prev = toBeGone->prev;
            delete toBeGone->unit;
            delete toBeGone;
            // TODO: notify remove
        }
        else
        {
            // advance to next
            node = node->next;
        }
    }
    return IUpdatable::RESPONSE_OK;
}


void UnitCollection::releaseAll(void)
{
    UNITNODE* node = m_Tail;
    UNITNODE* temp = NULL;
    while(node) {
        temp = node;
        node = temp->next;
        delete temp->unit;
        delete temp;
    }
    m_Head = NULL;
    m_Tail = NULL;
}


void UnitCollection::handlePositionChanged(Unit* unit, unsigned short oldPosX, unsigned short oldPosY)
{
    // TODO: implement some magic fast position-based data structure
    // Notes for implementation:
    // oldPosX and oldPosY indicate where the unit was before it's position changed
    // obviously the new position can be queried from the unit itself
}
