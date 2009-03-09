/**
 * 1-dimensional bit array object that works mostly like... ...array of bits :)
 *
 * You give the size of the array (count of the bits it can hold) in the constructor
 *
 * TODO: operator overload for write-operations
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __BITARRAY_H__
#define __BITARRAY_H__

#include <math.h>   // for ceil(..)
#include <string.h> // for memset(..)

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

class BitArray
{
    // type of the block used to store the bits
    typedef unsigned char bitblock;

public:

    /**
     * Create and allocate the array, sets every bit to 0
     * @param bitsize   allocated size of the array in bits
     */
    BitArray(const unsigned int bitsize)
    {
        m_BitSize = bitsize;
        //allocate( ceil( (float)bitsize / (float)blockBitSize() ) );
        m_BlockCount = (unsigned int)ceil( (float)bitsize / (float)blockBitSize() );
        m_Blocks = new bitblock[m_BlockCount];
        ::memset(m_Blocks, 0, m_BlockCount*sizeof(bitblock));
    }

    ~BitArray()
    {
        delete [] m_Blocks;
    }

// ===== Properties

    /**
     * @return  the size of one block (base allocation unit) in bits,
     *          how many bits can the basic storage-unit hold
     */
    const unsigned int blockBitSize() const
    {
        return sizeof(bitblock) * 8;
    }

    /**
     * @return  the size of one block (base allocation unit) in bytes
     */
    const unsigned int blockByteSize() const
    {
        return sizeof(bitblock);
    }

    /**
     * @return  the size of this array, the number of bits it can hold
     */
    const unsigned int getSize() const
    {
        return m_BitSize;
    }

// ===== Helpers

    /**
     * @param bitIndex  index of the bit for which to get the block
     * @return          index for the block holding bit with given index
     */
    const unsigned int blockIndexFor(const unsigned int bitIndex) const
    {
        return bitIndex / ( sizeof(bitblock) * 8 );
    }

    /**
     * @param bitIndex  index of the bit for which to get the shift value
     * @return          the shift value for bit inside the block it's stored in
     */
    const unsigned int shiftFor(const unsigned int bitIndex) const
    {
        return bitIndex % ( sizeof(bitblock) * 8 );
    }

// ===== Bit operations

    /**
     * Get the value of the bit
     * @param index     index of the bit to get
     * @return          value of the bit at index, 0 or 1
     */
    const unsigned int getBit(unsigned int bitIndex) const
    {
        const unsigned int block = bitIndex / ( sizeof(bitblock) * 8 );
        const unsigned int shift = bitIndex % ( sizeof(bitblock) * 8 );
        return m_Blocks[block] >> shift & 1;
    }

    /**
     * Get the value of the bit by bool
     * @param index     index of the bit to get
     * @return          value of the bit at index, true for 1 and false for 0
     */
    const bool getBool(const unsigned int bitIndex) const
    {
        const unsigned int block = bitIndex / ( sizeof(bitblock) * 8 );
        const unsigned int shift = bitIndex % ( sizeof(bitblock) * 8 );
        return (m_Blocks[block] >> shift & 1) ? true : false;
    }

    /**
     * Set the value of the bit
     * @param index     index of the bit to set
     * @param value     value of the bit to set, 0 for 0 and anything else
     *                  for 1
     */
    void setBit(const unsigned int bitIndex, const int value)
    {
        const unsigned int block = bitIndex / ( sizeof(bitblock) * 8 );
        const unsigned int shift = bitIndex % ( sizeof(bitblock) * 8 );
        if(value) // set to 1
            m_Blocks[block] |= 1 << shift;
        else // set to 0
            m_Blocks[block] = m_Blocks[block] ^ (m_Blocks[block] & (1 << shift));
    }

    /**
     * Set the value of the bit with boolean
     * Stupid overload..
     * @param index     index of the bit to set
     * @param value     value of the bit to set, true for 1 and false for 0
     */
    void setBool(const unsigned int bitIndex, const bool value)
    {
        const unsigned int block = bitIndex / ( sizeof(bitblock) * 8 );
        const unsigned int shift = bitIndex % ( sizeof(bitblock) * 8 );
        if(value) // set to 1
            m_Blocks[block] |= 1 << shift;
        else // set to 0
            m_Blocks[block] = m_Blocks[block] ^ (m_Blocks[block] & (1 << shift));
    }

    /**
     * Operator []
     * Read-only access to bits with []
     */
    const int operator [] (const int bitIndex) const
    {
        const unsigned int block = bitIndex / ( sizeof(bitblock) * 8 );
        const unsigned int shift = bitIndex % ( sizeof(bitblock) * 8 );
        return m_Blocks[block] >> shift & 1;
    }

// ===== DEBUG

#ifdef DEBUG
    void debug(bool printContent)
    {
        cout << "DEBUG:\n";
        cout << "   our block size: " << blockByteSize() << "bytes, " << blockBitSize() << "bits\n";
        cout << "requested bitsize: " << m_BitSize << "\n";
        cout << "        allocated: " << m_BlockCount << "blocks ("
             << blockByteSize()*m_BlockCount << "bytes, " << blockBitSize()*m_BlockCount << "bits)\n";

        if(printContent) {
            cout << " content: ";
            for(unsigned int i=0; i<getSize(); i++)
                cout << getBit(i);
            cout << "\n";
        }
    }
#endif // DEBUG

private:

/*
 * Allocation moved to constructor since there will be no support to
 * multi-dimensional functionality
    void allocate(const unsigned int blockCount) {
        m_BlockCount = blockCount;
        m_Blocks = new bitblock[m_BlockCount];
        ::memset(m_Blocks, 0, m_BlockCount);
    }
 */

    unsigned int        m_BitSize;      // number of the bits requested for allocation
    unsigned int        m_BlockCount;   // number of allocated blocks
    bitblock*           m_Blocks;       // blocks holding the actual bits
};

#endif // __BITARRAY_H__
