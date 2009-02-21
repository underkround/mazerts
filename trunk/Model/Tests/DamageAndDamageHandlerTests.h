#include "../Unit/Damage.h"
#include "../Unit/DamageHandler.h"
#include <assert.h>

void testDamageAndDamageHandler()
{
    Damage* pDamage = new Damage();
    
    //Test that damage-values for all types are 0
    for(int i = 0; i < Damage::DAMAGETYPE_AMOUNT; i++)
    {
        assert(pDamage->getDamage(i) == 0);
    }

    //Set damage-values
    pDamage->setDamage(Damage::KINETIC, 10);
    pDamage->setDamage(Damage::EXPLOSIVE, 10);
    pDamage->setDamage(Damage::ENERGY, 10);
    
    //Test that damage-values for all types are 10
    for(int i = 0; i < Damage::DAMAGETYPE_AMOUNT; i++)
    {
        assert(pDamage->getDamage(i) == 10);
    }

    //Delete the instance
    delete pDamage;

    pDamage = new Damage();
    
    //Set damage-values
    for(int i = 0; i < Damage::DAMAGETYPE_AMOUNT; i++)
    {
       pDamage->setDamage(i, 10);
    }
    
    //Test that damage-values for all types are 10
    for(int i = 0; i < Damage::DAMAGETYPE_AMOUNT; i++)
    {
        assert(pDamage->getDamage(i) == 10);
    }

    //Create handler
    DamageHandler* pDmgHandler = new DamageHandler();

    //All resistances should be 1
    const float* pResistance = pDmgHandler->getResistance();
    for(int i = 0; i < Damage::DAMAGETYPE_AMOUNT; i++)
    {
        assert(pResistance[i] == 1.0f);
    }

    //Handle the Damage-object
    pDmgHandler->filterDamage(pDamage);

    //Values should not change, damage handler resistances default to 1.0
    for(int i = 0; i < Damage::DAMAGETYPE_AMOUNT; i++)
    {
        assert(pDamage->getDamage(i) == 10);
    }

    //Set resistances using array
    float damageRes[Damage::DAMAGETYPE_AMOUNT] = {0.1f, 0.2f, 0.3f};
    pDmgHandler->setResistance(damageRes, 3);

    //Test that all values are set correctly
    pResistance = pDmgHandler->getResistance();
    assert(pResistance[0] == 0.1f);
    assert(pResistance[1] == 0.2f);
    assert(pResistance[2] == 0.3f);

    //Test that damagehandler changes damage values properly
    pDmgHandler->filterDamage(pDamage);
    
    assert(pDamage->getDamage(0) == 1);
    assert(pDamage->getDamage(1) == 2);
    assert(pDamage->getDamage(2) == 3);
    assert(pDamage->getTotalDamage() == 6);

    //Delete Damage- and DamageHandler-objects
    delete pDamage;
    delete pDmgHandler;

    //Recreate damage
    pDamage = new Damage();

    //Create DamageHandler with the parametric constructor
    pDmgHandler = new DamageHandler(damageRes, Damage::DAMAGETYPE_AMOUNT);

    //Check that values are correct
    pResistance = pDmgHandler->getResistance();
    assert(pResistance[0] == 0.1f);
    assert(pResistance[1] == 0.2f);
    assert(pResistance[2] == 0.3f);

    delete pDamage;
    delete pDmgHandler;
    delete pResistance;
}