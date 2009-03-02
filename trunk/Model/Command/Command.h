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
        MOVING      = 1 << 0,   // moving etc
        HOSTILE     = 1 << 1,   // attack
        FRIENDLY    = 1 << 2    // mine ore, fix etc, can be split to more spesific types
    };

    Command(Target t, int flags, int range);

    inline int getFlags() { return m_Flags; }

    inline Target* getTarget() { return &m_Target; }

    /**
     * Get range value for target
     * Range means minimum distance that needs to be maintained
     * to the target.
     */
    inline unsigned short getRange() { return m_Range; }

    /**
     * Specify minimum range that needs to be maintained with the target
     * int terrain squares.
     */
    inline void setRange(float range) { m_Range = range; }

private:

    Target          m_Target(); // target of the command
    float           m_Range;    // minimum range to be maintained to the target

};

#endif // __COMMAND_H__
