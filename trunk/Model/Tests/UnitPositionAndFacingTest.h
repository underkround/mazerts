#include "../Unit/Unit.h"
#include "../Common/Vector3.h"
#include <assert.h>

void testUnitPositionAndFacing()
{
    Unit* pUnit = new Unit();

    // unit's position and facing should be zero when created
    Vector3* v;
    v = pUnit->getPosition();
    assert(v->x == 0);
    assert(v->y == 0);
    assert(v->z == 0);
    v = pUnit->getDirection();
    assert(v->x == 0);
    assert(v->y == 0);
    assert(v->z == 0);

    // move unit by changing it's position values
    pUnit->getPosition()->x += 10;
    pUnit->getPosition()->y += 5;
    pUnit->getPosition()->z += 1;
    v = pUnit->getPosition();
    assert(v->x == 10);
    assert(v->y == 5);
    assert(v->z == 6);

    delete pUnit;
    pUnit = new Unit();

    // move unit with vector
    Vector3* v2 = new Vector3(1, 2, 3);
    v = pUnit->getPosition();
    *v += *v2;
    assert(v->x == 1);
    assert(v->y == 2);
    assert(v->z == 3);
    *v += *v2;
    assert(v->x == 2);
    assert(v->y == 4);
    assert(v->z == 7);

    delete pUnit;
}