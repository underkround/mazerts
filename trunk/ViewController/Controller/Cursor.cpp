#include "Cursor.h"
#include "../Input/MouseState.h"

const float Cursor::TEXOFFSET = 1.0 / CURS_AMOUNT;

HRESULT Cursor::create(LPDIRECT3DDEVICE9 pDevice)
{
    HRESULT hres = pDevice->CreateVertexBuffer(4 * sizeof(TRANSLITVERTEX), 0,
                                        TRANSLITVERTEX::GetFVF(),
                                        D3DPOOL_MANAGED,
                                        &m_pVB,
                                        NULL);

    if(FAILED(hres))
    {
        return hres;
    }

    //TODO: Get texture from resource container
    hres = D3DXCreateTextureFromFile(pDevice, _T("../data/textures/cursors3.png"), &m_pTexture);
    if(FAILED(hres))
    {
        return hres;
    }

    return S_OK;
}

HRESULT Cursor::update()
{
    TRANSLITVERTEX* pVertices = NULL;

    HRESULT hres = m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
    if(FAILED(hres))
    {
        return hres;
    }
   
    pVertices[0].x = (float)MouseState::mouseX;
    pVertices[0].y = (float)MouseState::mouseY;
    pVertices[0].z = 0.0f;
    pVertices[0].rhw = 0.999f;
    pVertices[0].tu = TEXOFFSET * m_Type;
    pVertices[0].tv = 0.0f;
    pVertices[0].dwColor = 0xFFFFFFFF;

    pVertices[1].x = (float)MouseState::mouseX + CURSORSIZE - 0.0001f;  //- 0.0001f to prevent texture bleeding from other cursor
    pVertices[1].y = (float)MouseState::mouseY;
    pVertices[1].z = 0.0f;
    pVertices[1].rhw = 0.999f;
    pVertices[1].tu = TEXOFFSET * (m_Type + 1);
    pVertices[1].tv = 0.0f;
    pVertices[1].dwColor = 0xFFFFFFFF;

    pVertices[2].x = (float)MouseState::mouseX + CURSORSIZE - 0.0001f;
    pVertices[2].y = (float)MouseState::mouseY + CURSORSIZE;
    pVertices[2].z = 0.0f;
    pVertices[2].rhw = 0.999f;
    pVertices[2].tu = TEXOFFSET * (m_Type + 1);
    pVertices[2].tv = 1.0f;
    pVertices[2].dwColor = 0xFFFFFFFF;

    pVertices[3].x = (float)MouseState::mouseX + 0.0001f;       //Again to prevent texture bleeding
    pVertices[3].y = (float)MouseState::mouseY + CURSORSIZE;
    pVertices[3].z = 0.0f;
    pVertices[3].rhw = 0.999f;
    pVertices[3].tu = TEXOFFSET * m_Type;
    pVertices[3].tv = 1.0f;
    pVertices[3].dwColor = 0xFFFFFFFF;

    hres = m_pVB->Unlock();
    if(FAILED(hres))
    {
        return hres;
    }

    return S_OK;

}

void Cursor::render(LPDIRECT3DDEVICE9 pDevice)
{
    pDevice->SetTexture(0, m_pTexture);

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    pDevice->SetFVF(TRANSLITVERTEX::GetFVF());
    pDevice->SetStreamSource(0, m_pVB, 0, sizeof(TRANSLITVERTEX));
    
    pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);    
    pDevice->SetTexture(0, NULL);
}

void Cursor::release()
{
    if(m_pTexture)
    {
        m_pTexture->Release();
        m_pTexture = NULL;
    }

    if(m_pVB)
    {
        m_pVB->Release();
        m_pVB = NULL;
    }

}