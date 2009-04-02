#include "ExplosionCollection.h"

DoubleLinkedList<Explosion*>* ExplosionCollection::pExplosionList = NULL;
ExplosionCollection::UICallBack ExplosionCollection::callBack = NULL;

void ExplosionCollection::create()
{
    if(pExplosionList)
    {
        ListNode<Explosion*>* node = pExplosionList->headNode();    
        while(node)
        {
            delete node->item;
            node = node->next;
        }

        pExplosionList->release();
    }
    pExplosionList = new DoubleLinkedList<Explosion*>();
}

void ExplosionCollection::update(const float deltaTime)
{
    ListNode<Explosion*>* node = pExplosionList->headNode();
    
    while(node)
    {
        node->item->update();
        if(callBack)
        {
            callBack(node->item);
        }
        delete node->item;
        node = pExplosionList->removeGetNext(node);
    }
}