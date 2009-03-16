/**
 * Helper class to use when giving commands to the assets.
 *
 * Purposes of this class:
 *  - provide simultaneous commanding-control over multiple assets
 *  - receiving hints for possible actions with target (can be used by ui)
 *
 * I guess this will do for now 
 *
 *
 * TODO: check the comments! some of them might be inaccurate since this class
 * has changed many times =P
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __UNITCOMMANDDISPATCHER_H__
#define __UNITCOMMANDDISPATCHER_H__

#include "Target.h"
//#include "Command.h"
#include "../Asset/Unit.h"
#include "../Common/DoubleLinkedList.h"

class Player;

class UnitCommandDispatcher
{
public:

    UnitCommandDispatcher(Player* owner);

    ~UnitCommandDispatcher();

    void release();

// ===== Subject control - subject is asset currently under control

    /**
     * Add asset that is meant to handle the command.
     * Used when player adds asset to control, or selects
     * multiple assets (call clearSubjects() first for new selection).
     */
    void addUnit(Unit* unit);

    /**
     * Set unit that is meant to handle the command,
     * remove previous subjects if any.
     * Used when user selects new unit to control.
     */
    void setUnit(Unit* unit);

    /**
     * Remove any subjects that are meant to handle the command.
     * Used when player deselects all units.
     */
    void clearUnits();

    /**
     * Return current subjects, assets that are currently handling the
     * possibly executing command.
     */
    DoubleLinkedList<Unit*>* getUnits() { return &m_Units; }

    /**
     * @return true, if there are currently units as subjects for commands
     */
    bool empty() { return m_Units.empty(); }

    /**
     * Indication for does the current selection handle target commands.
     * @DEPRECATED: this is changed to unit-only dispatcher, and
     * all units take targets
     */
//    bool subjectsTakeTargets();

// ===== Target and command functionality

    /**
     * Return the target that is currently set.
     * This is intented to use also when updating the target before
     * execution.
     */
    Target* getTarget();

    /**
     * Get command-flags associated with the current target.
     * @see Command::CommandFlag
     */
    int getCommandFlagsForTarget();

    /**
     * Dispatch the current command to currently selected assets (subjects).
     * If no assets are selected or current command is somehow false, this
     * return false.
     */
//    bool dispatch(unsigned short terrainX, unsigned short terrainY);

    /**
     * Dispatch the current command to currently selected assets (subjects)
     * and set given flags to it.
     */
//    bool dispatch(int commandFlags);

// TODO: these methods are for testing with current version which does not
// support commands, remove or modify when commands are implemented!
    bool dispatch(Target* target);

    /**
     * Dispatch current target
     * @param addToQueue If true, the command is added to the units' queue, otherwise the commandqueue
     *                   is cleared before adding
     */
    bool dispatch(bool addToQueue);

private:

    // current player to determine which are friendly assets
    Player*                     m_Owner;

    // Target to use with command
    Target                      m_Target;

    // current command being dispatched
//    Command*                    m_pCommand;

    // assets that are subject to control atm
    DoubleLinkedList<Unit*>     m_Units;

};

#endif // __UNITCOMMANDDISPATCHER_H__
