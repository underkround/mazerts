/**
 * C3DObjectDebug
 *
 * Debug-version of C3DObject, can be set to destroy itself after certain time
 * NOTE: When using self-destructing objects, the object will call "delete this"
 * inside the Update-method, so NEVER USE AUTOMATIC C3DObjectDebug-objects!
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __C3DOBJECTDEBUG_H__
#define __C3DOBJECTDEBUG_H__

#include <d3dx9.h>
#include "../App/C3DObject.h"
#include "../../Model/Common/DoubleLinkedList.h"

class C3DObjectDebug : public C3DObject
{
public:
    
    /**
     * Constructor for non-self-destructing object
     */
    C3DObjectDebug()
    {
        m_SelfDestructing = false;
    }

    /**
     * Constructor for self-destructing object
     * @param lifeTime How many seconds from creation the object
     *        destroys itself
     */
    C3DObjectDebug(float lifeTime)
    {
        m_SelfDestructing = true;
        m_LifeTime = lifeTime;
    }


    void Release();

    /**     
     * Updates the object
     * @param fFrametime application frame time
     * @return True if no action should take place, false if the
               child removed itself from parent
     */
     virtual bool Update(float fFrameTime);

private:

    /**
     * Whether this object self-destructs or not
     */
    bool m_SelfDestructing;

    /**
     * Time (in seconds) how long this object takes to destruct
     */
    float m_LifeTime;
};

#endif //__C3DOBJECTDEBUG_H__