#include "HealthBar.h"

HealthBar::HealthBar(void)
{
    m_Position.x = 0.0f;
    m_Position.y = 0.0f;
    m_pVertBuf = NULL;
}

HealthBar::~HealthBar(void)
{
}

HRESULT HealthBar::Create(LPDIRECT3DDEVICE9 pDevice)
{
    HRESULT hres = pDevice->CreateVertexBuffer(4 * sizeof(LITVERTEX),
                                               D3DUSAGE_WRITEONLY,
                                               LITVERTEX::GetFVF(),
                                               D3DPOOL_MANAGED,
                                               &m_pVertBuf,
                                               NULL);

    if(FAILED(hres))
    {
        return hres;
    }


    LITVERTEX* pVertex = NULL;

	D3DXVECTOR3 normal(0.0f, 0.0f, -1.0f);

	for (int i=0; i<4; ++i)
	{
		//pVertex[i].nx = normal.x;
		//pVertex[i].ny = normal.y;
		//pVertex[i].nz = normal.z;

		pVertex[i].z = 0.0f;
	}

    m_pVertBuf->Lock(0, 4 * sizeof(LITVERTEX), (void**)&pVertex, D3DLOCK_DISCARD);
    {
        float posX = m_Position.x;
        float posY = m_Position.y;

        pVertex[0].x = posX-10.0f;
        pVertex[0].y = posY+10.0f;
        pVertex[0].dwColor = 0xAA00FF00;
        pVertex[0].tu = 0.0f;
        pVertex[0].tv = 0.0f;

        pVertex[1].x = posX+10.0f;
        pVertex[1].y = posY+10.0f;
        pVertex[1].dwColor = 0xAA00FF00;
        pVertex[1].tu = 0.0f;
        pVertex[1].tv = 1.0f;

        pVertex[2].x = posX+10.0f;
        pVertex[2].y = posY-10.0f;
        pVertex[2].dwColor = 0xAAFF0000;
        pVertex[2].tu = 1.0f;
        pVertex[2].tv = 0.0f;

        pVertex[3].x = posX-10.0f;
        pVertex[3].y = posY-10.0f;
        pVertex[3].dwColor = 0xAAFF0000;
        pVertex[3].tu = 1.0f;
        pVertex[3].tv = 1.0f;
    }
    m_pVertBuf->Unlock();


    return S_OK;
}

void HealthBar::Release()
{
    if(m_pVertBuf)
    {
        m_pVertBuf->Release();
        m_pVertBuf = NULL;
    }
}

bool HealthBar::Update(float fFrametime)
{
    //I3DObject::Update(fFrametime);


    return TRUE;
}

void HealthBar::Render(LPDIRECT3DDEVICE9 pDevice)
{
    pDevice->SetTexture(0, NULL);

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		/*D3DMATERIAL9 material;
		::memset(&material, 0, sizeof(D3DMATERIAL9));
		material.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pDevice->SetMaterial(&material);*/
    
    pDevice->SetFVF(LITVERTEX::GetFVF());        
    pDevice->SetStreamSource(0, m_pVertBuf, 0, sizeof(LITVERTEX));
    pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

}
