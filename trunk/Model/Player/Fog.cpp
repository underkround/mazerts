#include "Fog.h"
#include "../Defs/DefManager.h"
#include "../Asset/AssetCollection.h"
#include "../Asset/Unit.h"
#include "../Asset/Building.h"
#include "../Terrain/Terrain.h" // for size
#include <string.h> //For memset

DoubleLinkedList<Fog::FOGMASK*> Fog::m_FogMasks = DoubleLinkedList<Fog::FOGMASK*>();

Fog::Fog()
{
    // create fog array
    const unsigned short len = Terrain::getInstance()->getSize();
    m_Fog = new bool*[len];
    for (int i = 0; i < len; i++) {
        m_Fog[i] = new bool[len];
    }

    // set array to false
    for (unsigned short i = 0; i < len; i++) {
        memset(m_Fog[i], false, len * sizeof(bool));
    }

    m_Enabled = true;
    m_ChangeCounter = 0;
    m_UpdateCounter = 100.0f;
    setUpdateInterval(0.5f);
}

Fog::~Fog()
{
  unsigned short len = Terrain::getInstance()->getSize();
  for (int i = 0; i < len; i++)
    delete [] m_Fog[i];    
  delete [] m_Fog;
}

Fog::FOGMASK* Fog::getFogMask(int range)
{
    ListNode<FOGMASK*>* ln = m_FogMasks.headNode();
    Fog::FOGMASK* pMask = NULL;

    while (ln) {
        Fog::FOGMASK* fm = ln->item;
        if (fm->radius == range) {
            pMask = fm;
            break;
        }
        ln = ln->next;
    }

    if (!pMask) {
        pMask = calculateFogMask(range);
        m_FogMasks.pushTail(pMask);
    }

    return pMask;
}

void Fog::update(float deltaT)
{
    m_UpdateCounter += deltaT;
    if(m_UpdateCounter < m_UpdateInterval)
        return;
    m_UpdateCounter = 0.0f;
    m_ChangeCounter++;

    // reset fog
    const unsigned short len = Terrain::getInstance()->getSize();
    for (unsigned short y = 0; y < len; y++)
    {
        memset(m_Fog[y], false, len * sizeof(bool));
    }

    // add units & buildings visibilities
    for (int asd = 0; asd < 2; asd++) { // 0 = units, 1 = buildings
        ListNode<Unit*>* lnu = NULL;
        ListNode<Building*>* lnb = NULL;
        if (asd == 0) {
            DoubleLinkedList<Unit*>* units = AssetCollection::getAllUnits();
            lnu = units->headNode();
        } else {
            DoubleLinkedList<Building*>* buildings = AssetCollection::getAllBuildings();
            lnb = buildings->headNode();
        }
        while (lnu || lnb)
        {
            IAsset* pAsset = NULL;
            if (lnu)
                pAsset = (IAsset*)lnu->item;
            else
                pAsset = (IAsset*)lnb->item;

            if (pAsset->getOwner() == getOwner()) {
                const unsigned short radius = (unsigned short) pAsset->getDef()->pDefRadar->range; // TODO: float? everywhere?
                const unsigned short centerx = pAsset->getCenterGridX();
                const unsigned short centery = pAsset->getCenterGridY();

                FOGMASK* fm = getFogMask(radius);
                if (centerx - radius >= 0 && centerx + radius < len)
                {
                    // totally inside map (horizontally)
                    short y = centery - radius;
                    unsigned short yoffset = 0;
                    unsigned short ycutoff = 0;
                    if (centery - radius < 0) yoffset = abs(centery - radius);
                    if (centery + radius >= len) ycutoff = abs(len - (centery + radius));

                    ListNode<Fog::FOGROW*>* row = fm->data.headNode();
                    while (row)
                    {
                        if (y >= centery - radius + yoffset)
                            memset(&m_Fog[y][centerx - radius + row->item->offset], true, row->item->amount);
                        ++y;
                        if (y >= centery + radius - ycutoff) break;
                        row = row->next;
                    }
                }
                else
                {
                    // partially over edge (horizontally)
                    unsigned short yoffset = 0;
                    unsigned short ycutoff = 0;

                    if (centery - radius < 0) yoffset = abs(centery - radius);
                    if (centery + radius >= len) ycutoff = abs(len - (centery + radius));

                    short y = centery - radius;
                    ListNode<Fog::FOGROW*>* row = fm->data.headNode();
                    while (row)
                    {
                        if (y >= centery - radius + yoffset) {
                            for (int x = max(row->item->offset + centerx - radius, 0); x < min(len, (unsigned short)(centerx - radius + row->item->offset + row->item->amount)); x++) {
                                m_Fog[y][x] = true;
                            }
                        }
                        ++y;
                        if (y >= centery + radius - ycutoff) break;
                        row = row->next;
                    }
                }
            }
            if (lnu)
                lnu = lnu->next;
            else if (lnb)
                lnb = lnb->next;
        }
    }
}

