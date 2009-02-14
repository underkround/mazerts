#include "../Unit/Unit.h"
#include "../Common/Vector3.h"
#include "../Common/Matrix4x3.h"
#include "../Common/MathUtil.h"

#include <iostream>
#include <assert.h>

void testUnitPositionAndFacing()
{
    Unit* pUnit;
    Vector3* v;
    Vector3* uv;

    // unit's position and facing should be zero when created
    pUnit = new Unit();
    uv = pUnit->getPosition();
    assert(uv->x == 0);
    assert(uv->y == 0);
    assert(uv->z == 0);
    uv = pUnit->getDirection();
    assert(uv->x == 0);
    assert(uv->y == 0);
    assert(uv->z == 0);
    delete pUnit;

    // move unit by changing it's position values
    pUnit = new Unit();
    pUnit->getPosition()->x += 10;
    pUnit->getPosition()->y += 5;
    pUnit->getPosition()->z += 1;
    v = pUnit->getPosition();
    assert(v->x == 10);
    assert(v->y == 5);
    assert(v->z == 1);
    delete pUnit;


    // move unit with vector
    pUnit = new Unit();
    v = new Vector3(1, 2, 3);
    uv = pUnit->getPosition();
    *uv += *v;
    assert(uv->x == 1);
    assert(uv->y == 2);
    assert(uv->z == 3);
    *uv += *v;
    assert(uv->x == 2);
    assert(uv->y == 4);
    assert(uv->z == 6);
    delete pUnit;
    delete v;

    // test moving unit with rotated vector
    pUnit = new Unit();
    v = new Vector3(1.0f, 0.0f, 0.0f); // move to right
    uv = pUnit->getPosition();
    Matrix4x3* m = new Matrix4x3();
    float theta = degToRad(90.0f);
    m->setupRotate(3, theta); // set the matrix to rotate 90deg over z-axis
    m->transform(*v); // v should now be around [0 -1 0] after rotation
    *uv += *v;

    assert(uv->x > -0.01f); // x should be near 0
    assert(uv->x <  0.01f);

    assert(uv->y >  0.99f); // y should be near 1
    assert(uv->y <  1.01f);

    assert(uv->z > -0.01f); // z should be near 0
    assert(uv->z <  0.01f);

    delete pUnit;

}