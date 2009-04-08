#include "Radar.h"
#include "../Terrain/Terrain.h"
#include "AssetCollection.h"

void Radar::update(const float deltaT)
{
    m_ListsUpdateCounter += deltaT;
    if (m_ListsUpdateCounter >= LIST_UPDATE_FREQUENCY)
    {
        m_VisibleListNeedRefreshing = true;
        m_VisibleEnemyListNeedRefreshing = true;
        m_ListsUpdateCounter = 0;
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
        const unsigned int halfrange = (int)(0.5f * m_Def.range);
        unsigned int x = (int)(m_pHost->getPosition()->x - halfrange);
        unsigned int y = (int)(m_pHost->getPosition()->y - halfrange);
        unsigned int w = (unsigned int)m_Def.range;
        unsigned int h = (unsigned int)m_Def.range;
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
    m_VisibleEnemyAssets.release();

    if (m_VisibleEnemyListNeedRefreshing)
    {
        //const unsigned short halfrange = (unsigned short)(0.5f * m_Def.range);
        
        signed short x = (signed short)(m_pHost->getCenterGridX() - (signed short)m_Def.range);
        signed short y = (signed short)(m_pHost->getCenterGridY() - (signed short)m_Def.range);
        unsigned short w = (unsigned short)(m_Def.range) << 1;
        unsigned short h = (unsigned short)(m_Def.range) << 1;
        unsigned short size = Terrain::getInstance()->getSize();
        
        //w & h are recalculated, if bounding occurs (otherwise the radar
        //sees m_Def.range squares from the side, no matter how close to the side the unit is)
        if (x < 0)
        {
            w += x;
            x = 0;            
        }
        if (y < 0)
        {
            h += y;
            y = 0;
        }
        if (x + w >= size) w = size - x;
        if (y + h >= size) h = size - y;

        AssetCollection::getAssetsAt(&m_VisibleEnemyAssets, (unsigned short)x, (unsigned short)y, w, h);
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
    return &m_VisibleEnemyAssets;
}
