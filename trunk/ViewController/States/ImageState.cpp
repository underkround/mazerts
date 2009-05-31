#include "ImageState.h"

#include "../App/TheApp.h"
// Components
#include "../UIComponent/RootContainer.h"
#include "../UIComponent/UIContainer.h"
#include "../UIComponent/UIComponent.h"

ImageState::ImageState(int imagenum, int w, int h)
{
    m_pApp = NULL;
	m_pImage = NULL;
	m_Created = false;
    m_TimesRendered = 0;
	
	m_ImageNum = imagenum;
	m_Width = w;
	m_Height = h;
}

ImageState::~ImageState()
{
    int i = 0;
}

HRESULT ImageState::create(CTheApp* pApplication)
{
//    HRESULT hres;
    m_pApp = pApplication;
    m_pApp->setDontClear(true);

    // UIComponents
    m_pRootContainer = RootContainer::getInstance();
    m_pRootContainer->create(pApplication->GetDevice(), m_pApp);
	
	m_pImage = new BasicButton(m_Width, m_Height);
    m_pImage->setBackgroundTexture(RootContainer::getInstance()->getIconTexture(m_ImageNum));
    m_pImage->setBackgroundTextureClicked(RootContainer::getInstance()->getIconTexture(m_ImageNum));
    m_pImage->setPosition((RootContainer::getInstance()->getWidth() - m_Width) >> 1, (RootContainer::getInstance()->getHeight() - m_Height) >> 1);
    RootContainer::getInstance()->addComponent(m_pImage);

    m_Created = true;
    return S_OK;
}


void ImageState::release()
{
	if (m_pImage) {
		m_pImage->release();
		delete m_pImage;
		m_pImage = NULL;
	}
    if (m_pApp)
    {
        m_pApp->setDontClear(false);
    }
}


bool ImageState::update(const float frameTime)
{
    return m_TimesRendered < 2;
}


void ImageState::prepareForRender(const LPDIRECT3DDEVICE9 pDevice, const float frameTime)
{
}


void ImageState::render(const LPDIRECT3DDEVICE9 pDevice)
{
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pRootContainer->render(pDevice);
    m_TimesRendered++;
}

HRESULT ImageState::onRestore()
{
    HRESULT hres;
    hres = m_pRootContainer->onRestore(m_pApp->GetDevice());
    if(FAILED(hres))
    {
        return hres;
    }

    return S_OK;
}
