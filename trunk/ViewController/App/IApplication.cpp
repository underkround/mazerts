/**
 * IApplication.cpp source file
 * Implementation for the IApplication class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 26.1.2009
 * 
 * An abstract class implementing basic
 * features of game application, including
 * window handling etc.
 */

#include "IApplication.h"

IApplication::IApplication(void)
{
    m_hWnd = NULL;
    m_bActive = FALSE;
    m_bWindowed = TRUE;

    ::memset(&m_rcScreen, 0, sizeof(RECT));
    ::memset(&m_rcWindow, 0, sizeof(RECT));

    // seed the random number generator
    ::srand( ::GetTickCount() );
}


IApplication::~IApplication(void)
{
}


HRESULT IApplication::Create(    int iWidth,
                                int iHeight,
                                int iBPP,
                                BOOL bWindowed,
                                LPCTSTR strTitle,
                                DWORD dwFlags)
{
    // create application window
    if (!CreateAppWindow(    iWidth,
                            iHeight,
                            bWindowed,
                            strTitle))
    {
        // failed to create window...
        // something is badly wrong
        return E_FAIL;
    }

    // init the timer
    m_Timer.Create();

    // call pure virtual OnCreate
    HRESULT hres;
    hres = OnCreate();
    if (FAILED(hres))
    {
        Release();
        return hres;
    }

    SetActive(TRUE);
    return S_OK;
}


void IApplication::Release(void)
{
    if (m_hWnd)
    {
        // call pure virtual OnRelease
        OnRelease();

        ::DestroyWindow(m_hWnd);
        m_hWnd = NULL;
    }

    m_Timer.Release();
}


BOOL IApplication::CreateAppWindow(    int iWidth,
                                    int iHeight,
                                    BOOL bWindowed,
                                    LPCTSTR strTitle)
{
    HINSTANCE                hInstance = ::GetModuleHandle(NULL);
    DWORD                    dwStyle = 0;
    HMENU                    hMenu = NULL;
    int                        iWindowWidth = 0;
    int                        iWindowHeight = 0;

    m_bWindowed = bWindowed;

    // create window
    if (IsWindowed())
    {
        // initialise windowed mode window
        dwStyle = WS_OVERLAPPEDWINDOW;

        // compute actual size of the window
        // iWidth and iHeight parameters are requested
        // drawable area of the window, we need to calculate
        // total size before creating the window
        iWindowWidth = iWidth +
            ::GetSystemMetrics(SM_CXSIZEFRAME) * 2;
        iWindowHeight = iHeight +
            ::GetSystemMetrics(SM_CYSIZEFRAME) * 2 +
            ::GetSystemMetrics(SM_CYMENU);
    }
    else
    {
        // initialise fullscreen window
        // WS_POPUP is a borderless "tool" window
        dwStyle = WS_POPUP;
        iWindowWidth = iWidth;
        iWindowHeight = iHeight;
    }


    // register window class
    const wchar_t strClassName[] = L"GameEngineWC";
    WNDCLASSEX        wc;

    ::memset(&wc, 0, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = ::LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor = ::LoadCursor(hInstance, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = strTitle;
    wc.lpszClassName = strClassName;
    wc.cbWndExtra = sizeof(IApplication*);

    ::RegisterClassEx(&wc);


    // create the window
    m_hWnd = ::CreateWindowEx(    0,
                                strClassName,
                                strTitle,
                                dwStyle,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                iWindowWidth,
                                iWindowHeight,
                                NULL,
                                hMenu,
                                hInstance,
                                NULL);
    if (!m_hWnd)
    {
        return FALSE;
    }

    // set our 'this' pointer to window extra data
    ::SetWindowLongPtr(m_hWnd, GWL_USERDATA, (LONG_PTR)this);


    // update & show the window
    ::UpdateWindow(m_hWnd);
    ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
    UpdateRectangles();
    return TRUE;
}


void IApplication::SetActive(BOOL bSet)
{
    m_bActive = bSet;
    m_Timer.BeginTimer();
}


long WINAPI IApplication::WndProc(    HWND hWnd,
                                    UINT iMessage,
                                    WPARAM wParam,
                                    LPARAM lParam)
{
    // get the application 'this' pointer from window extra data
    IApplication* pApplication =
        (IApplication*)::GetWindowLongPtr(hWnd, GWL_USERDATA);

    if (iMessage == WM_CREATE)
    {
        // when WM_CREATE is received, window creation is
        // not complete, and therefore the extra data
        // pointer is not set. Just activate the window and
        // exit
        ::SetForegroundWindow(hWnd);
        return 0;
    }


    if (!pApplication)
    {
        return (long)::DefWindowProc(hWnd, iMessage, wParam, lParam);
    }
    else
    {
        // call the virtual message handler
        if (pApplication->OnMessage(iMessage, wParam, lParam) == FALSE)
        {
            return (long)::DefWindowProc(hWnd, iMessage, wParam, lParam);
        }
    }

    return 0;
}


int IApplication::Run(void)
{
    MSG            msg;
    BOOL        bGotMessage = FALSE;

    ::memset(&msg, 0, sizeof(MSG));

    // loop until the WM_QUIT message
    while (msg.message != WM_QUIT)
    {
        if (IsActive())
        {
            bGotMessage = ::PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE);
        }
        else
        {
            bGotMessage = ::GetMessage(&msg, m_hWnd, 0, 0);
        }

        if (bGotMessage)
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            // no windows messages pending, perform the
            // screen update
            DoFlip();
        }
    }

    return (int)msg.wParam;
}


