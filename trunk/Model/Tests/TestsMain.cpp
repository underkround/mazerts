/**
 * Damage and DamageHandler-tests
 *
 * Tests that Damage and DamageHandler work as expected
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#include <iostream>


#include "DamageAndDamageHandlerTests.h"
#include "UnitPositionAndFacingTest.h"
#include "TerrainTest.h"
#include "PathFinderTest.h"
#include "VectorAndMatriceTest.h"

using namespace std;

#define VERBOSE 1

int main(int arcg, char* argv[])
{
    if(VERBOSE) cout << " *** damage and damagehandler ***\n";
    testDamageAndDamageHandler();

    if(VERBOSE) cout << "\n *** terrain ***\n";
    testTerrain();

    if(VERBOSE) cout << "\n *** Pathfinder ***\n";
    testPathFinder();

    if(VERBOSE) cout << "\n *** unit position ***\n";
    testUnitPositionAndFacing();

    //if(VERBOSE) cout << "\n *** vector & matrix ***\n";
    //testVectorAndMatrice ();

}

