/**
 * GameConsole
 *
 * GameConsole is a singleton holder for application-wide stringbuffer
 * that stores messages from different sources, and can be used interactive
 * with registered parsers.
 *
 * $Revision: 33 $
 * $Date: 2009-02-16 03:03:31 +0200 (ma, 16 helmi 2009) $
 * $Id: GameConsole.h 33 2009-02-16 01:03:31Z zem $
 */

//TODO: This class is unfinished, STC

#ifndef __GAMECONSOLE_H__
#define __GAMECONSOLE_H__

#include <tchar.h>
#include "ID3DApplication.h"

#define CONSOLE_LENGTH 10

class GameConsole
{
public:

    /**
     * Get the singleton instance
     */
    static GameConsole* getInstance(void);

    void input(char* input);

    /**
     * Clear messages
     */
    void clear(void);

    /**
     * print messages to screen
     * MUST BE BETWEEN BeginText and EndText!
     */
    void drawLines(int bottom);

protected:

    GameConsole(void);

    ~GameConsole(void);

// *** MEMBERS ***

    static TCHAR m_arrMessages[128][CONSOLE_LENGTH];
    static GameConsole* pInstance;
};

#endif // __GAMECONSOLE_H__
