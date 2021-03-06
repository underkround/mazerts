#include "HealthBlock.h"
#include "UIAsset.h"

#define HEALTHBLOCK_ROTATIONSPEED 0.75f // comment this line out to disable rotation
#define HEALTHBLOCK_TORUS_CORNERS 6

HealthBlock::HealthBlock(UIAsset* pAsset, float yOffset, C3DResourceContainer* pContainer)
{
    m_pAsset = pAsset->getAsset();
    pAsset->setHealthBlock(this);
    SetVisible(false);
    
    Create(getHealthBlockMesh(pContainer, pAsset->getHalfSize()));
    
    m_Alive = true;
    m_Dir = 0;
    m_UnitDir = 0;
    m_YOffSet = yOffset;
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
    if(!m_Alive)
        return false; // d�d

    if (IsVisible())
    {
        // count new material color
        int ratio = (int)((m_pAsset->getHitpoints() / m_pAsset->getHitpointsMax()) * 512.0f);
        int r = 512 - ratio;
        if (r > 255) r = 255;
        int g = ratio;
        if (g > 255) g = 255;

        GetMeshDataArray()[0].pMaterial->Emissive = D3DXCOLOR(0xff000000 + (r << 16) + (g << 8));

        // rotate
#ifdef HEALTHBLOCK_ROTATIONSPEED
    
        m_Dir += fFrametime * HEALTHBLOCK_ROTATIONSPEED;
        
        D3DXMATRIX& matrix2 = GetMatrix();
        D3DXMatrixRotationY(&matrix2, m_Dir - m_UnitDir);
        D3DXVECTOR3 vec = (*(D3DXVECTOR3*)&matrix2._31);
        (*(D3DXVECTOR3*)&matrix2._31) = (*(D3DXVECTOR3*)&matrix2._21);
        (*(D3DXVECTOR3*)&matrix2._21) = vec;
        matrix2._42 = m_YOffSet;
#endif
        I3DObject::Update(fFrametime);
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
    TCHAR meshName[100];
    // create mesh with name like:  "healthblockmesh (4x4)"
    _stprintf_s(meshName, _T("%s (%dx%d)"), HEALTHBLOCK_MESHNAME, m_pAsset->getDef()->width, m_pAsset->getDef()->height);
    pMesh = pContainer->FindMesh(meshName);
    if (!pMesh)
    {
//        D3DXCreateBox(m_ResourceContainer.GetDevice(), HEALTHBLOCK_WIDTH, HEALTHBLOCK_HEIGHT, HEALTHBLOCK_DEPTH, &pMesh, NULL);
        D3DXCreateTorus(pContainer->GetDevice(), 0.1f * radius, 1.5f * radius, 3, HEALTHBLOCK_TORUS_CORNERS, &pMesh, NULL);
        // TODO: Catch FAILED?
        pContainer->AddResource(meshName, pMesh);
        pMesh = pContainer->FindMesh(meshName);
    }
    return pMesh;
}
