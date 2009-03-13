#ifndef __TESTDLINKLIST_H__
#define __TESTDLINKLIST_H__

#include <assert.h>
#include <iostream>
#include "../Common/DoubleLinkedList.h"

using namespace std;

class Foo
{
public:
    Foo(int val) : id(counter) {
        value = val;
        counter++;
        cout << "Foo #" << id << " in constructor with " << value << "\n";
    }
    Foo(const Foo& f) : id(counter) {
        cout << "Foo #" << id << " in copyconstructor, from Foo #" << f.id << "\n";
    }
    ~Foo() {
        cout << "Foo #" << id << " in destructor with " << value << "\n";
    }
    /**
     * Decrease value as long as it's positive, returning true.
     * Return false when the value reaches zero.
     */
    bool Update(float dummy) {
        cout << "Foo #" << id << " == " << value << "\n";
        if(value <= 0) return false;
        --value; return true;
    }
    int id;
    int value;
    static int counter;
};

int Foo::counter = 0;

// =====

void testDoubleLinkedList()
{
    DoubleLinkedList<Foo*> m_arrChildren;
    m_arrChildren.pushHead(new Foo(5));
    m_arrChildren.pushHead(new Foo(1));
    m_arrChildren.pushHead(new Foo(1));
    m_arrChildren.pushHead(new Foo(2));

    float fFrametime = 1;
    ListNode<Foo*>* node;
    while(m_arrChildren)
    {
        cout << " --update--\n";
        node = m_arrChildren.headNode();
        while(node)
        {
            if(node->item->Update(fFrametime) == false)
            {
                ListNode<Foo*>* temp = node->next;
                Foo* pChild = m_arrChildren.remove(node);
                //pChild->Release();
                delete pChild;
                node = temp;
            }
            else
                node = node->next;
        }
    }

    /**
     * The alternate way, possibly not so recommended
     */
    cout << "\n** test #2 **\n";

    m_arrChildren.pushHead(new Foo(1));
    m_arrChildren.pushHead(new Foo(3));
    m_arrChildren.pushHead(new Foo(5));
    m_arrChildren.pushHead(new Foo(1));

    while(m_arrChildren)
    {
        cout << " --update--\n";
        node = m_arrChildren.headNode();
        while(node)
        {
            if (node->item->Update(fFrametime))
                node = node->next;
            else {
                //node->item->Release();
                delete node->item;
                node = m_arrChildren.removeGetNext(node);
            }
        }
    }

    if(m_arrChildren.headNode() == NULL)    cout << " List: head is null\n";
    else                                    cout << " List: head is NOT null\n";

    if(m_arrChildren.tailNode() == NULL)    cout << " List: tail is null\n";
    else                                    cout << " List: tail is NOT null\n";

    m_arrChildren.release();
}

#endif __TESTDLINKLIST_H__
