/**
 * IGameConsoleParser
 *
 * Parser that can handle lines from console and print some response to it.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

//TODO: This class is unfinished, STC

#ifndef __IGAMECONSOLEPARSER_H__
#define __IGAMECONSOLEPARSER_H__

#include "GameConsole.h"
#include "../Common/JString.h"

class GameConsole
{
public:

    IGameConsoleParser(void)
    {
    }

    virtual ~IGameConsoleParser(void)
    {
    }

    /**
     * Handle input line
     */
    virtual void handleInput(JString* trigger, JString inputLine) = 0;

};

#endif // __IGAMECONSOLEPARSER_H__
