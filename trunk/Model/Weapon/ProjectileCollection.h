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

#include "Projectile.h"
#include "../Common/DoubleLinkedList.h"

class ProjectileCollection
{
public:

    /**
     * Function-pointer to send information to UI about created projectiles (only certain types)
     */
    typedef void (*UICallBack)(Projectile* pProjectile);    

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
    inline static void addProjectile(Projectile* pProjectile) 
    {
        if(pProjectile->getConcreteType() == Projectile::SHELL && pCreationCallback)
        {
            pCreationCallback(pProjectile);
        }
        pProjectileList->pushTail(pProjectile); 
    }

    /**
     * Gets the list within the collection (for testing purposes)
     */
    inline static DoubleLinkedList<Projectile*>* getList() { return pProjectileList; }
    
    /**
     * Sets the creation callback for UI
     */
    inline static void setCreationCallback(UICallBack callback) { pCreationCallback = callback; }

private:

    /**
     * List of projectiles within collection
     */
    static DoubleLinkedList<Projectile*>* pProjectileList;

    static UICallBack pCreationCallback;
};

#endif //__PROJECTILECOLLECTION_H__