BOOL IApplication::OnMessage(    UINT iMessage,
                                WPARAM wParam,
                                LPARAM lParam)
{
    switch (iMessage)
    {
    case WM_CLOSE:
        SetActive(FALSE);
        ::DestroyWindow(m_hWnd);
        m_hWnd = NULL;
        break;

    case WM_DESTROY:
        Release();
        ::PostQuitMessage(0);
        break;
    
    case WM_ACTIVATEAPP:
        if (wParam == 0)
        {
            // application is de-activating
            if (!IsWindowed())
            {
                SetActive(FALSE);
            }
        }
        else
        {
            // application is activating
            SetActive(TRUE);
        }
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            SetActive(FALSE);
        }
        else
        {
            if (IsWindowed())
            {
                UpdateRectangles();
            }
            SetActive(TRUE);
        }
        break;

    case WM_MOVE:
        UpdateRectangles();
        break;

    case WM_GETMINMAXINFO:
        if (IsActive() && IsWindowed())
        {
            // fix the minimum size of the window
            MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
            pInfo->ptMinTrackSize.x = 100;
            pInfo->ptMinTrackSize.y = 50;
        }
        break;

    case WM_SETCURSOR:
        if (!IsWindowed())
        {
            ::SetCursor(NULL);
            return TRUE;
        }
        break;

    case WM_KEYDOWN:
        OnKeyDown( (DWORD)wParam );
        break;

    case WM_CHAR:
        OnChar( (TCHAR)wParam );
        break;

    case WM_EXITSIZEMOVE:
        m_Timer.BeginTimer();
        break;
    }

    return FALSE;
}


void IApplication::UpdateRectangles(void)
{
    // get the window client area
    ::GetClientRect(m_hWnd, &m_rcWindow);

    // copy window rectangle to screen rectangle
    ::memcpy(&m_rcScreen, &m_rcWindow, sizeof(RECT));

    // convert rectangle to screen coordinates
    ::ClientToScreen(m_hWnd, (POINT*)&m_rcScreen.left);
    ::ClientToScreen(m_hWnd, (POINT*)&m_rcScreen.right);
}


void IApplication::DoFlip(void)
{
    if (!IsActive())
    {
        return;
    }

    // compute the frame time
    m_Timer.EndTimer();
    m_Timer.BeginTimer();

    // call pure virtual OnFlip function
    OnFlip();
}


void IApplication::OnKeyDown(DWORD dwKey)
{
    // no implementation required
}


void IApplication::OnChar(TCHAR c)
{
    // no implementation required
}


void IApplication::Close(void)
{
    if (m_hWnd)
    {
        ::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
    }
}


int IApplication::RandInt(int iLow, int iHigh)
{
    return (::rand() % ((iHigh + 1) - iLow) + iLow);
}


float IApplication::RandFloat(float fLow, float fHigh)
{
    return ((fHigh - fLow) * (float)(::rand() / (float)RAND_MAX)) + fLow;
}