void Fog::create()
{
    // todo: create all fogmasks here?
}

void Fog::release()
{
    // delete fog masks
    ListNode<FOGMASK*>* ln1 = m_FogMasks.headNode();
    while (ln1)
    {
        ListNode<FOGROW*>* ln2 = ln1->item->data.headNode();
        while (ln2)
        {
            delete ln2->item;
            ln2 = ln1->item->data.removeGetNext(ln2);
        }
        delete ln1->item;
        ln1 = m_FogMasks.removeGetNext(ln1);
    }
}

Fog::FOGMASK* Fog::calculateFogMask(const int radius)
{
  // array is len * len (w * h)
  const unsigned short len = radius * 2 + 1;
  bool** tempArray = new bool*[len];
  for (unsigned short i = 0; i < len; i++) {
    tempArray[i] = new bool[len];
  }

  // set array to false
  for (unsigned short i = 0; i < len; i++) {
    memset(tempArray[i], false, len * sizeof(bool));
  }

  // initial values
  unsigned short x = 0;
  unsigned short y = radius;
  int p = (5 - radius * 4) / 4;
  addValues(tempArray, x, y, radius);

  while (x < y)
  {
    ++x;
    if (p < 0) {
      p += 2 * x + 1;
    } else {
      --y;
      p += 2 * (x - y) + 1;
    }

    addValues(tempArray, radius - x, radius - y, radius);
    if (x < y) {
      addValues(tempArray, radius - y, radius - x, radius);
    }
  }

  FOGMASK* fm = new FOGMASK();
  fm->radius = radius;
  DoubleLinkedList<FOGROW*>& list = fm->data;
  for (unsigned short yy = 0; yy < len; yy++) {
        FOGROW* fogrow = new FOGROW();
        for (unsigned short xx = 0; xx < len; xx++) {
            if (tempArray[yy][xx]) {
                fogrow->offset = xx;
                fogrow->amount = len - xx * 2;
                break;
            }
        }
        list.pushTail(fogrow);
  }

  for (unsigned short i = 0; i < len; i++)
    delete [] tempArray[i];
  delete [] tempArray;

  return fm;
}

void Fog::addValues(bool** tempArray, const int x, const int y, const int radius)
{
  const int len = (radius - x) * 2 + 1; // magic amount of "pixels" per line
  for (int i = 0; i < len; i++) {
    tempArray[y][x + i] = true; // upper half
    tempArray[radius * 2 - y][x + i] = true; // lower half
  }
}

const bool Fog::isAssetVisible(IAsset* pAsset)
{
    if(!m_Enabled) return true;

    const short x = pAsset->getGridX();
    const short y = pAsset->getGridY();
    const unsigned short w = pAsset->getWidth() - 1;
    const unsigned short h = pAsset->getHeight() - 1;
    return m_Fog[y][x] || m_Fog[y + h][x] || m_Fog[y][x + w] || m_Fog[y + h][x + w];
}