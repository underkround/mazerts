/**
 * State-interface for application state machine
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __ISTATE_H__
#define __ISTATE_H__

#include "../App/ID3DApplication.h"
#include "d3d9.h"

class IState
{
public:
    IState() 
    {
        m_Created = false;
    }

    virtual ~IState() {}

    /**
     * Called when the state is created
     * @param pApplication Pointer to I3DApplication, get D3D-device and other needed stuff through this
     * @return S_OK if state was initialized successfully, otherwise error code
     */
    virtual HRESULT create(ID3DApplication* pApplication) = 0;

    /**
     * Called before the state is going to be destroyed, release resources
     */
    virtual void release() = 0;

    /** 
     * Updates the state
     * @param frameTime Time passed since last update in seconds
     * @return True, if the state is kept running, false if the state should be switched
     *         to next in list (like transitioning from intro to menu)
     */
    virtual bool update(float frameTime) = 0;

    /**
     * Called before BeginScene, set view-matrix, lights etc., whatever is needed
     * @param pDevice Pointer to D3d-device
     */
    virtual void prepareForRender(LPDIRECT3DDEVICE9 pDevice) = 0;

    /**
     * Renders the state, BeginScene will already be called before entering and
     * EndScene will be called after this method exits
     * @param pDevice Pointer to D3D-device
     */
    virtual void render(LPDIRECT3DDEVICE9 pDevice) = 0;

    /**
     * Tells if the state has been initialized or not
     */
    inline virtual bool isCreated() { return m_Created; }

    // ARE THESE NEEDED?
    /**
     * OnDeviceLost
     * virtual handler is called when 3d device gets
     * to 'lost' state
     */
    //virtual void OnDeviceLost(void) = 0;

    /**
     * OnRestore
     * virtual handler is called when 3d device is ready
     * to restore 'lost' resources
     */
    //virtual HRESULT OnRestore(void) = 0;

protected:

    /**
     * Tells the application whether the state has already been initialized or not
     */
    bool m_Created;

};

#endif