/**
 * Default implementation of IAssetRadar.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __RADAR_H__
#define __RADAR_H__

#define LIST_UPDATE_FREQUENCY 0.2f

#include "IAssetRadar.h"
#include "IAsset.h"
#include "../Defs/Defs.h"

class Radar : public IAssetRadar
{
public:

    Radar(RadarDef& def) : IAssetRadar(DEFAULT, def)
    {
        m_ListsUpdateCounter = 0.0f;
    }

    virtual ~Radar()
    {
    }

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
    float   m_ListsUpdateCounter;                   // Counter for list updating

};

#endif // __RADAR_H__