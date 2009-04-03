/**
 * ExplosionCollection
 *
 * Keeps track of "live" explosions and handles updating them
 *
 * $Revision:$
 * $Date:$
 * $Id:$
 */

#ifndef __EXPLOSIONCOLLECTION_H__
#define __EXPLOSIONCOLLECTION_H__

#include "Explosion.h"
#include "../Common/DoubleLinkedList.h"

class ExplosionCollection
{
public:

    /**
     * Function-pointer to send information to UI about the explosion
     */
    typedef void (*UICallBack)(Explosion* pExplosion);

    /**
     * Creates the collection
     */
    static void create();

    /**
     * Updates the explosions within the collection
     * @param deltaTime Time passed since last call (seconds)
     */
    static void update(const float deltaTime);

    /**
     * Adds the given explosion to collection
     */
    inline static void addExplosion(Explosion* pExplosion) { pExplosionList->pushTail(pExplosion); }

    /**
     * Gets the list within the collection (for testing purposes)
     */
    inline static DoubleLinkedList<Explosion*>* getList() { return pExplosionList; }
    
    /**
     * Sets the callback-method (needs to be static) which gets called on explosion update
     */
    inline static void setCallBack(UICallBack method) { callBack = method; }

private:

    /**
     * List of explosions within collection
     */
    static DoubleLinkedList<Explosion*>* pExplosionList;

    /**
     * Callback to ui
     */
    static UICallBack callBack;
};

#endif //__EXPLOSIONCOLLECTION_H__