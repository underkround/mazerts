/**
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

DoubleLinkedList<Unit*>     AssetCollection::m_UnitReleaseStack     = DoubleLinkedList<Unit*>();
DoubleLinkedList<Building*> AssetCollection::m_BuildingReleaseStack = DoubleLinkedList<Building*>();

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
    ListNode<Building*>* node;
    ListNode<Building*>* temp;

    // release requested from previous frame, if any
    if(!m_BuildingReleaseStack.empty())
    {
        Building* b;
        node = m_BuildingReleaseStack.headNode();
        while(node)
        {
            b = node->item;
            clearBuildingArray((unsigned short)b->getPosition()->x, (unsigned short)b->getPosition()->y, b->getWidth(), b->getHeight());
            b->release();
            delete b;
            node = m_BuildingReleaseStack.removeGetNext(node);
        }
    }

    // update buildings
    node = buildings.tailNode();
    while(node)
    {
        if(node->item->update(deltaT) == IAsset::RESULT_DELETEME)
        {
            // store next node to go
            temp = node;
            node = node->prev;
            // delete asset
            notifyAssetReleased(temp->item);

            //temp->item->release();
            //delete temp->item;
            m_BuildingReleaseStack.pushHead(temp->item);

            buildings.remove(temp);
        }
        else
            node = node->prev;
    }
}


void AssetCollection::updateUnits(const float deltaT)
{
    ListNode<Unit*>* node;
    ListNode<Unit*>* temp;

    // release requested from previous frame, if any
    if(!m_UnitReleaseStack.empty())
    {
        Unit* u;
        node = m_UnitReleaseStack.headNode();
        while(node)
        {
            u = node->item;
            clearUnitArray((unsigned short)u->getPosition()->x, (unsigned short)u->getPosition()->y, u->getWidth(), u->getHeight());
            u->release();
            delete u;
            node = m_UnitReleaseStack.removeGetNext(node);
        }
    }

    // update units
    node = units.tailNode();
    while(node)
    {
        if(node->item->update(deltaT) == IAsset::RESULT_DELETEME)
        {
            // store next node to go
            temp = node;
            node = node->prev;
            // delete asset
            notifyAssetReleased(temp->item);

            //temp->item->release();
            //delete temp->item;
            m_UnitReleaseStack.pushHead(temp->item);

            // delete node
            units.remove(temp);
        }
        else
            node = node->prev;
    }
}

// ===== REGISTERING

void AssetCollection::registerAsset(IAsset* a)
{
    switch(a->getAssetType())
    {
    case IAsset::UNIT:
        registerUnit( (Unit*)a );
        break;
    case IAsset::BUILDING:
        registerBuilding( (Building*)a );
        break;
    default:
        break;
    }
}

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

void AssetCollection::clearUnitArray(unsigned short posX, unsigned short posY, unsigned short width, unsigned short height)
{
    for (unsigned short x = 0; x < width; x++)
        for (unsigned short y = 0; y < height; y++)
            m_pppUnitArray[y + posY][x + posX] = NULL;
}


void AssetCollection::clearBuildingArray(unsigned short posX, unsigned short posY, unsigned short width, unsigned short height)
{
    for (unsigned short x = 0; x < width; x++)
        for (unsigned short y = 0; y < height; y++)
            m_pppBuildingArray[y + posY][x + posX] = NULL;
}


void AssetCollection::releaseUnit(Unit* u)
{
    notifyAssetReleased(u);
    units.remove(u);

    clearUnitArray((unsigned short)u->getPosition()->x, (unsigned short)u->getPosition()->y, u->getWidth(), u->getHeight());
/*    for (unsigned short x = 0; x < u->getWidth(); x++)
        for (unsigned short y = 0; y < u->getHeight(); y++)
            m_pppUnitArray[y + (unsigned short)u->getPosition()->y][x + (unsigned short)u->getPosition()->x] = NULL;
*/

    delete u;
}


void AssetCollection::releaseBuilding(Building* b)
{
    notifyAssetReleased(b);
    buildings.remove(b);

    clearBuildingArray((unsigned short)b->getPosition()->x, (unsigned short)b->getPosition()->y, b->getWidth(), b->getHeight());
/*    for (unsigned short x = 0; x < b->getWidth(); x++)
        for (unsigned short y = 0; y < b->getHeight(); y++)
            m_pppBuildingArray[y + (unsigned short)b->getPosition()->y][x + (unsigned short)b->getPosition()->x] = NULL;
*/

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
}


Building* AssetCollection::getBuildingAt(const unsigned short x, const unsigned short y)
{
    return m_pppBuildingArray[y][x];
}

const int AssetCollection::getUnitsAt(DoubleLinkedList<Unit*>* pList, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height)
{
    /**
     * @TODO: figure out which way is faster
     * Here's some room for optimizing. Way 1 lacks checking whether unit is
     * already in the list. That could be achieved using local boolean table.
     *
     * One way to get things roll faster would be:
     * int method = (IAsset::getInstanceCount() - IAsset::getInstanceDestructionCount()  >  200 && width * height < 100000) ? way1 : way2;
     */

    // way 1
    /*
    for (unsigned short w = 0; w < width; w++)
        for (unsigned short h = 0; h < height; h++)
            if (m_pppUnitArray[y + h][x + w])
                pList->pushTail(m_pppUnitArray[y + h][x + w]);
    */

    // way 2
    int items = 0;
    ListNode<Unit*>* pLn = units.headNode();
    while (pLn)
    {
        Vector3* pV = pLn->item->getPosition();
        unsigned short uwidth = pLn->item->getWidth();
        unsigned short uheight = pLn->item->getHeight();
        if (pV->x <= (x + width) && (pV->x + uwidth) >= x && pV->y <= (y + height) && (pV->y + uheight) >= y) 
        {
            if(pList)
            {
                pList->pushTail(pLn->item);
            }
            items++;
        }
        pLn = pLn->next;
    }
    return items;
}

