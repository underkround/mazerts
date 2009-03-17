#ifndef __TESTBITARRAY_H__
#define __TESTBITARRAY_H__

#include <assert.h>
#include <iostream>

#include "../Common/BitArray.h"
#include "CTimer.h"

using namespace std;

void testBitArray() {
    BitArray* b;
// test initialization to zero
    b = new BitArray(500);
    int sum = 0;
    for(unsigned int i=0; i<b->getSize(); i++)
        sum += b->getBit(i);
    assert(sum == 0);
    delete b;

// test sumthing

    b = new BitArray(33);

    for(unsigned int i=0; i<b->getSize(); i++)
        b->setBit(i, 1);
    b->debug(true);

    b->setBit(32, 1);
    b->setBit(33, 0);
    b->setBit(34, 1);
    b->setBit(35, 0);
    assert( b->getBit(32));
    assert(!b->getBit(33));
    assert( b->getBit(34));
    assert(!b->getBit(35));

    for(unsigned int i=0; i<b->getSize(); i++)
        b->setBit(i, 0);
    b->debug(true);
    sum = 0;
    for(unsigned int i=0; i<b->getSize(); i++)
        sum += b->getBit(i);
    assert(sum == 0);

    delete b;
    b = new BitArray(100000);
    b->debug(false);
    b->setBit(99997, 1);
    b->setBit(99999, 1);
    assert(!b->getBit(99996));
    assert( b->getBit(99997));
    assert(!b->getBit(99998));
    assert( b->getBit(99999));
    b->setBit(35, 1);
    assert(!b->getBit(34));
    assert( b->getBit(35));
    assert(!b->getBit(36));

    for(unsigned int i=0; i<b->getSize(); i++)
        b->setBit(i, 0);
    b->debug(false);

    delete b;
    b = new BitArray(16);
    // on
    b->setBit(0,1);
    b->setBit(1,1);
    b->setBit(2,1);
    b->setBit(3,1);
    b->setBit(5,1);
    b->setBit(8,1);
    b->setBit(13,1);
    b->setBit(15,1);
    // off
    b->setBit(5,0);
    b->setBit(2,0);
    b->debug(true);
    assert( b->getBool(0));
    assert( b->getBool(1));
    assert(!b->getBool(2));
    assert( b->getBool(3));
    assert(!b->getBool(4));
    assert( b->getBool(15));
    delete b;

    BitArray ba(2);
    ba.setBit(0, 1);
    assert(ba[0] == 1);
    assert(ba[1] == 0);
    for(unsigned int i=0; i<ba.getSize(); i++)
        cout << ba[i];
    ba.debug(true);

    const int bigsize = 1048576; // 1024x1024
    cout << " - creating bitarray of " << bigsize << " bits\n";
    b = new BitArray(bigsize);
    CTimer timer;
    timer.Create();
    timer.BeginTimer();
    for(unsigned int i=0; i<b->getSize(); i++)
        b->setBit(i, 1);
    timer.EndTimer();
    cout << "   -> setting all bits from 0 to 1 took " << timer.GetElapsedSeconds() << " seconds\n";

    char* arr = new char[bigsize];
    timer.BeginTimer();
    for(int i=0; i<bigsize; i++)
        arr[i] = 1;
    timer.EndTimer();
    cout << " - setting char* with " << bigsize << " elements to 1 took " << timer.GetElapsedSeconds() << " seconds\n";
    delete [] arr;

    cout << "\n";
    delete b;
};

#endif // __TESTBITARRAY_H__
