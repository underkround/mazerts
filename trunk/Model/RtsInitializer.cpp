#include "RtsInitializer.h"

#include "Common/Config.h"
#include "Terrain/Terrain.h"
#include "Terrain/AntinTerrainGenerator.h"
#include "Terrain/ImageTerrainGenerator.h"
#include "Terrain/PlainTerrainGenerator.h"
#include "Asset/AssetCollection.h"
#include "Asset/AssetFactory.h"

#include "Player/PlayerManager.h"
#include "Player/Fog.h"

#include "Defs/DefManager.h"

#include "PathFinding/PathFinderMaster.h"
#include "Weapon/ProjectileCollection.h"
#include "Weapon/ExplosionCollection.h"

#include <string>

const int RtsInitializer::RandInt(const int iLow, const int iHigh)
{
    return (::rand() % ((iHigh + 1) - iLow) + iLow);
}


void RtsInitializer::loadScenario(const char* filename)
{
    // path
    string path = "../data/scenarios/";
    // use the default is none given
    if(strlen(filename) == 0)
        path += "debug.ini";
    else
        path += filename;
    Config::getInstance()->setFilename(path);
    Config::getInstance()->readFile();
}


// ---------------------------------------------------------------------------


/**
 * Do your developement-time testing initializations here
 *
 */
#ifndef __RELEASE__
const bool RtsInitializer::initializeDebug()
{
    // Load definition files
    DefManager::getInstance()->loadConfigurations();

    Terrain* pTerrain = Terrain::getInstance();
    AntinTerrainGenerator* pGenerator = new AntinTerrainGenerator(100, 512);
    //ImageTerrainGenerator* pGenerator = new ImageTerrainGenerator("../data/terrains/mapx.bmp");// :P

    // for zemm's slow computer's local override
    if(!Config::getInstance()->getValueAsBool("debug skip terrain generating", false))
        pTerrain->initialize(pGenerator);

    // initialize asset collection
    AssetCollection::create(pTerrain->getSize());

    //Initialize projectile collection
    ProjectileCollection::create();

    //Initialize explosion collection
    ExplosionCollection::create();

    // Initialize player manager
    PlayerManager::create(4);

    // Get the pathfinder running
    PathFinderMaster::getInstance()->start();


    // TEST - UNIT CREATION
    Player* pCurrentPlayer = PlayerManager::getPlayer(1);
    if(!Config::getInstance()->getValueAsBool("debug skip starting units", false))
    {
        //Units
        for(int i = 0; i < 100; i++)
        {
            int plr = 1;
            int posx = RandInt(20, Terrain::getInstance()->getSize() - 20);
            int posy = RandInt(20, Terrain::getInstance()->getSize() - 20);
            if(i < 25)
                plr = 1;                
            else if(i < 50)
                plr = 2;            
            else if(i < 75)
                plr = 3;
            else
                plr = 4;
            AssetFactory::createUnit(PlayerManager::getPlayer(plr), (rand() % 6) + 1, posx, posy, true);
        }

        //Buildings
        for(int i = 0; i < 30; i++)
        {
            int plr = (rand() % 4) + 1;
            int posx = RandInt(20, Terrain::getInstance()->getSize() - 20);
            int posy = RandInt(20, Terrain::getInstance()->getSize() - 20);

            AssetFactory::createBuilding(PlayerManager::getPlayer(plr), 52 + rand() % 3, posx, posy, true);
        }

        //Mines
        for(int i = 0; i < 10; i++)
        {
            int posx = RandInt(20, Terrain::getInstance()->getSize() - 20);
            int posy = RandInt(20, Terrain::getInstance()->getSize() - 20);

            //AssetFactory::createOreMine(posx, posy);
            AssetFactory::createAsset(PlayerManager::getPlayer(0), 51, posx, posy);
        }

        // some kind of starting base for player 1
        for (int i = 0; i < 5; i++)
        {
            AssetFactory::createUnit(pCurrentPlayer, 1, 50 + i * 7, 10, true);
            AssetFactory::createUnit(pCurrentPlayer, 2, 50 + i * 7, 17, true);
            AssetFactory::createUnit(pCurrentPlayer, 3, 50 + i * 7, 24, true);
            //AssetFactory::createUnit(pCurrentPlayer, 4, 50 + i * 7, 31, true);
            AssetFactory::createUnit(pCurrentPlayer, 5, 50 + i * 7, 31, true);
            AssetFactory::createUnit(pCurrentPlayer, 6, 50 + i * 7, 38, true);
        }
        AssetFactory::createBuilding(pCurrentPlayer, 54, 15, 15, true);
        AssetFactory::createBuilding(pCurrentPlayer, 54, 30, 15, true);
        AssetFactory::createBuilding(pCurrentPlayer, 52, 15, 30, true);
        AssetFactory::createBuilding(pCurrentPlayer, 53, 30, 30, true);

        //AssetFactory::createOreMine(15, 80);
        AssetFactory::createAsset(PlayerManager::getPlayer(0), 51, 15, 80);

    }
    else 
    {
        pCurrentPlayer->getFog()->setEnabled(false);
    }




    return true;
}
#endif // ! __RELEASE__


// ---------------------------------------------------------------------------


