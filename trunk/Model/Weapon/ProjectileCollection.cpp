#include "ProjectileCollection.h"

DoubleLinkedList<IProjectile*>* ProjectileCollection::pProjectileList = NULL;

void ProjectileCollection::create()
{
    if(pProjectileList)
    {
        ListNode<IProjectile*>* node = pProjectileList->headNode();    
        while(node)
        {
            delete node->item;
            node = node->next;
        }

        pProjectileList->release();
    }
    pProjectileList = new DoubleLinkedList<IProjectile*>();
}

void ProjectileCollection::update(const float deltaTime)
{
    ListNode<IProjectile*>* node = pProjectileList->headNode();
    
    while(node)
    {
        if (node->item->update(deltaTime))
        {
            node = node->next;
        }
        else 
        {            
            delete node->item;
            node = pProjectileList->removeGetNext(node);
        }
    }
}