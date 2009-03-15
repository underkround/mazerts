/**
 * Default implementation of IAssetRadar.
 *
 * $Revision: 74 $
 * $Date: 2009-03-02 23:53:12 +0200 (ma, 02 maalis 2009) $
 * $Id: IAssetRadar.h 74 2009-03-02 21:53:12Z zem $
 */
#ifndef __RADAR_H__
#define __RADAR_H__

#define LIST_UPDATE_FREQUENCY 0.2

#include "IAssetRadar.h"
#include "IAsset.h"

class Radar : IAssetRadar
{
    /**
     * Normal per-frame update call from the hosting unit.
     * @param deltaT    time from last update
     */
    virtual void update(const float deltaT);

    /**
     * Returns true :P
     */
    virtual bool release();

    /**
     * Returns a list of all visible assets by this radar
     */
    virtual DoubleLinkedList<IAsset*>* getVisibleAssets();

    /**
     * Returns a list of enemy assets visible
     */
    virtual DoubleLinkedList<IAsset*>* getVisibleEnemyAssets();

private:
    bool    m_VisibleListNeedRefreshing;            // guess
    bool    m_VisibleEnemyListNeedRefreshing;       // guess
    float   m_ListsLastUpdated;                     // when lists were last updated
};

#endif // __RADAR_H__