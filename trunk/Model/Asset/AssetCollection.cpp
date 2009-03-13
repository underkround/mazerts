/**
 * @TODO: convert to use DoubleLinkedList class
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "AssetCollection.h"

#include "Unit.h"
#include "Building.h"

#include <string.h> //for memset

DoubleLinkedList<Unit*>     AssetCollection::units      = DoubleLinkedList<Unit*>();
DoubleLinkedList<Building*> AssetCollection::buildings  = DoubleLinkedList<Building*>();

DoubleLinkedList<IAssetCollectionListener*> AssetCollection::listeners = DoubleLinkedList<IAssetCollectionListener*>();

Unit***         AssetCollection::m_pppUnitArray         = NULL;
Building***     AssetCollection::m_pppBuildingArray     = NULL;
unsigned short  AssetCollection::m_MapSize              = 0;

// should not need ctor & dtor ever
AssetCollection::AssetCollection() { }
AssetCollection::~AssetCollection() { releaseAll(); }


// ===== CREATE

void AssetCollection::create(const unsigned int mapSize)
{
    m_MapSize = mapSize;
    // free memory for unit arrays
    m_pppUnitArray = new Unit**[mapSize];
    for(unsigned short i = 0; i < mapSize; i++)
    { 
        m_pppUnitArray[i] = new Unit*[mapSize]; 
        ::memset(m_pppUnitArray[i], 0, sizeof(Unit*) * mapSize);
    }

    m_pppBuildingArray = new Building**[mapSize];
    for(unsigned short i = 0; i < mapSize; i++)
    { 
        m_pppBuildingArray[i] = new Building*[mapSize]; 
        ::memset(m_pppBuildingArray[i], 0, sizeof(Building*) * mapSize);
    }
}

// ===== UPDATE

void AssetCollection::updateBuildings(const float deltaT)
{
    ListNode<Building*>* node = buildings.tailNode();
    ListNode<Building*>* temp;
    while(node)
    {
        if(node->item->update(deltaT) == IAsset::RESULT_DELETEME)
        {
            // store next node to go
            temp = node;
            node = node->prev;
            // delete asset
            notifyAssetReleased(temp->item);
            temp->item->release();
            delete temp->item;
            // delete node
            buildings.remove(temp);
        }
        else
            node = node->prev;
    }
}

void AssetCollection::updateUnits(const float deltaT)
{
    ListNode<Unit*>* node = units.tailNode();
    ListNode<Unit*>* temp;
    while(node)
    {
        if(node->item->update(deltaT) == IAsset::RESULT_DELETEME)
        {
            // store next node to go
            temp = node;
            node = node->prev;
            // delete asset
            notifyAssetReleased(temp->item);
            temp->item->release();
            delete temp->item;
            // delete node
            units.remove(temp);
        }
        else
            node = node->prev;
    }
}

// ===== REGISTERING

void AssetCollection::registerUnit(Unit* u) {
    units.pushHead(u);
    notifyAssetCreated(u);
    for (unsigned short x = 0; x < u->getWidth(); x++)
        for (unsigned short y = 0; y < u->getHeight(); y++)
            m_pppUnitArray[y + (unsigned short)u->getPosition()->y][x + (unsigned short)u->getPosition()->x] = u;
}

void AssetCollection::registerBuilding(Building* b) {
    buildings.pushHead(b);
    notifyAssetCreated(b);
    for (unsigned short x = 0; x < b->getWidth(); x++)
        for (unsigned short y = 0; y < b->getHeight(); y++)
            m_pppBuildingArray[y + (unsigned short)b->getPosition()->y][x + (unsigned short)b->getPosition()->x] = b;
}

// ===== RELEASING

void AssetCollection::releaseUnit(Unit* u)
{
    notifyAssetReleased(u);
    units.remove(u);
    for (unsigned short x = 0; x < u->getWidth(); x++)
        for (unsigned short y = 0; y < u->getHeight(); y++)
            m_pppUnitArray[y + (unsigned short)u->getPosition()->y][x + (unsigned short)u->getPosition()->x] = NULL;
    delete u;
}


void AssetCollection::releaseBuilding(Building* b)
{
    notifyAssetReleased(b);
    buildings.remove(b);
    for (unsigned short x = 0; x < b->getWidth(); x++)
        for (unsigned short y = 0; y < b->getHeight(); y++)
            m_pppBuildingArray[y + (unsigned short)b->getPosition()->y][x + (unsigned short)b->getPosition()->x] = NULL;
    delete b;
}


void AssetCollection::releaseAll()
{
    // release all units
    ListNode<Unit*>* uNode = units.headNode();
    while(uNode)
    {
        notifyAssetReleased(uNode->item);
        uNode->item->release();
        uNode = uNode->next;
    }
    // do the actual deletion of the objects
    uNode = units.headNode();
    while(uNode)
    {
        delete uNode->item;
        uNode = uNode->next;
    }
    units.release(); // not really needed? just in case

    // release buildings
    ListNode<Building*>* bNode = buildings.headNode();
    while(bNode)
    {
        notifyAssetReleased(bNode->item);
        bNode->item->release();
        bNode = bNode->next;
    }
    // do the actual deletion of the objects
    while(bNode)
    {
        delete bNode->item;
        bNode = bNode->next;
    }
    buildings.release(); // not really needed? just in case

    // delete arrays
    for(int i = 0; i < m_MapSize; i++) 
    {
        delete [] m_pppUnitArray[i];
        delete [] m_pppBuildingArray[i];
    }
    delete [] m_pppUnitArray;
    delete [] m_pppBuildingArray;

}

// ===== SEARCHING

IAsset* AssetCollection::getAssetAt(const unsigned short x, const unsigned short y)
{
    IAsset* result = NULL;
    // buildings have higher return priority
    result = getBuildingAt(x, y);
    if(result)
        return result;
    // try for unit
    result = getUnitAt(x, y);
    return result;
}


Unit* AssetCollection::getUnitAt(const unsigned short x, const unsigned short y)
{
    return m_pppUnitArray[y][x];
    /*
     * deprecated
    // @TODO: location based data structure
    unsigned short tmpX, tmpY;
    ListNode<Unit*>* node = units.headNode();
    while(node) {
        tmpX = node->item->getGridX();
        if( (tmpX <= x)  &&  (tmpX >= x + node->item->getWidth()) )
        {
            tmpY = node->item->getGridY();
            if( (tmpY <= y)  &&  (tmpY >= y + node->item->getHeight()) )
            {
                return node->item;
            }
        }
        node = node->next;
    }
    return NULL;
    */
}


