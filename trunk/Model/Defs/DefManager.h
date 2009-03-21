/**
 * Class that load, stores and creates property-structs
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __DEFMANAGER_H__
#define __DEFMANAGER_H__

#include "Defs.h"
#include "../Common/DoubleLinkedList.h"
#include <sstream>

using namespace std;

class DefManager
{
public:

    /**
     * @return singleton instance
     */
    static DefManager* getInstance()
    {
        static DefManager instance;
        return &instance;
    }

    /**
     * Load definitions from config files.
     * Can be used to reload changed values at runtime also.
     */
    void loadConfigurations();

    /**
     *
     */
    AssetDef* getAssetDef(int tag);
    ProjectileDef* getProjectileDef(int tag);
    RadarDef* getRadarDef(int tag);
    MovingDef* getMovingDef(int tag);
    BuilderDef* getBuilderDef(int tag);
    ResourcerDef* getResourcerDef(int tag);

    /**
     * Convert integer to string
     */
    string intToString(int in)
    {
        string s;
        stringstream out;
        out << in;
        s = out.str();
        return s;
    }

    /**
     * Get node for defs list with which you can iterate through
     * asset definitions loaded
     */
    ListNode<AssetDef*>* getAssetDefNode()
    {
        return m_AssetDefs.headNode();
    }

    bool hasAssetDef(int tag);
    bool hasMovingDef(int tag);
    bool hasProjectileDef(int tag);
    bool hasBuilderDef(int tag);
    bool hasResourcerDef(int tag);
    bool hasRadarDef(int tag);

private:

    /**
     * Private ctor & dtor for singleton
     */
    DefManager();
    ~DefManager();

    /**
     * Load individual definitions
     */
    bool loadAssetDef(int tag);
    bool loadMovingDef(int tag);
    bool loadProjectileDef(int tag);
    bool loadBuilderDef(int tag);
    bool loadResourcerDef(int tag);
    bool loadRadarDef(int tag);

    // TODO: change these to maps ?
    // not that important for now since there are too many of these anyway
    DoubleLinkedList<AssetDef*>         m_AssetDefs;
    DoubleLinkedList<MovingDef*>        m_MovingDefs;
    DoubleLinkedList<ProjectileDef*>    m_ProjectileDefs;
    DoubleLinkedList<BuilderDef*>       m_BuilderDefs;
    DoubleLinkedList<ResourcerDef*>     m_ResourcerDefs;
    DoubleLinkedList<RadarDef*>         m_RadarDefs;

};

#endif // __DEFMANAGER_H__
