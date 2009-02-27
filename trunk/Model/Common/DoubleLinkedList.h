/**
 * Double linked list template class.
 * No iterator implemented, iterate by fetching head/tail -node and next/prev
 * from it.
 *
 * Does not, under any circumstances, delete the item-objects, so if you store
 * pointers, make sure to release them manually.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__

/**
 * Double linked list itemnode
 */
template <typename T>
class ListNode
{
public:
    ListNode(T t, ListNode* p, ListNode* n) : item(t), prev(p), next(n) { }
    T             item;
    ListNode<T>*  prev;
    ListNode<T>*  next;
};

/**
 * Double linked list
 */
template <typename T>
class DoubleLinkedList
{

public:

    DoubleLinkedList() : head( NULL ), tail ( NULL ) {}

    ~DoubleLinkedList();

    /**
     * Add item to the end or beginning of the list.
     */
    void pushHead(T);
    void pushTail(T);

    /**
     * Return first or last item, delete the node from the list.
     */
    T popHead();
    T popTail();

    /**
     * Return first or last item, node and item are left untouched.
     */
    T peekHead();
    T peekTail();

    /**
     * Get head or tail node with which you can iterate the list.
     */
    ListNode<T>* headNode() { return head; }
    ListNode<T>* tailNode() { return tail; }

    /**
     * Remove item and it's node and fix the list, if found from the list.
     * Deletes the node object, but not the item inside.
     * User is responsible for destroying the actual item object.
     * warning: iterates through the whole list, so might be slow.
     * If you are iterating the list already, use remove(ListNode) instead!
     * @return true, if item was found and removed
     */
    bool remove(T item);

    /**
     * Remove node and fix the list. Used while iterating nodes.
     * Deletes the node object, but not the item inside.
     * User is responsible for destroying the actual item object.
     */
    T remove(ListNode<T>* node);

    /**
     * Delete all nodes and set head / tail to NULL.
     * Does not delete items, be sure to do it yourself
     */
    void release();

    /**
     * @TODO:
     * if you need the list to return next or previous nodes when deleting node
     * while iterating, implement methods such as:
     *
     * ListNode<T>* removeAndNext(ListNode<T>* beGone)
     * ListNode<T>* removeAndPrev(ListNode<T>* beGone)
     */

    /**
     * Self explanary
     */
    bool empty() const { return (!head || !tail); }

    /**
     * Usage:
     * if(list) ? "has elements" : "is empty";
     */
    operator bool() const { return !empty(); }

private:

    ListNode<T>* head;
    ListNode<T>* tail;

};


// ===== implementation


template <typename T>
DoubleLinkedList<T>::~DoubleLinkedList()
{
    release();
}

template <typename T>
void DoubleLinkedList<T>::release()
{
    while(head)
    {
        ListNode<T>* temp = head;
        head = head->next;
        delete temp;
    }
    head = NULL;
    tail = NULL;
}

// ===== push

template <typename T>
void DoubleLinkedList<T>::pushTail(T item)
{
    tail = new ListNode<T>(item, tail, NULL);
    if( tail->prev )
        tail->prev->next = tail;
    if( !head )
        head = tail;
}

template <typename T>
void DoubleLinkedList<T>::pushHead(T item)
{
    head = new ListNode<T>(item, NULL, head);
    if( head->next )
        head->next->prev = head;
    if( !tail )
        tail = head;
}

// ===== pop

template<typename T>
T DoubleLinkedList<T>::popTail()
{
    if( empty() )
        return NULL;
    //ListNode<T>* temp(tail);
    ListNode<T>* temp = tail;
    T item( tail->item );
    tail = tail->prev ;
    if( tail )
        tail->next = NULL;
    else
        head = NULL ;
    delete temp;
    return item;
}

template<typename T>
T DoubleLinkedList<T>::popHead()
{
    if( empty() )
        return NULL;
    //ListNode<T>* temp(head);
    ListNode<T>* temp = head;
    T item( head->item );
    head = head->next ;
    if( head )
        head->prev = NULL;
    else
        tail = NULL;
    delete temp;
    return item;
}

// ===== peek

template<typename T>
T DoubleLinkedList<T>::peekTail()
{
    if(tail)
        return tail->item;
    return NULL;
}

template<typename T>
T DoubleLinkedList<T>::peekHead()
{
    if(head)
        return head->item;
    return NULL;
}

// ===== remove

template<typename T>
T DoubleLinkedList<T>::remove(ListNode<T>* node)
{
    if(!node || empty())
        return NULL;
    if(node->next)
        node->next->prev = node->prev;
    if(node->prev)
        node->prev->next = node->next;
    if(node == head)
        head = node->next;
    if(node == tail)
        tail = node->prev;
    T item = node->item;
    delete node;
    return item;
}

template<typename T>
bool DoubleLinkedList<T>::remove(T item)
{
    if( empty() )
        return false;
    ListNode<T>* temp = head;
    while(temp)
    {
        if(temp->item == item)
        {
            remove(temp);
            return true;
        }
        temp = temp->next;
    }
    return false;
}

#endif // __DOUBLELINKEDLIST_H__

