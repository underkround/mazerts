#include "../Unit/Unit.h"
#include "../Common/Vector3.h"
#include "../Common/Matrix4x3.h"
#include "../Common/MathUtil.h"

#include <iostream>
#include <assert.h>

void testVectorAndMatrice()
{

    // DEPRECATED
    // Now this is just pointless!
    // Furthermore, the second test results as gimbal lock :D


    Vector3* v1;
    Vector3* v2;
    Vector3* v3;
    Matrix4x3* m1;

    // add two vectors, set result to third
    v1 = new Vector3(1.0f, 2.0f, 3.0f);
    v2 = new Vector3(9.0f, 8.0f, 7.0f);
    v3 = new Vector3(0.0f, 0.0f, 0.0f);
    *v3 = *v1 + *v2;
    assert(v3->x == 10);
    assert(v3->y == 10);
    assert(v3->z == 10);
    delete v1; delete v2; delete v3;

    // setup matrix to rotate vector over each axis
    m1 = new Matrix4x3();
    v1 = new Vector3(1.0f, 0.0f, 0.0f);

    // 180deg over x
    m1->setupRotate(1, degToRad(180.0f));
    m1->transform(*v1);
    std::cout << v1->x <<" "<< v1->y <<" "<< v1->z <<"\n";
    // 180deg over y
    m1->setupRotate(2, degToRad(180.0f));
    m1->transform(*v1);
    std::cout << v1->x <<" "<< v1->y <<" "<< v1->z <<"\n";
    // 180deg over z
    m1->setupRotate(3, degToRad(180.0f));
    m1->transform(*v1);
    std::cout << v1->x <<" "<< v1->y <<" "<< v1->z <<"\n";
    delete v1;
    delete m1;

/*
    std::cout << v->x <<" "<< v->y <<" "<< v->z <<"\n";
    std::cout << "\n";
    std::cout << m->m11 <<" "<< m->m21 <<" "<< m->m31 <<"\n";
    std::cout << m->m12 <<" "<< m->m22 <<" "<< m->m32 <<"\n";
    std::cout << m->m13 <<" "<< m->m23 <<" "<< m->m33 <<"\n";
    std::cout << "\n";
    std::cout << uv->x <<" "<< uv->y <<" "<< uv->z <<"\n";
*/

}
