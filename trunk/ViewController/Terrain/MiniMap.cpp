#include "MiniMap.h"

MiniMap::MiniMap()
{
	m_pTexture = NULL;
}

MiniMap::~MiniMap()
{
}

void MiniMap::create()
{
	m_pVertices = new TRANSLITVERTEX[4];

	m_pVertices[0].x = 10.0f;
	m_pVertices[0].y = 10.0f;
	m_pVertices[0].z = 0.0;
	m_pVertices[0].tu = 0.0f;
	m_pVertices[0].tv = 1.0f;
	m_pVertices[0].dwColor = 0xFFFFFFFF;
	m_pVertices[0].rhw = 0.0f;

	m_pVertices[1].x = 310.0f;
	m_pVertices[1].y = 10.0f;
	m_pVertices[1].z = 0.0;
	m_pVertices[1].tu = 1.0f;
	m_pVertices[1].tv = 1.0f;
	m_pVertices[1].dwColor = 0xFFFFFFFF;
	m_pVertices[1].rhw = 0.0f;

	m_pVertices[2].x = 310.0f;
	m_pVertices[2].y = 310.0f;
	m_pVertices[2].z = 0.0;
	m_pVertices[2].tu = 1.0f;
	m_pVertices[2].tv = 0.0f;
	m_pVertices[2].dwColor = 0xFFFFFFFF;
	m_pVertices[2].rhw = 0.0f;

	m_pVertices[3].x = 10.0f;
	m_pVertices[3].y = 310.0f;
	m_pVertices[3].z = 0.0;
	m_pVertices[3].tu = 0.0f;
	m_pVertices[3].tv = 0.0f;
	m_pVertices[3].dwColor = 0xFFFFFFFF;
	m_pVertices[3].rhw = 0.0f;

}

void MiniMap::release()
{
	if(m_pVertices)
	{
		delete [] m_pVertices;
		m_pVertices = NULL;
	}

}


void MiniMap::render(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->SetFVF(TRANSLITVERTEX::GetFVF());	
	pDevice->SetTexture(0, m_pTexture);

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_pVertices, sizeof(TRANSLITVERTEX));

}