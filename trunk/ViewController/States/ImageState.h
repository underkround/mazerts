/**
 * shows images (for loading screen etc)
 *
 * $Revision: 395 $
 * $Date: 2009-05-27 02:16:14 +0300 (ke, 27 touko 2009) $
 * $Id: ImageState.h 395 2009-05-26 23:16:14Z murgo $
 */

#ifndef __IMAGESTATE_H__
#define __IMAGESTATE_H__

#include "IState.h"
#include "../App/ID3DApplication.h"
#include "../UIComponent/RootContainer.h"
#include "../UIComponent/BasicButton.h"

class CTheApp;

class ImageState : public IState
{
public:
    ImageState(int imagenum, int w, int h);
    virtual ~ImageState();

    /**
     * Called when the state is created
     * @param pApplication Pointer to I3DApplication
     * @return S_OK if state was initialized successfully, otherwise error code
     */
    virtual HRESULT create(CTheApp* pApplication);

    /**
     * Called before the state is going to be destroyed, releases resources
     */
    virtual void release();

    /** 
     * Updates the game controls and game logic
     * @param fFrameTime Time passed since last update in seconds
     * @return True, if the game keeps running, false if it is terminated
     */
    virtual bool update(const float frameTime);

    /**
     * Sets lights and camera
     * @param pDevice Pointer to D3d-device
     */
    virtual void prepareForRender(const LPDIRECT3DDEVICE9 pDevice, const float frameTime);

    /**
     * Renders the game
     * @param pDevice Pointer to D3d-device
     */
    virtual void render(const LPDIRECT3DDEVICE9 pDevice);

    /**
     * Releases D3DPOOL_DEFAULT-stuff on device lost
     */
    virtual void onDeviceLost()
    {
    }

    /**
     * Recreates D3DPOOL_DEFAULT-stuff after device reset
     * @return HRESULT
     */
    virtual HRESULT onRestore();

private:

    /**
     * Main application
     */
    CTheApp* m_pApp;

    // Root container covering the whole screen and managing components
    RootContainer*      m_pRootContainer;
	BasicButton*		m_pImage;
	
	int m_ImageNum;
	int m_Width;
	int m_Height;
    int                 m_TimesRendered;
};

#endif //__GAMESTATE_H__