const bool RtsInitializer::initializeScenario()
{
    Config* c = Config::getInstance();

    // check for loaded scenario
    if(!c->hasSection("scenario"))
    {
        // let's use the debug init, unless the release version
#ifdef __RELEASE__
        return false; // TODO: should it be empty game instead of failing?..
#else
        return initializeDebug();
#endif
    }

    // Load definition files
    DefManager::getInstance()->loadConfigurations();

    /*
     * TERRAIN
     * validate scenario confs & initialize the terrain
     */
    Terrain* pTerrain = Terrain::getInstance();
    string generator = c->getValueAsString("scenario", "scenario terrain generator", ""); // TODO
    ITerrainGenerator* pGenerator = NULL;

    // choose the generator
    // this is nowhere near elegant :D
    if(generator == "bmp")
    {
        string mapfile = "../data/terrains/";
        mapfile += c->getValueAsString("scenario", "scenario terrain params", "default.bmp");
        pGenerator = new ImageTerrainGenerator(mapfile.c_str());
    }
    else if(generator == "anthex")
    {
        Config::ValueNode* paramNode = c->getNode("scenario", "scenario terrain params");
        int seed = (paramNode) ? paramNode->getInt() : 0;
        if(paramNode) paramNode = paramNode->next;
        int size = (paramNode && paramNode->getInt() > 0) ? paramNode->getInt() : 256;
        pGenerator = new AntinTerrainGenerator(seed, size);
    }
    else if (generator == "plain")
    {
        int size = c->getValueAsInt("scenario", "scenario terrain params", 512);
        pGenerator = new PlainTerrainGenerator(size);
    }

    // apply the generator
    pTerrain->initialize(pGenerator);

    /*
     * COLLECTION INITIALIZATION
     */
    // initialize asset collection
    AssetCollection::create(pTerrain->getSize());
    //Initialize projectile collection
    ProjectileCollection::create();
    //Initialize explosion collection
    ExplosionCollection::create();

    // Get the pathfinder running
    PathFinderMaster::getInstance()->start();

    /*
     * PLAYERS
     * validate scenario confs & initialize players
     */
    int players = c->getValueAsInt("scenario", "scenario player count", 2);
    // Initialize player manager
    PlayerManager::create(players);

    if(players > 0 && c->getValueAsBool("scenario", "scenario disable fog", false))
    {
        // disable fog for players if set in ini
        for(int i=0; i<=PlayerManager::getPlayerCount(); i++)
            PlayerManager::getPlayer(i)->getFog()->setEnabled(false);
    }

    //give players ore they deserve
    if(players > 0)
    {
        for(int i=0; i<=PlayerManager::getPlayerCount(); i++)
        {
            string strOre = "scenario player ";
            strOre += Config::intToString(i);
            strOre += " ore";
            int iOre = c->getValueAsInt("scenario", strOre, 0);
            PlayerManager::getPlayer(i)->setOre(iOre);
        }
    }

    /*
     * ASSETS
     * validate scenario confs & initialize assets
     */
    unsigned short mapSize = Terrain::getInstance()->getSize();
    Player* pPlayer;
    AssetDef* assetDef;
    // loop through scenario's assets and create them
    for(int playerNum=0; playerNum<=PlayerManager::getPlayerCount(); playerNum++)
    {
        string section = "scenario assets ";
        section += Config::intToString(playerNum);
        if(c->hasSection(section))
        {
            pPlayer = PlayerManager::getPlayer(playerNum);
            // create defined assets
            Config::ValueNode* typeNode = c->getNode(section, "scenario asset type");
            Config::ValueNode* posxNode = c->getNode(section, "scenario asset posx");
            Config::ValueNode* posyNode = c->getNode(section, "scenario asset posy");
            // all values need to exist
            while(typeNode && posxNode && posyNode)
            {
                assetDef = DefManager::getInstance()->getAssetDef(typeNode->getInt());
                // validate values
                if(
                    assetDef &&
                    (posxNode->getInt() >= 0 && posxNode->getInt() < (mapSize - assetDef->width)) &&
                    (posyNode->getInt() >= 0 && posyNode->getInt() < (mapSize - assetDef->height))
                )
                {
                    AssetFactory::createAsset(pPlayer, assetDef->tag, (short)posxNode->getInt(), (short)posyNode->getInt(), true);
                }
                // advance nodes
                typeNode = typeNode->next;
                posxNode = posxNode->next;
                posyNode = posyNode->next;
            }
        }
    }

    // TODO: set player resources

    // TODO: victory conditions?

    return true;
}


// ---------------------------------------------------------------------------


void RtsInitializer::release()
{
    //Tell the PathFinder-thread to stop and wait for it to finish
    if(PathFinderMaster::getInstance()->isRunning())
    {
        PathFinderMaster::getInstance()->stop();
        PathFinderMaster::getInstance()->wait();
    }

    //The children of the Object Manager root object are listeners to model-side
    //AssetCollection, and will be released via listener-interface, but they have
    //to be released first (order matters!)
    AssetCollection::releaseAll();

    // release fog
    Fog::release();

    // release player manager
    PlayerManager::release();

    // release the terrain
    Terrain::getInstance()->release();
}
