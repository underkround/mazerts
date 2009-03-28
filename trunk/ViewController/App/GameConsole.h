/**
 * GameConsole
 *
 * GameConsole is a singleton holder for application-wide stringbuffer
 * that stores messages from different sources, and can be used interactive
 * with registered parsers.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

//TODO: This class is unfinished, STC

#ifndef __GAMECONSOLE_H__
#define __GAMECONSOLE_H__

#include <tchar.h>
#include "ID3DApplication.h"

#define CONSOLE_BUFFERLENGTH 10

class GameConsole
{
public:

    /**
     * Get the singleton instance
     */
    static GameConsole* getInstance(void);

    static void input(TCHAR input);

    static TCHAR* output();

    /**
     * Clear messages
     */
    static void clear(void);

    /**
     * perform command
     */
    static void parse();

    inline int getSize() { return CONSOLE_BUFFERLENGTH; }

protected:

    GameConsole(void);

    ~GameConsole(void);

private:
// *** MEMBERS ***

    static TCHAR m_arrMessageBuffer[CONSOLE_BUFFERLENGTH][128];
    static TCHAR m_arrMessage[128];
    static int m_MessageLength;
    static GameConsole* pInstance;
};

#endif // __GAMECONSOLE_H__
