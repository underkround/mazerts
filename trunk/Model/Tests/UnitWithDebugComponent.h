#include "../Unit/Unit.h"
#include "../Terrain/Terrain.h"

#include "DebugComponent.h"

#include <iostream>
#include <assert.h>

void testUnitWithDebugComponent()
{
    // initialize terrain
    Terrain::getInstance()->initialize(5);

    // create unit
    Unit* u;
    u = new Unit();

    // set debug component to unit
    DebugComponent* dc = new DebugComponent(0);
    assert(u->addComponent(dc));

    // position unit
    assert(!u->forcePosition(21, 21)); // false positioning
    assert(u->forcePosition(2, 2)); // valid positioning

    // update unit, test debugComponent modes 0, 1 and 2
    // 0: idle
    // 1: sweep x from side to side in the terrain
    // 1: sweep y from side to side in the terrain
    int updates = 12;
    unsigned short cacheX, cacheY; // comparison values for assert
    cacheX = u->getCellX(); cacheY = u->getCellY();

    cout << " Debug component in sweep x mode\n";
    for(int i=0; i<updates; i++)  {
        u->update(0.001f);
        assert( cacheX == u->getCellX() );
        assert( cacheY == u->getCellY() );
        cacheX = u->getCellX(); cacheY = u->getCellY();
    }

    dc->setMode(1);
    cout << " Debug component in sweep x mode\n";
    for(int i=0; i<updates; i++)
    {
        cout << "unit at " << u->getCellX() << ":" << u->getCellY() << "\n";
        u->update(0.001f);
        assert( (cacheX == u->getCellX()) || (cacheX == u->getCellX()+1) || (cacheX == u->getCellX()-1) );
        assert( cacheY == u->getCellY() );
        cacheX = u->getCellX(); cacheY = u->getCellY();
    }

    cout << " Debug component in sweep y mode\n";
    dc->setMode(2);
    for(int i=0; i<updates; i++)
    {
        cout << "unit at " << u->getCellX() << ":" << u->getCellY() << "\n";
        u->update(0.001f);
        assert( cacheX == u->getCellX() );
        assert( (cacheY == u->getCellY()) || (cacheY == u->getCellY()+1) || (cacheY == u->getCellY()-1) );
        cacheX = u->getCellX(); cacheY = u->getCellY();
    }
    cout << "unit at " << u->getCellX() << ":" << u->getCellY() << "\n";
    dc = NULL;



    // add components over the initial capacity
    cout << "adding components to unit over it's initial capacity\n";
    for(int i=0; i<Unit::DEFAULT_COMPONENT_ARRAY_SIZE+2; i++)
    {
        cout << " adding.. ";
        assert(u->addComponent(new DebugComponent(0)));
    }

    delete u;

};
