#include <iostream>
#include <string>

#include "../Defs/Defs.h"
#include "../Defs/DefManager.h"

using namespace std;

void testDefLoading()
{
/*
    Config* c = Config::getInstance();
    c->setFilename("../data/defs/assets.ini");
    c->readFile();
//    Config::IntNode* node = c->getIntNode("", "", "declared assets");
    cout << c->getValueAsInt("", "def declarations", "declared assets") << endl;
    cout << c->getValueAsInt("", "a2", "asset name") << endl;
    cout << endl;
    return;
*/

    DefManager* dm = DefManager::getInstance();
    dm->loadConfigurations();

//    Config::getInstance()->printSettings();

    ListNode<AssetDef*>* node = dm->getAssetDefNode();
    AssetDef* a;
    while(node) {
        a = node->item;
        cout << "tag: " << a->tag << endl;
        cout << "concreteType: " << a->concreteType << endl;
        cout << "constructionCostEnergy: " << a->constructionCostEnergy << endl;
        cout << "constructionCostOre: " << a->constructionCostOre << endl;
        cout << "description: " << a->description << endl;
        cout << "destructionTime: " << a->destructionTime << endl;
        cout << "gridExitPointX: " << a->gridExitPointX << endl;
        cout << "gridExitPointY" << a->gridExitPointY << endl;
        cout << "height: " << a->height << endl;
        cout << "idleEnergyConsumption: " << a->idleEnergyConsumption << endl;
        cout << "maxHitpoints: " << a->maxHitpoints << endl;
        cout << "name: " << a->name << endl;
        cout << "width: " << a->width << endl;
        cout << " -- associated with: --" << endl;
        if(a->pDefBuilder) {
            cout << " builder: " << endl;
        }
        if(a->pDefMoving) {
            cout << " moving: " << endl;
        }
        if(a->pDefWeapon) {
            cout << " weapon: " << endl;
        }
        if(a->pDefRadar) {
            cout << " radar: " << endl;
        }
        if(a->pDefResourcer) {
            cout << " resourcer: " << endl;
        }
        cout << endl;
        node = node->next;
    }
/* */

};