Building* AssetCollection::getBuildingAt(const unsigned short x, const unsigned short y)
{
    return m_pppBuildingArray[y][x];
    /*
     * deprecated
    // @TODO: location based data structure
    unsigned short tmpX, tmpY;
    ListNode<Building*>* node = buildings.headNode();
    while(node) {
        tmpX = node->item->getGridX();
        if( (tmpX >= x)  &&  (tmpX + node->item->getWidth() <= x) )
        {
            tmpY = node->item->getGridY();
            if( (tmpY >= y)  &&  (tmpY + node->item->getHeight() <= y) )
            {
                return node->item;
            }
        }
        node = node->next;
    }
    return NULL;
    */
}

// ===== OBSERVERS

void AssetCollection::registerListener(IAssetCollectionListener* listener)
{
    listeners.pushTail(listener);
}

void AssetCollection::unregisterListener(IAssetCollectionListener* listener)
{
    listeners.remove(listener);
}

void AssetCollection::notifyAssetCreated(IAsset* created)
{
    ListNode<IAssetCollectionListener*>* node = listeners.headNode();
    while(node)
    {
        node->item->handleCreatedAsset(created);
        node = node->next;
    }
}

void AssetCollection::notifyAssetReleased(IAsset* released)
{
    ListNode<IAssetCollectionListener*>* node = listeners.headNode();
    while(node)
    {
        node->item->handleReleasedAsset(released);
        node = node->next;
    }
}

// ===== UPDATES

void AssetCollection::updatePosition(Unit* u, const unsigned short oldPosX, const unsigned short oldPosY)
{
    // remove unit from array
    for (unsigned short x = 0; x < u->getWidth(); x++)
        for (unsigned short y = 0; y < u->getHeight(); y++)
            m_pppUnitArray[y + oldPosY][x + oldPosX] = NULL;

    // add unit to array
    for (unsigned short x = 0; x < u->getWidth(); x++)
        for (unsigned short y = 0; y < u->getHeight(); y++)
            m_pppUnitArray[y + (unsigned short)u->getPosition()->y][x + (unsigned short)u->getPosition()->x] = u;
}

// ===== DEBUG

#include <iostream>
void AssetCollection::debugPrintCollection()
{
    std::cout << "<AssetCollectionContent>\n";
    {
        ListNode<Building*>* node = buildings.tailNode();
        while(node) {
            std::cout << "  building (IID: " << node->item->getIID() << ")\n";
            node = node->prev;
        }
    }
    {
        ListNode<Unit*>* node = units.tailNode();
        while(node) {
            std::cout << "  unit (IID: " << node->item->getIID() << ")\n";
            node = node->prev;
        }
    }
    std::cout << "</AssetCollectionContent>\n";
}
