/**
 * ProjectileCollection
 *
 * Keeps track of "live" projectiles and handles updating them
 *
 * $Revision:$
 * $Date:$
 * $Id:$
 */

#ifndef __PROJECTILECOLLECTION_H__
#define __PROJECTILECOLLECTION_H__

#include "IProjectile.h"
#include "../Common/DoubleLinkedList.h"

class ProjectileCollection
{
public:

    /**
     * Creates the collection
     */
    static void create();

    /**
     * Updates the projectiles within the collection
     * @param deltaTime Time passed since last call (seconds)
     */
    static void update(const float deltaTime);

    /**
     * Adds the given projectile to collection
     */
    inline static void addProjectile(IProjectile* pProjectile) { pProjectileList->pushTail(pProjectile); }

    /**
     * Gets the list within the collection (for testing purposes)
     */
    inline static DoubleLinkedList<IProjectile*>* getList() { return pProjectileList; }
    

private:

    /**
     * List of projectiles within collection
     */
    static DoubleLinkedList<IProjectile*>* pProjectileList;
};

#endif //__PROJECTILECOLLECTION_H__