const int AssetCollection::getBuildingsAt(DoubleLinkedList<Building*>* pList, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height)
{
    int items = 0;
    ListNode<Building*>* pLn = buildings.headNode();
    while (pLn)
    {
        Vector3* pV = pLn->item->getPosition();
        unsigned short uwidth = pLn->item->getWidth();
        unsigned short uheight = pLn->item->getHeight();
        if (pV->x <= (x + width) && (pV->x + uwidth) >= x && pV->y <= (y + height) && (pV->y + uheight) >= y)         
        {
            if(pList)
            {
                pList->pushTail(pLn->item);
            }
            items++;
        }
        pLn = pLn->next;
    }
    return items;
}

const int AssetCollection::getAssetsAt(DoubleLinkedList<IAsset*>* pList, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height)
{
    int items = 0;
    ListNode<IAsset*>* pLn = (ListNode<IAsset*>*)units.headNode();
    for (int i = 0; i < 2; i++)
    {
        if (i == 1) pLn = (ListNode<IAsset*>*)buildings.headNode();
        while (pLn)
        {
            Vector3* pV = pLn->item->getPosition();
            unsigned short uwidth = pLn->item->getWidth();
            unsigned short uheight = pLn->item->getHeight();
            if (pV->x <= (x + width) && (pV->x + uwidth) >= x && pV->y <= (y + height) && (pV->y + uheight) >= y) 
            {
                if(pList)
                {
                    pList->pushTail(pLn->item);
                }
                items++;
            }
            pLn = pLn->next;
        }
    }
    return items;
}

const int AssetCollection::getPlayerUnitsAt(DoubleLinkedList<Unit*>* pList, const int player, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height)
{
    int items = 0;
    ListNode<Unit*>* pLn = units.headNode();
    while (pLn)
    {
        if (pLn->item->getOwner()->getId() == player)
        {
            Vector3* pV = pLn->item->getPosition();
            unsigned short uwidth = pLn->item->getWidth();
            unsigned short uheight = pLn->item->getHeight();
            if (pV->x <= (x + width) && (pV->x + uwidth) >= x && pV->y <= (y + height) && (pV->y + uheight) >= y) 
            {
                if(pList)
                {
                    pList->pushTail(pLn->item);
                }
                items++;
            }
        }
        pLn = pLn->next;
    }
    return items;
}

const int AssetCollection::getPlayerBuildingsAt(DoubleLinkedList<Building*>* pList, const int player, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height)
{
    int items = 0;
    ListNode<Building*>* pLn = buildings.headNode();
    while (pLn)
    {
        if (pLn->item->getOwner()->getId() == player)
        {
            Vector3* pV = pLn->item->getPosition();
            unsigned short uwidth = pLn->item->getWidth();
            unsigned short uheight = pLn->item->getHeight();
            if (pV->x <= (x + width) && (pV->x + uwidth) >= x && pV->y <= (y + height) && (pV->y + uheight) >= y) 
            {
                if(pList)
                {
                    pList->pushTail(pLn->item);
                }
                items++;
            }
        }
        pLn = pLn->next;
    }
    return items;
}

const int AssetCollection::getPlayerAssetsAt(DoubleLinkedList<IAsset*>* pList, const int player, const unsigned short x, const unsigned short y, const unsigned short width, const unsigned short height)
{
    int items = 0;
    ListNode<IAsset*>* pLn = (ListNode<IAsset*>*)units.headNode();
    for (int i = 0; i < 2; i++)
    {
        if (i == 1) pLn = (ListNode<IAsset*>*)buildings.headNode();
        while (pLn)
        {
            if (pLn->item->getOwner()->getId() == player)
            {
                Vector3* pV = pLn->item->getPosition();
                unsigned short uwidth = pLn->item->getWidth();
                unsigned short uheight = pLn->item->getHeight();
                if (pV->x <= (x + width) && (pV->x + uwidth) >= x && pV->y <= (y + height) && (pV->y + uheight) >= y) 
                {
                    if(pList)
                    {
                        pList->pushTail(pLn->item);
                    }
                    items++;
                }
            }
            pLn = pLn->next;
        }
    }
    return items;
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

void AssetCollection::updatePosition(IAsset* a, const unsigned short oldPosX, const unsigned short oldPosY)
{
    // select correct array
    IAsset*** pArr;
    if (a->getAssetType() == IAsset::UNIT)
        pArr = (IAsset***)m_pppUnitArray;
    else
        pArr = (IAsset***)m_pppBuildingArray;

    // remove asset from array
    for (unsigned short x = 0; x < a->getWidth(); x++)
        for (unsigned short y = 0; y < a->getHeight(); y++)
            if (pArr[y + oldPosY][x + oldPosX] == a) // this check is probably not needed in future
                pArr[y + oldPosY][x + oldPosX] = NULL;

    // add asset to array
    for (unsigned short x = 0; x < a->getWidth(); x++)
        for (unsigned short y = 0; y < a->getHeight(); y++)
            pArr[y + (unsigned short)a->getPosition()->y][x + (unsigned short)a->getPosition()->x] = a;
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
