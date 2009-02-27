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

DoubleLinkedList<Unit*>     AssetCollection::units      = DoubleLinkedList<Unit*>();
DoubleLinkedList<Building*> AssetCollection::buildings  = DoubleLinkedList<Building*>();

DoubleLinkedList<IAssetCollectionListener*> AssetCollection::listeners = DoubleLinkedList<IAssetCollectionListener*>();

// should not need ctor & dtor ever
AssetCollection::AssetCollection() { }
AssetCollection::~AssetCollection() { releaseAll(); }

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
}

void AssetCollection::registerBuilding(Building* b) {
    buildings.pushHead(b);
    notifyAssetCreated(b);
}

// ===== RELEASING

void AssetCollection::releaseUnit(Unit* u)
{
    notifyAssetReleased(u);
    units.remove(u);
    delete u;
}


void AssetCollection::releaseBuilding(Building* b)
{
    notifyAssetReleased(b);
    buildings.remove(b);
    delete b;
}


void AssetCollection::releaseAll()
{
    // release buildings
    ListNode<Unit*>* uNode = units.headNode();
    ListNode<Unit*>* uTemp;
    while(uNode) {
        uTemp = uNode;
        uNode = uTemp->next;
        // remove item
        notifyAssetReleased(uTemp->item);
        uTemp->item->release();
        delete uTemp->item;
        units.remove(uTemp);
    }
    units.release(); // not really needed? just in case

    // release units
    ListNode<Building*>* bNode = buildings.headNode();
    ListNode<Building*>* bTemp;
    while(bNode) {
        bTemp = bNode;
        bNode = bTemp->next;
        // remove item
        notifyAssetReleased(bTemp->item);
        bTemp->item->release();
        delete bTemp->item;
        buildings.remove(bTemp);
    }
    buildings.release(); // not really needed? just in case
}

// ===== SEARCHING

IAsset* AssetCollection::getAssetAt(const short x, const short y)
{
    // @TODO: location based data structure
    IAsset* result = NULL;
    // buildings have higher return priority
    result = getBuildingAt(x, y);
    if(result)
        return result;
    // try for unit
    result = getUnitAt(x, y);
    return result;
}


Unit* AssetCollection::getUnitAt(const short x, const short y)
{
    // @TODO: location based data structure
    short tmpX, tmpY;
    ListNode<Unit*>* node = units.headNode();
    while(node) {
        tmpX = node->item->getGridX();
        if( (tmpX >= x)  &&  (tmpX + node->item->getWidth() <= x) ) {
            tmpY = node->item->getGridY();
            if( (tmpY >= y)  &&  (tmpY + node->item->getHeight() <= y) ) {
                return node->item;
            }
        }
        node = node->next;
    }
    return NULL;
}


Building* AssetCollection::getBuildingAt(const short x, const short y)
{
    // @TODO: location based data structure
    short tmpX, tmpY;
    ListNode<Building*>* node = buildings.headNode();
    while(node) {
        tmpX = node->item->getGridX();
        if( (tmpX >= x)  &&  (tmpX + node->item->getWidth() <= x) ) {
            tmpY = node->item->getGridY();
            if( (tmpY >= y)  &&  (tmpY + node->item->getHeight() <= y) ) {
                return node->item;
            }
        }
        node = node->next;
    }
    return NULL;
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
