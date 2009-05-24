/**
 * ID3DApplication.cpp source file
 * Implementation for the ID3DApplication class
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

#include "ID3DApplication.h"
#include "../../Model/Common/Config.h"

ID3DApplication::ID3DApplication(void)
{
    // init members
    m_pD3D = NULL;
    m_pDevice = NULL;
    ::memset(&m_Present, 0, sizeof(D3DPRESENT_PARAMETERS));

    m_pSprite = NULL;
    m_pFont = NULL;
    m_iTextHeight = 0;
}


ID3DApplication::~ID3DApplication(void)
{
}


HRESULT ID3DApplication::Create(    int iWidth,
                                    int iHeight,
                                    int iBPP,
                                    BOOL bWindowed,
                                    LPCTSTR strTitle,
                                    DWORD dwFlags)
{
    if (!CreateAppWindow(    iWidth,
                            iHeight,
                            bWindowed,
                            strTitle))
    {
        return E_FAIL;
    }

    // create the Direct3D
    m_pD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
    if (!m_pD3D)
    {
        return E_FAIL;
    }

    bool aa = Config::getInstance()->getValueAsBool("antialias", false);
    D3DMULTISAMPLE_TYPE samples = D3DMULTISAMPLE_NONE;
    if (aa) {
        if( FAILED(m_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
					        D3DDEVTYPE_HAL , D3DFMT_R8G8B8, FALSE, 
                            samples, NULL ) ) )
                            return E_FAIL;

        switch (Config::getInstance()->getValueAsInt("antialias samples", 2))
        {
        case 2: samples = D3DMULTISAMPLE_2_SAMPLES; break;
        case 3: samples = D3DMULTISAMPLE_3_SAMPLES; break;
        case 4: samples = D3DMULTISAMPLE_4_SAMPLES; break;
        case 5: samples = D3DMULTISAMPLE_5_SAMPLES; break;
        case 6: samples = D3DMULTISAMPLE_6_SAMPLES; break;
        case 7: samples = D3DMULTISAMPLE_7_SAMPLES; break;
        case 8: samples = D3DMULTISAMPLE_8_SAMPLES; break;
        case 9: samples = D3DMULTISAMPLE_9_SAMPLES; break;
        case 10: samples = D3DMULTISAMPLE_10_SAMPLES; break;
        case 11: samples = D3DMULTISAMPLE_11_SAMPLES; break;
        case 12: samples = D3DMULTISAMPLE_12_SAMPLES; break;
        case 13: samples = D3DMULTISAMPLE_13_SAMPLES; break;
        case 14: samples = D3DMULTISAMPLE_14_SAMPLES; break;
        case 15: samples = D3DMULTISAMPLE_15_SAMPLES; break;
        case 16: samples = D3DMULTISAMPLE_16_SAMPLES; break;
        default:
            return E_FAIL;
        }
    }

    // initialise the present parameters
    HRESULT hres;
    m_Present.hDeviceWindow = GetWindow();

    // initialise automatic z-buffering
    m_Present.EnableAutoDepthStencil = TRUE;

    if (IsWindowed())
    {
        // init windowed mode present parameters
        m_Present.BackBufferWidth = GetWindowRect().right;
        m_Present.BackBufferHeight = GetWindowRect().bottom;
        m_Present.Windowed = TRUE;
        m_Present.BackBufferCount = 1;

        // antialias
        if (aa)
        {
            m_Present.SwapEffect      = D3DSWAPEFFECT_DISCARD;
            m_Present.MultiSampleType = samples;
        }
        else
        {
            m_Present.SwapEffect = D3DSWAPEFFECT_COPY;
        }

        // force z-buffer to 16 bit format, without stencil bits
        m_Present.AutoDepthStencilFormat = D3DFMT_D16;

        // get the desktop display mode
        D3DDISPLAYMODE dm;
        m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);
        m_Present.BackBufferFormat = dm.Format;
    }
    else
    {
        // init fullscreen mode present parameters
        m_Present.BackBufferWidth = iWidth;
        m_Present.BackBufferHeight = iHeight;
//        m_Present.SwapEffect = D3DSWAPEFFECT_FLIP;
        m_Present.FullScreen_RefreshRateInHz = 0;
        m_Present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

        // antialias
        if (aa)
        {
            m_Present.SwapEffect      = D3DSWAPEFFECT_DISCARD;
            m_Present.MultiSampleType = samples;
        }
        else
        {
            m_Present.SwapEffect = D3DSWAPEFFECT_FLIP;
        }

        switch (iBPP)
        {
        case 8:
            // forgetaboutit... not supported anymore
            return E_FAIL;
            break;

        case 16:
            m_Present.BackBufferFormat = D3DFMT_R5G6B5;
            m_Present.AutoDepthStencilFormat = D3DFMT_D16;
            break;

        case 24:
            // forgetaboutit... not supported anymore
            return E_FAIL;
            break;

        case 32:
            m_Present.BackBufferFormat = D3DFMT_X8R8G8B8;
            m_Present.AutoDepthStencilFormat = D3DFMT_D16;
            break;

        default:
            // unknown bits per pixel value
            return E_FAIL;
        }
    }


    // Create Direct3D device
    // use the fallback mechanism to select best possible
    // 3d device type
    hres = m_pD3D->CreateDevice(    D3DADAPTER_DEFAULT,
                                    D3DDEVTYPE_HAL,
                                    GetWindow(),
                                    D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                    &m_Present,
                                    &m_pDevice);
    if (FAILED(hres))
    {
        hres = m_pD3D->CreateDevice(    D3DADAPTER_DEFAULT,
                                        D3DDEVTYPE_HAL,
                                        GetWindow(),
                                        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                        &m_Present,
                                        &m_pDevice);
    }
    if (FAILED(hres))
    {
        hres = m_pD3D->CreateDevice(    D3DADAPTER_DEFAULT,
                                        D3DDEVTYPE_REF,
                                        GetWindow(),
                                        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                        &m_Present,
                                        &m_pDevice);
    }
    if (FAILED(hres))
    {
        // failed to create any meaningful 3d device...
        Release();
        return hres;
    }



    SetDeviceDefaults();


    // init the text drawing system
    hres = D3DXCreateSprite(m_pDevice, &m_pSprite);
    if (FAILED(hres))
    {
        Release();
        return hres;
    }

    // init the font
    hres = SetTextFont(NULL);
    if (FAILED(hres))
    {
        Release();
        return hres;
    }



    // init the timer
    GetTimer().Create();

    // call the pure virtual OnCreate
    hres = OnCreate();
    if (FAILED(hres))
    {
        Release();
        return hres;
    }

    SetActive(TRUE);
    return S_OK;
}


void ID3DApplication::Release(void)
{
    if (m_pDevice)
    {
        // call pure virtual OnRelease
        OnRelease();

        if (m_pFont)
        {
            m_pFont->Release();
            m_pFont = NULL;
        }
        if (m_pSprite)
        {
            m_pSprite->Release();
            m_pSprite = NULL;
        }

        m_pDevice->Release();
        m_pDevice =  NULL;

        if (m_pD3D)
        {
            m_pD3D->Release();
            m_pD3D = NULL;
        }
    }
}


void ID3DApplication::DoFlip(void)
{
    if (!IsActive())
    {
        return;
    }

    IApplication::DoFlip();

    HRESULT hres;
    hres = m_pDevice->Present(NULL, NULL, NULL, NULL);
    if (FAILED(hres))
    {
        hres = m_pDevice->TestCooperativeLevel();
        if (FAILED(hres))
        {
            // device is in lost state, and all non-managed
            // video memory resources are lost and need to
            // be re-loaded
            OnDeviceLost();

            m_pDevice->Reset(&m_Present);
            SetDeviceDefaults();

            if (FAILED(OnRestore()))
            {
                // game level application failed to
                // restore the video memory resources
                // nothing we can do about it...
                Close();
            }
        }
    }
}


void ID3DApplication::SetDeviceDefaults(void)
{
    if (!m_pDevice)
    {
        return;
    }

    // set basic transformations

    // world transformation
    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);
    m_pDevice->SetTransform(D3DTS_WORLD, &world);

    // set the view transformation (camera)
    D3DXMATRIX view;
    D3DXMatrixLookAtLH(    &view,
                        &D3DXVECTOR3(0.0f, 0.0f, -75.0f),
                        &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
                        &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
    m_pDevice->SetTransform(D3DTS_VIEW, &view);


    // set the projection transformation
    D3DXMATRIX projection;

    D3DXMatrixPerspectiveFovLH(    &projection,
                                    D3DX_PI * 0.3f,        
                                    (float)m_Present.BackBufferWidth / (float)m_Present.BackBufferHeight,
                                    1.0f,
                                    1000.0f);

    m_pDevice->SetTransform(D3DTS_PROJECTION, &projection);



    // set the default light
    D3DLIGHT9        light;
    ::memset(&light, 0, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_POINT;//D3DLIGHT_DIRECTIONAL;
    light.Position = D3DXVECTOR3(0.0, 0.0, -30.0);
    light.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
    light.Range = 1000.0f;
    light.Attenuation0 = 0.01f;
    light.Attenuation1 = 0.001f;
    light.Attenuation2 = 0.0001f;

    m_pDevice->SetLight(0, &light);
    m_pDevice->LightEnable(0, TRUE);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pDevice->SetRenderState(D3DRS_AMBIENT, 0x00FFFFFF);    


    // set the default material
    D3DMATERIAL9    material;
    ::memset(&material, 0, sizeof(D3DMATERIAL9));
    material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    material.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
    m_pDevice->SetMaterial(&material);


    
    // set default alphablending states
    m_pDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);



    // set default texturing states
    int i;
    for (i=0; i<8; i++)
    {
        m_pDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        m_pDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        m_pDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        m_pDevice->SetSamplerState(i, D3DSAMP_MAXMIPLEVEL, 0);
    }

    // disable shaders
    m_pDevice->SetPixelShader(NULL);
    m_pDevice->SetVertexShader(NULL);
}


void ID3DApplication::HandleWindowSizeChange(void)
{
    if (IsWindowed() && IsActive() && m_pDevice)
    {
        // compare new and old size and reset the device
        // if necesasry
        if (m_Present.BackBufferWidth != (DWORD) GetWindowRect().right ||
            m_Present.BackBufferHeight != (DWORD) GetWindowRect().bottom)
        {
            // window size has changed, get the new size
            m_Present.BackBufferWidth = GetWindowRect().right;
            m_Present.BackBufferHeight = GetWindowRect().bottom;

            OnDeviceLost();

            // reset the 3d device
            m_pDevice->Reset(&m_Present);
            SetDeviceDefaults();

            if (FAILED(OnRestore()))
            {
                Close();
            }
        }
    }
}


BOOL ID3DApplication::OnMessage(UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    BOOL ret = IApplication::OnMessage(iMessage, wParam, lParam);

    switch (iMessage)
    {
    case WM_SIZE:
        if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
        {
            HandleWindowSizeChange();
        }
        break;

    case WM_EXITSIZEMOVE:
        HandleWindowSizeChange();
        break;
    }

    return ret;
}


void ID3DApplication::OnDeviceLost(void)
{
    if (m_pFont)
    {
        m_pFont->OnLostDevice();
    }
    if (m_pSprite)
    {
        m_pSprite->OnLostDevice();
    }
}


HRESULT ID3DApplication::OnRestore(void)
{
    if (m_pFont)
    {
        m_pFont->OnResetDevice();
    }
    if (m_pSprite)
    {
        m_pSprite->OnResetDevice();
    }
    return S_OK;
}





// ----- Text Drawing System Functions -----

HRESULT ID3DApplication::SetTextFont(LOGFONT* pFont)
{
    LOGFONT lf;
    if (pFont)
    {
        // copy the parameter to our local fontspec
        ::memcpy(&lf, pFont, sizeof(LOGFONT));
    }
    else
    {
        // parameter is NULL, init fontspec with
        // default font
        ::memset(&lf, 0, sizeof(LOGFONT));
        lf.lfHeight = -12;
        lf.lfWeight = 400;
        lf.lfOutPrecision = 1;
        lf.lfClipPrecision = 2;
        lf.lfQuality = 1;
        lf.lfPitchAndFamily = 49;
        ::_tcscpy_s(lf.lfFaceName, _T("Lucida Console"));
    }

    // release the previous font
    if (m_pFont)
    {
        m_pFont->Release();
        m_pFont = NULL;
    }

    // create new font
    HRESULT hres;
    hres = D3DXCreateFont(    m_pDevice,
                            lf.lfHeight,
                            lf.lfWidth,
                            lf.lfWeight,
                            0,
                            lf.lfItalic,
                            lf.lfCharSet,
                            lf.lfOutPrecision,
                            lf.lfQuality,
                            lf.lfPitchAndFamily,
                            lf.lfFaceName,
                            &m_pFont );
    if (FAILED(hres))
    {
        return hres;
    }

    RECT rcArea = GetTextRect(0, 0, _T("W"));
    m_iTextHeight = rcArea.bottom;
    return S_OK;
}


void ID3DApplication::BeginText(void)
{
    if (m_pSprite)
    {
        m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
    }
}


void ID3DApplication::EndText(void)
{
    if (m_pSprite)
    {
        m_pSprite->End();
    }
}


void ID3DApplication::DrawText(int iX, int iY, LPCTSTR strText, DWORD dwColor)
{
    if (m_pFont == NULL || m_pSprite == NULL)
    {
        // invalid user error...
        return;
    }

    RECT rcArea = GetTextRect(iX, iY, strText);
    m_pFont->DrawText(    m_pSprite,
                        strText,
                        -1,
                        &rcArea,
                        0,
                        dwColor);
}


RECT ID3DApplication::GetTextRect(int iX, int iY, LPCTSTR strText)
{
    RECT rcArea;
    rcArea.left = iX;
    rcArea.top = iY;
    rcArea.right = 0;
    rcArea.bottom = 0;

    m_pFont->DrawText(    m_pSprite,
                        strText,
                        -1,
                        &rcArea,
                        DT_CALCRECT,
                        0);
    return rcArea;
}


