/**
 * ConsoleBuffer
 *
 * Ringbuffer for console JStrings and their levels
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __CONSOLEBUFFER_H__
#define __CONSOLEBUFFER_H__

#include "ConsoleBufferIterator.h"

class RingBufferS
{
public:

// ===== CONSTANTS

    /**
     * The default size of the buffer in lines
     */
    static const short DEFAULT_SIZE = 100;

// ===== CONSTRUCTOR & DESTRUCTOR

    ConsoleBuffer(const unsigned short sizeLines);

    virtual ~ConsoleBuffer(void);

// ===== USAGE & SETTERS

    void clear(void);

    void resize(const unsigned short sizeLines);

    void addLine(JString line, unsigned short tags);

// ===== GETTERS

    inline const int getSize(void) const
    {
        return m_Size;
    }

    /**
     * Create and return iterator for stringbuffer.
     * The user of the iterator is responsible for destroying
     * it.
     */
    inline IConsoleBufferIterator* getIterator(void)
    {
    }

protected:

    /**
     * Size of the buffer in lines
     */
    unsigned short m_Size;

    /**
     * Buffer for strings
     */
    JString*                m_LineBuffer;

    /**
     * Buffer for message flags
     */
    unsigned short*         m_FlagBuffer;

};

#endif // __CONSOLEBUFFER_H__
