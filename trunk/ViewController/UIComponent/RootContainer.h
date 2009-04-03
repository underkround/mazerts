/**
 *
 * RootComponent is intented to be the top-level container on the
 * screen that covers the whole screen, and contains all the
 * other containers and components.
 *
 * The root container is transparent, and it should reduce the
 * needless operations that normal components and containers
 * have, since it's never intented to be visible or to render
 * itself.
 *
 * There should be only one RootContainer.
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __ROOTCONTAINER_H__
#define __ROOTCONTAINER_H__

#include "UIComponent.h"
#include "UIContainer.h"
#include "../App/IApplication.h"


#define CHARBUFFER_SIZE 30

class RootContainer : public UIContainer
{
public:

    /**
     * Access to singleton instance
     */
    static RootContainer* getInstance()
    {
        static RootContainer instance;
        return &instance;
    }

    /**
     * Create by setting the application from which to get the screensize
     * @param pDevice   normal device-pointer
     * @param pApp      pointer to application from which to get the screensize
     */
    HRESULT create(LPDIRECT3DDEVICE9 pDevice, IApplication* pApp);

    /**
     * Normal create is disabled for RootContainer -> we need the IApplication
     * pointer so this create fails
     */
    virtual HRESULT create(LPDIRECT3DDEVICE9 pDevice)
    {
        return S_FALSE;
    }

    /**
     * Load / reload configurations (assumes the config files to be loaded)
     */
    void loadConfigurations();

    /**
     * Add char that was typed
     */
    void inputChar(TCHAR c)
    {
        if(m_CharBufferCount < CHARBUFFER_SIZE)
        {
            m_CharBuffer[m_CharBufferCount] = c;
            ++m_CharBufferCount;
        }
    }

// =====
// ===== Begin derived methods
// =====

    virtual HRESULT onDeviceLost();

    virtual HRESULT onRestore(LPDIRECT3DDEVICE9 pDevice);

    virtual void onRender(LPDIRECT3DDEVICE9 pDevice)
    {
        // root container does not render
    }

    virtual void render(LPDIRECT3DDEVICE9 pDevice);

    virtual const bool setSize(const int width, const int height)
    {
        return false; // we get our size from window
    }

    virtual void release()
    {
    }

    /**
     * STC
     */
    virtual int updateControls(const float frameTime);

    virtual UIComponent* getFocus();

protected:

    virtual void onChange(LPDIRECT3DDEVICE9 pDevice)
    {
        // root container does not have change actions
    }

    virtual const bool setPosition(const int x, const int y)
    {
        return false; // we have always position (0, 0)
    }

// =====
// ===== Begin own methods
// =====

protected:

    /**
     * Protected ctor & dtor
     */
    RootContainer();
    virtual ~RootContainer()
    { }

    /**
     * Prevent copying
     */
    RootContainer(const RootContainer& rc) { }
    RootContainer& operator =(const RootContainer& orc) { }

// ===== Members

    // application class from which to get the screen size
    IApplication*   m_pApp;

    bool            m_Created;

    // focused component, or null
    UIComponent*    m_pFocused;

    // treshold for mouse idle event
    float           m_MouseIdleTreshold;

    // charbuffer
    int             m_CharBufferCount;
    TCHAR           m_CharBuffer[CHARBUFFER_SIZE];

};

#endif // __ROOTCONTAINER_H__
