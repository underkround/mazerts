/**
 * General model tests
 *
 * Tests that ensure separate software parts work as expected
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#include <iostream>

#define VERBOSE 1


#include "TerrainTest.h"
#include "PathFinderTest.h"
#include "VectorAndMatriceTest.h"
#include "ConfigTest.h"
#include "PathFinderMasterTest.h"
//#include "AntinAITest.h"

#include "testAssetCollection.h"
#include "testAssetFactory.h"

using namespace std;

#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif

int main(int arcg, char* argv[])
{
//    if(VERBOSE) cout << "\n *** terrain ***\n";                     testTerrain();

//    if(VERBOSE) cout << "\n *** vector & matrix ***\n";             testVectorAndMatrice ();

//    if(VERBOSE) cout << "\n *** config from file ***\n";            testConfig();

//    if(VERBOSE) cout << "\n *** Pathfinder ***\n";                  testPathFinder();

    if(VERBOSE) cout << "\n *** PathFinderMaster-thread ***\n";     testPathFinderMaster();

//    if(VERBOSE) cout << "\n *** Antin AI ***\n";                    testAntinAI();

    if(VERBOSE) cout << "\n *** Asset collection ***\n";            testAssetCollection();

    if(VERBOSE) cout << "\n *** Asset factory ***\n";               testAssetFactory();

//    _CrtDumpMemoryLeaks();
    return 0;
}

