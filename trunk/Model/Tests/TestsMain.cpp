/**
 * Damage and DamageHandler-tests
 *
 * Tests that Damage and DamageHandler work as expected
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#include "DamageAndDamageHandlerTests.h"
#include "UnitPositionAndFacingTest.h"
#include "TerrainTest.h"

int main(int arcg, char* argv[])
{
    testDamageAndDamageHandler();
    testTerrain();
    //testUnitPositionAndFacing();
}