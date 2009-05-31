/**
 * Static class used to create assets to the game.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __ASSETFACTORY_H__
#define __ASSETFACTORY_H__


//Forward declarations
class IAsset;
class Unit;
class Building;
class OreMine;
class MoneyPlant;
class RadarBuilding;
class Player;

class AssetFactory
{
public:

    static const int UNIT_TYPE_DEBUG = 0;
    static const int BUILDING_TYPE_DEBUG = 0;

    /**
     * Creates new asset based on given type and sets the parameters based
     * on it
     * @return pointer to new Asset instance
     */
    static IAsset* createAsset( Player* owner,
                                int assetType,
                                short positionX,
                                short positionY);

        /**
     * Creates new asset based on given type and sets the parameters based
     * on it
     * @param free if true, builds asset without taking payment for it
     *             please note that we don't care here if player can afford
     *             this asset or not, we just reduce it's cost
     * @return pointer to new Asset instance
     */
    static IAsset* createAsset( Player* owner,
                                int assetType,
                                short positionX,
                                short positionY,
                                bool free);

    /**
     * Creates new unit, sets the parameters against unitType
     * @return pointer to new Unit instance
     */
    static Unit* createUnit(Player* owner,
                            int unitType,
                            short positionX,
                            short positionY);

    /**
     * Creates new unit, sets the parameters against unitType
     * @param free if true, builds unit without taking payment for it
     * @return pointer to new Unit instance
     */
    static Unit* createUnit(Player* owner,
                            int unitType,
                            short positionX,
                            short positionY,
                            bool free);

    static Unit* createUnit(Player* owner,
                            int unitType,
                            Building* building);

    /**
     * Creates new building, sets the parameters against buildingType
     * @return pointer to new Building instance
     */
    static Building* createBuilding(Player* owner,
                                    int buildingType,
                                    short positionX,
                                    short positionY);

    /**
     * Creates new building, sets the parameters against buildingType
     * @param free if true, builds building without taking payment for it
     * @return pointer to new Building instance
     */
    static Building* createBuilding(Player* owner,
                                    int buildingType,
                                    short positionX,
                                    short positionY,
                                    bool free);

    /**
     * Creates new ore mine, this is a special case building which should never be
     * created by players, and should be only created at the beginning of the game
     * @return pointer to new OreMine-instance
     */
    static OreMine* createOreMine(short positionX, short positionY);

    /**
     * Creates deep bore mine, this is a special case building which produces
     * ore to it's owner by itself
     */
    static MoneyPlant* createDeepBoreMine(Player* owner, short positionX, short positionY);

    /**
     * Creates radar building that has ability to enable/disable player minimap
     */
    static RadarBuilding* createRadarBuilding(Player* owner, short positionX, short positionY);

private:

    /**
     * Private ctor and dtor for static class
     */
    AssetFactory();
    ~AssetFactory();

    /**
     * Set components based on asset's definition
     */
    static bool setRadar(IAsset* a);
    static bool setBuilder(IAsset* a);
    static bool setResourcer(IAsset* a);
    static bool setWeapon(IAsset* a);
    static bool setMoving(Unit* u);

};

#endif // __ASSETFACTORY_H__
