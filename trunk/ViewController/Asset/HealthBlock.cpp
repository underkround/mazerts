#include "HealthBlock.h"
#include "UIAsset.h"

#define HEALTHBLOCK_ROTATIONSPEED 0.5f // comment this line out to disable rotation
#define HEALTHBLOCK_TORUS_CORNERS 6

HealthBlock::HealthBlock(UIAsset* pAsset, float yOffset, C3DResourceContainer* pContainer)
{
    m_pAsset = pAsset->getAsset();
    pAsset->setHealthBlock(this);
    SetVisible(false);
    GetMatrix()._42 += yOffset;
    Create(getHealthBlockMesh(pContainer, pAsset->getHalfSize()));
    m_Alive = true;
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
	material->Emissive = D3DXCOLOR(0xff00ff00);
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
    if(!m_Alive)
        return false; // död

    if (IsVisible())
    {
        // count new material color
        float ratio = m_pAsset->getHitpoints() / m_pAsset->getHitpointsMax();
        int r = (int)(512 - ratio * 512);
        if (r > 255) r = 255;
        int g = (int)(ratio * 512);
        if (g > 255) g = 255;

        GetMeshDataArray()[0].pMaterial->Emissive = D3DXCOLOR(0xff000000 + (r << 16) + (g << 8));

        //D3DXMATRIX &wm = m_pParent->GetParent()->GetWorldMatrix();
        D3DXMATRIX &wm = GetWorldMatrix();
        D3DXMATRIX &om = GetMatrix();
        D3DXMATRIX m;
        //D3DXMatrixRotationY(&m, D3DX_PI*0.5f);
        D3DXMatrixRotationY(&m, wm._22);
        D3DXMatrixMultiply(&om, &om, &m);

/*
        // rotate
#ifdef HEALTHBLOCK_ROTATIONSPEED
        D3DXMATRIX matrix;
        D3DXMATRIX& matrix2 = GetMatrix();
        D3DXMatrixRotationY(&matrix, fFrametime * HEALTHBLOCK_ROTATIONSPEED);
        D3DXMatrixMultiply(&matrix2, &matrix2, &matrix);
#endif
*/
    }

    return true;
}

void HealthBlock::Render(LPDIRECT3DDEVICE9 pDevice)
{
    C3DObject::Render(pDevice);
}

LPD3DXMESH HealthBlock::getHealthBlockMesh(C3DResourceContainer* pContainer, const float radius)
{
    LPD3DXMESH pMesh;
    pMesh = pContainer->FindMesh(HEALTHBLOCK_MESHNAME);
    if (!pMesh)
    {
//        D3DXCreateBox(m_ResourceContainer.GetDevice(), HEALTHBLOCK_WIDTH, HEALTHBLOCK_HEIGHT, HEALTHBLOCK_DEPTH, &pMesh, NULL);
        D3DXCreateTorus(pContainer->GetDevice(), 0.1f * radius, 1.1f * radius, 3, HEALTHBLOCK_TORUS_CORNERS, &pMesh, NULL);
        // TODO: Catch FAILED?
        pContainer->AddResource(HEALTHBLOCK_MESHNAME, pMesh);
        pMesh = pContainer->FindMesh(HEALTHBLOCK_MESHNAME);
    }
    return pMesh;
}
