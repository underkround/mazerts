/**
 * ID3DApplication.h header file
 * Interface for the ID3DApplication class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 2.2.2009
 * 
 * An abstract class implementing Direct3D
 * version 9 initialisation and main loop
 *
 * REQUIRES:
 * Project wide define: DIRECT3D_VERSION=0x900
 * Libraries: dxguid.lib d3d9.lib d3dx9.lib
 */

#ifndef __ID3DAPPLICATION_H__
#define __ID3DAPPLICATION_H__

#include "IApplication.h"

// include the D3D stuff
#include <d3d9.h>
#include <d3dx9.h>


class ID3DApplication : public IApplication
{
public:
    ID3DApplication(void);
    virtual ~ID3DApplication(void);

    /**
     * Create
     * start the Direct3D application
     * @param iWidth window or resolution width
     * @param iHeight window or resolution height
     * @param iBPP bits per pixel (16 or 32)
     * @param bWindowed TRUE to start in windowed mode, FALSE otherwise
     * @param strTitle application title text
     * @param dwFlags implementation specific startup flags
     */
    HRESULT Create(    int iWidth,
                    int iHeight,
                    int iBPP,
                    BOOL bWindowed,
                    LPCTSTR strTitle,
                    DWORD dwFlags);

    /**
     * Release
     * release Direct3D resources and shutdown
     */
    virtual void Release(void);


    /**
     * DoFlip
     * virtual from base class, handles the screen update
     */
    virtual void DoFlip(void);


    /**
     * OnDeviceLost
     * virtual handler is called when 3d device gets
     * to 'lost' state
     */
    virtual void OnDeviceLost(void);

    /**
     * OnRestore
     * virtual handler is called when 3d device is ready
     * to restore 'lost' resources
     */
    virtual HRESULT OnRestore(void);


    /**
     * GetD3D
     * @return pointer to initialised IDirect3D9 interface
     */
    inline LPDIRECT3D9 GetD3D() { return m_pD3D; }

    /**
     * GetDevice
     * @return pointer to initialised IDirect3DDevice9 interface
     */
    inline LPDIRECT3DDEVICE9 GetDevice() { return m_pDevice; }



    // ----- Text Drawing System Functions -----

    /**
     * SetTextFont
     * initialise the font system with given font
     * @param pFont pointer to font specs, or NULL to use
     *                default font.
     * @return S_OK or an error code
     */
    HRESULT SetTextFont(LOGFONT* pFont);

    /**
     * BeginText
     * prepare system for text drawing
     */
    void BeginText(void);

    /**
     * EndText
     * call when finished drawing text
     */
    void EndText(void);

    /**
     * DrawText
     * draw text to screen
     * @param iX x-coordinate to draw to
     * @param iY y-coordinate to draw to
     * @param strText text to draw
     * @param dwColor 32 bit text color value ARGB
     */
    void DrawText(int iX, int iY, LPCTSTR strText, DWORD dwColor);

    /**
     * GetTextRect
     * compute minimum rectangle to contain the text
     * @param iX x-coordinate to draw to
     * @param iY y-coordinate to draw to
     * @param strText text to draw
     * @return minimum rectangle to contain the text
     */
    RECT GetTextRect(int iX, int iY, LPCTSTR strText);

    /**
     * GetTextHeight
     * @return height of the text line in pixels
     */
    inline int GetTextHeight(void) const { return m_iTextHeight; }


private:

    /**
     * SetDeviceDefaults
     * set some default settings to 3d device
     * This allows us to see at least 'something'
     * when we render the graphics
     */
    void SetDeviceDefaults(void);

    /**
     * HandleWindowSizeChange
     * function resets the 3d device with current
     * window size. All device states are lost in
     * operation
     */ 
    void HandleWindowSizeChange(void);

    /**
     * OnMessage
     * virtual windows message handler, Interrupt and handle
     * messages related to window size and position
     */
    virtual BOOL OnMessage(UINT iMessage, WPARAM wParam, LPARAM lParam);


    // Direct3D members
    LPDIRECT3D9                        m_pD3D;
    LPDIRECT3DDEVICE9                m_pDevice;
    D3DPRESENT_PARAMETERS            m_Present;

    // variables for text drawing
    LPD3DXSPRITE                    m_pSprite;
    LPD3DXFONT                        m_pFont;
    int                                m_iTextHeight;
};


#endif    // __ID3DAPPLICATION_H__

