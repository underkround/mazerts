#include "Console.h"

Console* Console::getInstance()
{
    static Console instance;
    return &instance;
}

Console::~Console()
{
}

Console::Console() {
    m_bufferMaxSize = 20;
}

void Console::add(JString* msg, CONSOLE_MSG_LEVEL level)
{
    add(new ConsoleMsg(level, msg));
}

void Console::add(ConsoleMsg* msg)
{
    while(m_buffer.count() > m_bufferMaxSize) {
        ConsoleMsg* toRemove = m_buffer.popTail();
        delete toRemove->msg;
    }
    m_buffer.pushHead(msg);
}

void Console::clear()
{
    while(ConsoleMsg* toRemove = m_buffer.popTail()) {
        delete toRemove->msg;
    }
}

ListNode<ConsoleMsg*>* Console::getHead()
{
    return m_buffer.headNode();
}

ListNode<ConsoleMsg*>* Console::getTail()
{
    return m_buffer.tailNode();
}


void Console::debug(JString* str) {
    Console* c = getInstance();
    c->add(str, MSG_LEVEL_DEBUG);
}

void Console::out(JString* str) {
    Console* c = getInstance();
    c->add(str, MSG_LEVEL_NOTIFY);
}

void Console::error(JString* str) {
    Console* c = getInstance();
    c->add(str, MSG_LEVEL_ERROR);
}

void Console::notify(JString* str) {
    Console* c = getInstance();
    c->add(str, MSG_LEVEL_NOTIFY);
}



void Console::debug(const char* str) {
    debug(new JString(str));
}
void Console::out(const char* str)  {
    out(new JString(str));
}
void Console::error(const char* str) {
    error(new JString(str));
}
void Console::notify(const char* str) {
    notify(new JString(str));
}
