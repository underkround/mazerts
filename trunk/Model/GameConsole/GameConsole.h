/**
 * GameConsole
 *
 * GameConsole is a singleton holder for application-wide stringbuffer
 * that stores messages from different sources, and can be used interactive
 * with registered parsers.
 *
 * Implementation notes:
 *
 * hashmap triggerMap
 *  [key: trigger word] -> [pointer to handler]
 *
 * stringbuffer for log
 *
 * $Revision$
 * $Date$
 * $Id$
 */

//TODO: This class is unfinished, STC

#ifndef __GAMECONSOLE_H__
#define __GAMECONSOLE_H__

#include "../Common/JString.h"

class GameConsole
{
public:

    /**
     * Message levels, set as bits plz
     */
    enum MessageLevel
    {
        MESSAGE  = 1,   // the default message level, can be anything
        RESPONSE = 2,   // response to input line
        INPUT    = 4,   // user input
        ERROR    = 8,   // "red messages"
        DEBUG    = 16   // these should not exist in release version at all
    };

    /**
     * Get the singleton instance
     */
    static GameConsole* getInstance(void);

// ===== MESSAGES

    /**
     * Log a message
     * @param message   The message line to store to the buffer
     */
    void log(JString message);
    void log(char* message);

    /**
     * Log a message with level
     * @param message   The message line to store to the buffer
     * @param level     The message level to flag the line with
     */
    void log(JString message, MessageLevel level);
    void log(char* message, MessageLevel level);

    /**
     * Input a line that is parsed through parsers
     */
    void input(JString input);
    void input(char* input);

// ===== BUFFER

    /**
     * Get an iterator that can be used to check and display for new
     * messages as well as iterating through all the messages in the buffer.
     */
    StringBufferIterator* getIterator(void); // return iterator for buffer

    /**
     * Clear messages in the buffer
     */
    void clear(void);

    /**
     * Resize the message buffer
     * Buffer content can be lost when calling this.
     */
    void resize(unsigned const short lines);

// ===== PARSERS

// NOTE: do this another way around, parser should register to console,
// not so that the console would get the trigger words... ?
//    void addParser(IGameConsoleParser* parser);

    /**
     * Associate trigger word with parser.
     * The trigger is matched to the first word in the command.
     * If the command matches the trigger, the input line is passed
     * to the handler for processing.
     * @param trigger   trigger for the first word in the input line
     * @param handler   pointer to the parser that matched line is passed
     * @return          false if the keyword is already registered by another
     *                  handler
     */
    bool bindTrigger(JString trigger, IGameConsoleParser* pHandler);

    /**
     * Unbind trigger-word, if it's found from triggers.
     * @param trigger   the word to unbind
     * @return          true if the trigger word had bind
     */
    bool unbindTrigger(JString trigger);

protected:

    GameConsole(void);

    ~GameConsole(void);

// ===== MEMBERS



};

#endif // __GAMECONSOLE_H__
