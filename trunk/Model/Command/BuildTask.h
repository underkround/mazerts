/**
 *
 * Usage:
 *  1. create using static method that does the initial validation
 *  2. initialize & validate settings
 *  3. start process
 *  4. delete task, or use it as status indicator
 *     (delete task when the process is completed since it's useless)
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __BUILDTASK_H__
#define __BUILDTASK_H__

#include "../Defs/Defs.h"

#include "../Asset/IAsset.h"

class Player;

class BuildTask
{
public:

    static bool canBuild(Player* pPlayer, int assetDefTag);
    static bool canBuild(Player* pPlayer, AssetDef* assetDef);

    static BuildTask* createTask(Player* pPlayer, int assetDefTag);
    static BuildTask* createTask(Player* pPlayer, AssetDef* assetDef);

    // before execute methods

    bool requiresPosition() const;

    bool canAfford() const;

    void setPosition(const unsigned short x, const unsigned short y)
    {
        //if(requiresPosition())
        // ..well, allow setting custom position with units etc assets build
        // inside others too (why not). do the check in controller instead
        m_PosX = x; m_PosY = y;
    }

    bool hasPosition() const
    {
        return (m_PosX > 0 && m_PosY > 0) ? true : false;
    }

    // is the set position free to build, or occupied?
    bool isFreeToBuild() const;

    /**
     * Returns -1 for unset values
     */
    unsigned short getPositionX() const { return m_PosX; }
    unsigned short getPositionY() const { return m_PosY; }

    /**
     * Start building, do the thing, return true if success
     */
    bool execute();

    // after execute methods

    bool isStarted() const
    {
        return m_Started;
    }

    bool isFinished() const;

    int getStatusPercentage();

private:

    BuildTask(Player* pPlayer, AssetDef* assetDef);

    ~BuildTask();

    Player*         m_pPlayer;
    AssetDef*       m_pAssetDef;

    unsigned short  m_PosX;
    unsigned short  m_PosY;

    // has the process been started
    bool            m_Started;
    // has the process finished
    bool            m_Finished;

    // asset that we are building
    IAsset*         m_pAsset;

};

#endif // __BUILDTASK_H__
