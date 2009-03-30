#include "HealthBlock.h"
#include "UIUnit.h"

HealthBlock::HealthBlock(UIUnit* pAsset, float yOffset)
{
    m_pAsset = pAsset->getUnit();
    pAsset->setHealthBlock(this);
    SetVisible(false);
    GetMatrix()._42 += yOffset;
}

HealthBlock::~HealthBlock(void)
{

}

void HealthBlock::Create(LPD3DXMESH pMesh)
{
    // rotate torus 90 degrees (don't do this with box)
    D3DXMATRIX matrix;
    D3DXMATRIX& matrix2 = GetMatrix();
    float yoff = matrix2._42;
    D3DXMatrixIdentity(&matrix);
    D3DXMatrixRotationX(&matrix, D3DX_PI / 2);
    D3DXMatrixMultiply(&matrix2, &matrix2, &matrix);
    matrix2._41 = 0.0f;
    matrix2._42 = yoff;
    matrix2._43 = 0.0f;
    C3DObject::Create(pMesh);
    
    // create material
	MESHDATA data;
	data.pTexture = NULL;
	D3DMATERIAL9* material = new D3DMATERIAL9;
	::memset(material, 0, sizeof(D3DMATERIAL9));
	material->Diffuse = D3DXCOLOR(0xff00ff00);
    data.pMaterial = material;

	AddMeshData(data);
}

void HealthBlock::Release()
{
    // release material
    MESHDATA pData = GetMeshDataArray()[0];
    delete pData.pMaterial;
    GetMeshDataArray().clear();
    
    C3DObject::Release();
}

bool HealthBlock::Update(float fFrametime)
{
    I3DObject::Update(fFrametime);
    if (IsVisible())
    {
        // count new material color
        float ratio = m_pAsset->getHitpoints() / m_pAsset->getHitpointsMax();
        int r = (int)(512 - ratio * 512);
        if (r > 255) r = 255;
        int g = (int)(ratio * 512);
        if (g > 255) g = 255;

        GetMeshDataArray()[0].pMaterial->Diffuse = D3DXCOLOR(0xff000000 + (r << 16) + (g << 8));
    }

    return true;
}

void HealthBlock::Render(LPDIRECT3DDEVICE9 pDevice)
{
    // might be nice to set lighting & whatnot to false
    C3DObject::Render(pDevice);
}
