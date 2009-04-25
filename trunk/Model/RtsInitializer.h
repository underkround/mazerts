/**
 * This static class setups the game - initializes terrain and starting
 * assets, players, resources etc.
 *
 * It currently supports scenario-file or random debug-initing.
 *
 * Here are the stuffs previously in DXUI::GameState's create method.
 *
 * Reasons for moving those here:
 *  - to avoid bloating the gamestate too much when bringing the support
 *    for scenario files
 *  - it might bee good to keep the basic game initialization apart from
 *    the ui
 *
 * Note that the scenario-functionality assumes that the appropriate
 * scenario file is already loaded to config. If scenario is not found, use the
 * debug setup method.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __RTSINITIALIZER_H__
#define __RTSINITIALIZER_H__

class RtsInitializer
{
public:

    /**
     * Load a scenario file.
     * Filename needs to be basename (without path) of the file inside scenario-path.
     * Acts as wrapper to config::readFile hiding the scenario-path.
     */
    static void RtsInitializer::loadScenario(const char* filename);

    /**
     * Initialize the game based on scenario file loaded into the Config
     * object. If no valid scenario found do not initialize and return false.
     * @return  true, if scenario was loaded and initialized, otherwise false
     */
    static const bool initializeScenario();

    /**
     * Initialize the game based on hardcoded stuff.
     * You can do whatever you like in here, this should not be used in the
     * release version.
     */
#ifndef __RELEASE__
    static const bool initializeDebug();
#endif

    /** 
     * Do game related releasings, after which the game could be
     * re-initialized.
     */
    static void release();

private:

    static const int RandInt(const int iLow, const int iHigh);

};

#endif // __RTSINITIALIZER_H__
