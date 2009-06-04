/**
 * War factory has extraction point for new units, otherwise same as others
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#pragma once


#include "Building.h"

class WarFactory : public Building
{
public:
    WarFactory(AssetDef& def) : Building(def)
    {
        m_Created = false;
        m_unitExtractionPointX = getDef()->gridEntrancePointX;
        m_unitExtractionPointY = getDef()->gridEntrancePointY - 6;
    }

    inline void create()
    {
        Building::create();
    }

    inline void setExtractionPoint(unsigned short  x, unsigned short  y) { m_unitExtractionPointX = x; m_unitExtractionPointY = y; }
    inline const unsigned short getExtractionPointX() const { return m_unitExtractionPointX; }
    inline const unsigned short getExtractionPointY() const { return m_unitExtractionPointY; }

private:
    unsigned short m_unitExtractionPointX;
    unsigned short m_unitExtractionPointY;

};