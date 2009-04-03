#include "ProjectileCollection.h"

DoubleLinkedList<Projectile*>* ProjectileCollection::pProjectileList = NULL;
ProjectileCollection::UICallBack ProjectileCollection::pCreationCallback = NULL;

void ProjectileCollection::create()
{
    if(pProjectileList)
    {
        ListNode<Projectile*>* node = pProjectileList->headNode();    
        while(node)
        {
            delete node->item;
            node = node->next;
        }

        pProjectileList->release();
    }
    pProjectileList = new DoubleLinkedList<Projectile*>();
}

void ProjectileCollection::update(const float deltaTime)
{
    ListNode<Projectile*>* node = pProjectileList->headNode();
    
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