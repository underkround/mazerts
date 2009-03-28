#include "HealthBar.h"

HealthBar::HealthBar(void)
{
}

HealthBar::~HealthBar(void)
{
}

HRESULT HealthBar::create(LPDIRECT3DDEVICE9 pDevice)
{
    HRESULT hres = pDevice->CreateVertexBuffer(4 * sizeof(TRANSLITVERTEX),
                                               D3DUSAGE_WRITEONLY,
                                               TRANSLITVERTEX::GetFVF(),
                                               D3DPOOL_MANAGED,
                                               &m_pVertBuf,
                                               NULL);

    if(FAILED(hres))
    {
        return hres;
    }

    return S_OK;
}

void HealthBar::release()
{
}

bool HealthBar::Update(float fFrametime)
{
    I3DObject::Update(fFrametime);
    //TRANSLITVERTEX* pVertices = NULL;
    return false;
}

void HealthBar::Render(LPDIRECT3DDEVICE9 pDevice)
{
    //pDevice->SetTexture(0, pTexture);

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    
    pDevice->SetFVF(TRANSLITVERTEX::GetFVF());        
    pDevice->SetStreamSource(0, m_pVertBuf, 0, sizeof(TRANSLITVERTEX));
    pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

}
