/**
 * Command is currently a container for Target-object and flag-bits associated
 * with it.
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

    /**
     * Static command-type flags.
     * Actual flag can also be custom,
     */
    enum CommandFlag
    {
        MOVE        = 1 << 0,   // moving etc
        HOSTILE     = 1 << 1,   // attack
        FRIENDLY    = 1 << 2    // mine ore, fix etc, can be split to more spesific types
    };

    /**
     * Create new command..
     * @param target    Target for this command, can be set to NULL if none
     *                  NOTE: this will take the ownership of the target-object
     * @param flags     Flags associated with this command
     * @param range     Range for this command's target, can be set to 0
     */
    Command(Target* target, int flags, int range)
    {
        m_Target = target;
        m_Flags = flags;
        m_Range = range;
    }

    ~Command()
    {
        // if we have target attached, delete it
        if(m_Target)
            delete m_Target;
    }

    /**
     * Set flags for this command
     */
    inline void setFlag(int flag, bool value)
    {
        // set flag-bit according to value
        m_Flags = (value) ? m_Flags | flag : m_Flags ^ (m_Flags & flag);
    }

    /**
     * Return the current flags set to this command
     */
    inline int getFlags() {
        return m_Flags;
    }

    /**
     * Clear all flags from this command
     */
    inline int clearFlags() {
        m_Flags = 0;
    }

    /**
     * Return the current target associated with the command, or NULL
     * if none.
     */
    inline Target* getTarget() {
        return m_Target;
    }

    /**
     * Get range value for target
     * Range means minimum distance that needs to be maintained
     * to the target.
     */
    inline unsigned short getRange() {
        return m_Range;
    }

    /**
     * Specify minimum range that needs to be maintained with the target
     * int terrain squares.
     */
    inline void setRange(float range) {
        m_Range = range;
    }

private:

    Target*         m_Target;   // target of the command
    int             m_Flags;    // flags set to this command
    float           m_Range;    // minimum range to be maintained to the target

};

#endif // __COMMAND_H__
