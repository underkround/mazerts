/**
 * IAssetRadar's responsibility is to keep track of the interesting
 * subjects in the unit's visibility area, update the players fog-
 * and possibly the terrain -bitmaps.
 *
 * IAssetRadar should advertise the subjects in the range to the
 * unit's other logics, or the logics could ask for list of them
 * from the radar.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __IASSETRADAR_H__
#define __IASSETRADAR_H__

#include "../Common/DoubleLinkedList.h"

#include "../Defs/Defs.h"

class IAsset;

class IAssetRadar
{
public:

    IAssetRadar(RadarDef def) : m_Def(def)
    {
    }

    virtual ~IAssetRadar()
    {
    }

    /**
     * This gets called by the asset when the radar is attached to
     * a asset.
     * DO NOT call this from outside!
     * @see IAsset::setRadar(..)
     */
    inline virtual void attach(IAsset* host) { m_pHost = host; }

    /**
     * Normal per-frame update call from the hosting unit.
     * @param deltaT    time from last update
     */
    virtual void update(const float deltaT) = 0;

    /**
     * Return true, if the concrete radar is spesific to hosting asset
     * (one radar-instance per asset-instance)
     * When this returns true, the host will destroy this instance when
     * returned.
     * If the same radar-instance is used with multiple asset-instances,
     * this should return false, and the host will not destroy this
     * radar instance.
     */
    virtual bool release() = 0;

    /**
     * Returns a list of all visible assets by this radar
     */
    virtual DoubleLinkedList<IAsset*>* getVisibleAssets() = 0;

    /**
     * Returns a list of enemy assets visible
     */
    virtual DoubleLinkedList<IAsset*>* getVisibleEnemyAssets() = 0;

    /**
     * Getters and setters for range
     */
    inline void setRange(const unsigned int range) { m_Range = range; }
    inline const unsigned int getRange() { return m_Range; }

protected:

    IAsset*                     m_pHost; // Asset this radar is attached to

    RadarDef                    m_Def; // definition struct for radar parameters

    DoubleLinkedList<IAsset*>   m_VisibleAssets; // Visible assets
    DoubleLinkedList<IAsset*>   m_VisibleEnemyAssets; // visible enemy assets

    // TODO: move to RadarDef
    unsigned int                m_Range; // Range of this radar

};

#endif // __IASSETRADAR_H__
