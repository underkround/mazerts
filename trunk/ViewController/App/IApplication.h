/**
 * IApplication.h header file
 * Interface for the IApplication class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 26.1.2009
 * 
 * An abstract class implementing basic
 * features of game application, including
 * window handling etc.
 */



#ifndef __IAPPLICATION_H__
#define __IAPPLICATION_H__

// include all windows specific stuff
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Timer.h"


class IApplication
{
public:
    IApplication(void);
    virtual ~IApplication(void);

    /**
     * Create
     * start the application
     * @param iWidth window or resolution width
     * @param iHeight window or resolution height
     * @param iBPP bits per pixel (only in fullscreen mode)
     * @param bWindowed TRUE to start in windowed mode, FALSE otherwise
     * @param strTitle application title text
     * @param dwFlags implementation specific startup flags, set to 0
     */
    HRESULT Create(    int iWidth,
                    int iHeight,
                    int iBPP,
                    BOOL bWindowed,
                    LPCTSTR strTitle,
                    DWORD dwFlags);

    /**
     * Release
     * release application resources and shutdown
     */
    virtual void Release(void);


    /**
     * Close
     * close application window and start the shutdown
     */
    void Close(void);




    // ------------------------------------------
    // Pure virtual functions


    /**
     * OnCreate
     * pure virtual function for the application to initialise
     * all application specific resources, graphics, sounds, whatever.
     * Called after application initialisation is complete.
     * @return S_OK if successful, or any error code to cancel app startup
     */
    virtual HRESULT OnCreate(void) = 0;

    /**
     * OnRelease
     * pure virtual function for the application to release
     * all application specific resources, graphics, sounds, whatever.
     * Called just before the application is shut down.
     */
    virtual void OnRelease(void) = 0;

    /**
     * OnFlip
     * pure virtual application main loop function
     * this function is called by the engine just before
     * graphics are transferred to the screen
     */
    virtual void OnFlip(void) = 0;




    /**
     * Run
     * application message pump and main loop
     * @return when application quits, value
     * from WM_QUIT windows message
     */
    int Run(void);


    /**
     * GetWindow
     * @return handle to application main window
     */
    inline HWND GetWindow(void) { return m_hWnd; }

    /**
     * IsActive
     * @return TRUE if application is active and running
     * FALSE otherwise
     */
    inline BOOL IsActive(void) const { return m_bActive; }

    /**
     * IsWindowed
     * @return TRUE if application is in windowed mode,
     * FALSE otherwise
     */
    inline BOOL IsWindowed(void) const { return m_bWindowed; }

    /**
     * GetScreenRect
     * @return reference to screen rectangle, where
     * main window is related to the desktop
     */
    inline RECT& GetScreenRect(void) { return m_rcScreen; }

    /**
     * GetWindowRect
     * @return reference to window rectangle, (0,0,width,height)
     */
    inline RECT& GetWindowRect(void) { return m_rcWindow; }

    /**
     * SetActive
     * @param bSet TRUE to activate application
     */
    void SetActive(BOOL bSet);

    
    /**
     * DoFlip
     * function performs one screen update
     */
    virtual void DoFlip(void);

    /**
     * GetTimer
     * @return reference to application frame timer
     */
    inline CTimer& GetTimer() { return m_Timer; }

    /**
     * GetFrameTime
     * @return number of seconds passed since last frame
     */
    inline float GetFrameTime() const { return m_Timer.GetElapsedSeconds(); }


    /**
     * RandInt
     * @param iLow low value in range
     * @param iHigh high value in range
     * @return random integer in between iLow and iHigh
     */
    static int RandInt(int iLow, int iHigh);

    /**
     * RandFloat
     * @param fLow low value in range
     * @param fHigh high value in range
     * @return random float value in between fLow and fHigh
     */
    static float RandFloat(float fLow, float fHigh);

protected:

    /**
     * CreateAppWindow
     * @param iWidth window or resolution width
     * @param iHeight window or resolution height
     * @param bWindowed TRUE to start in windowed mode, FALSE otherwise
     * @param strTitle application title text
     */
    BOOL CreateAppWindow(    int iWidth,
                            int iHeight,
                            BOOL bWindowed,
                            LPCTSTR strTitle);

    /**
     * OnMessage
     * virtual handler for the windows messages
     * @param iMessage message type
     * @param wParam message wparam
     * @param lParam message lparam
     */
    virtual BOOL OnMessage(    UINT iMessage,
                            WPARAM wParam,
                            LPARAM lParam);


    /**
     * UpdateRectangles
     * calculate current window and screen rectangles
     */
    void UpdateRectangles(void);

    /**
     * OnKeyDown
     * virtual message handler for keydown messages
     * @param dwKey the virtual key code of the pressed key
     */
    virtual void OnKeyDown(DWORD dwKey);

    /**
     * OnChar
     * virtual message handler for processed keyboard
     * messages
     * @param c the pressed character
     */
    virtual void OnChar(TCHAR c);



private:
    /**
     * WndProc
     * static window procedure function
     */
    static long WINAPI WndProc(    HWND hWnd,
                                UINT iMessage,
                                WPARAM wParam,
                                LPARAM lParam);


    // application main window
    HWND                    m_hWnd;

    // as application active
    BOOL                    m_bActive;

    // running in windowed or fullscreen mode
    BOOL                    m_bWindowed;

    // screen rectangle, where window is related
    // to desktop
    RECT                    m_rcScreen;

    // window rectangle, just a size of our window
    // 0,0,width,height
    RECT                    m_rcWindow;

    // application frame time counter
    CTimer                    m_Timer;
};


#endif    // __IAPPLICATION_H__


