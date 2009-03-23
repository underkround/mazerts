#include "Radar.h"
#include "../Terrain/Terrain.h"
#include "AssetCollection.h"

void Radar::update(const float deltaT)
{
    m_ListsLastUpdated += deltaT;
    if (deltaT >= LIST_UPDATE_FREQUENCY)
    {
        m_VisibleListNeedRefreshing = true;
        m_VisibleEnemyListNeedRefreshing = true;
    }
}

bool Radar::release()
{
    return true;
}

DoubleLinkedList<IAsset*>* Radar::getVisibleAssets()
{
    if (m_VisibleListNeedRefreshing)
    {
        m_VisibleAssets.release();
        const unsigned int halfrange = (int)(0.5f * m_Range);
        unsigned int x = (int)(m_pHost->getPosition()->x - halfrange);
        unsigned int y = (int)(m_pHost->getPosition()->y - halfrange);
        unsigned int w = m_Range;
        unsigned int h = m_Range;
        unsigned short size = Terrain::getInstance()->getSize();
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x + w >= size) w = size - x;
        if (y + h >= size) h = size - y;

        AssetCollection::getAssetsAt(&m_VisibleAssets, x, y, w, h);
        // TODO: pick units at range & check for mountains etc blocks

        m_VisibleListNeedRefreshing = false;
    }
    return &m_VisibleAssets;
}

DoubleLinkedList<IAsset*>* Radar::getVisibleEnemyAssets()
{
    if (m_VisibleEnemyListNeedRefreshing)
    {
        const unsigned int halfrange = (int)(0.5f * m_Range);
        unsigned int x = (int)(m_pHost->getPosition()->x - halfrange);
        unsigned int y = (int)(m_pHost->getPosition()->y - halfrange);
        unsigned int w = m_Range;
        unsigned int h = m_Range;
        unsigned short size = Terrain::getInstance()->getSize();
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x + w >= size) w = size - x;
        if (y + h >= size) h = size - y;

        AssetCollection::getAssetsAt(&m_VisibleEnemyAssets, x, y, m_Range, m_Range);
        // TODO: pick units at range & check for mountains etc blocks
        ListNode<IAsset*>* ln = m_VisibleEnemyAssets.headNode();
        while (ln)
        {
            if (!(m_pHost->getOwner()->getEnemies() & ln->item->getOwner()->getId()))
                ln = m_VisibleEnemyAssets.removeGetNext(ln);
            else
                ln = ln->next;
        }

        m_VisibleEnemyListNeedRefreshing = false;
    }
    return &m_VisibleAssets;
}
