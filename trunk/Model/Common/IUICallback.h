/**
 * Interface for UI-callbacks
 *
 * Used to send data to UI members using function pointers, without tying Model to UI
 *
 * $Revision:$
 * $Date:$
 * $Id:$
 */

#ifndef __IUICALLBACK_H__
#define __IUICALLBACK_H__

class IUICallback
{
public:    
    virtual ~IUICallback() {}

    //Function signatures, override the ones you need (and add here if you need new signatures)
    inline virtual void callBack(void) {}
    inline virtual void callBack(void* pVoid) {}

};

#endif //__IUICALLBACK_H__