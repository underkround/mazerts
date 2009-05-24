/**
 * Messagebuffer to store messages with level
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "Common/DoubleLinkedList.h"
#include "Common/JString.h"

enum CONSOLE_MSG_LEVEL
{
    MSG_LEVEL_DEBUG = 0,
    MSG_LEVEL_NOTIFY,
    MSG_LEVEL_ERROR
};


struct ConsoleMsg
{
    ConsoleMsg(CONSOLE_MSG_LEVEL l, JString* m)
        : level(l), msg(m)
    {
    }

    CONSOLE_MSG_LEVEL level;

    JString* msg;

};


class Console
{

public:

    static Console* getInstance();

    ListNode<ConsoleMsg*>* getHead();
    ListNode<ConsoleMsg*>* getTail();

    void clear();

    static void debug(const char* str);
    static void debug(JString* str);

    static void out(const char* str);
    static void out(JString* str);

    static void error(const char* str);
    static void error(JString* str);

    static void notify(const char* str);
    static void notify(JString* str);

    int getMaxSize() const
    {
        return m_bufferMaxSize;
    }

    int getCurrentSize() const
    {
        return m_buffer.count();
    }

    /**
     * Ownership is transferred to this
     */
    void add(JString* msg, CONSOLE_MSG_LEVEL level);

    /**
     * Ownership is transferred to this
     */
    void add(ConsoleMsg* msg);

private:

    Console();
    ~Console();
    Console(const Console& c) { }

    int m_bufferMaxSize;
    DoubleLinkedList<ConsoleMsg*> m_buffer;

};

#endif // __CONSOLE_H__
