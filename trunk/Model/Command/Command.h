/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __COMMAND_H__
#define __COMMAND_H__

class Command
{
public:

    enum Flag
    {
        NEUTRAL   = 1 << 0, // moving etc
        HOSTILE   = 1 << 1, // attacking
        FRIENDLY  = 1 << 2  // mine ore, fix etc
    };

    inline Target* getTarget();

private:

    Target m_Target();

};

#endif // __COMMAND_H__
