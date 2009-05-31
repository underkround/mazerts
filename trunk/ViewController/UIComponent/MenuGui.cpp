#include "MenuGui.h"
#include "RootContainer.h"
#include "GridLayout.h"
#include "BasicButton.h"

MenuGui::MenuGui(IButtonListener* pListener)
{
    m_pButtonPanel = NULL;
    m_pListener = pListener;
    create();
}

MenuGui::~MenuGui()
{
    if (m_pButtonPanel)
    {
        m_pButtonPanel->release();
        delete m_pButtonPanel;
        m_pButtonPanel = NULL;
    }
}

void MenuGui::create()
{
    if (!m_pButtonPanel)
    {
        RootContainer* rc = RootContainer::getInstance();

        m_pButtonPanel = new UIContainer(0, 0, rc->getWidth(), rc->getHeight());
        m_pButtonPanel->setTransparent(true);

        MarginSet* panelSnaps = m_pButtonPanel->getSnapMargins();
        panelSnaps->left = 0;
        panelSnaps->bottom = 0;
        panelSnaps->right = 0;
        panelSnaps->top = 0;
        rc->addComponent(m_pButtonPanel);

        const int w = 512;
        const int h = 128;
        const int x = (rc->getWidth() - w) / 2;
        const int y2 = (rc->getHeight() - h) / 2;
        const int y1 = (int)(y2 - h * 1.5);
        const int y3 = (int)(y2 + h * 1.5);

        BasicButton* bNewGame = new BasicButton(w, h, 0, m_pListener);
        bNewGame->setPosition(x, y1);
        bNewGame->setBackgroundTexture(RootContainer::getInstance()->getIconTexture(1021));
        bNewGame->setBackgroundTextureClicked(RootContainer::getInstance()->getIconTexture(1022));
        bNewGame->setAlphaBlending(true);
        m_pButtonPanel->addComponent(bNewGame);

        BasicButton* bCredits = new BasicButton(w, h, 1, m_pListener);
        bCredits->setPosition(x, y2);
        bCredits->setBackgroundTexture(RootContainer::getInstance()->getIconTexture(1023));
        bCredits->setBackgroundTextureClicked(RootContainer::getInstance()->getIconTexture(1024));
        bCredits->setAlphaBlending(true);
        m_pButtonPanel->addComponent(bCredits);

        BasicButton* bQuit = new BasicButton(w, h, 2, m_pListener);
        bQuit->setPosition(x, y3);
        bQuit->setBackgroundTexture(RootContainer::getInstance()->getIconTexture(1025));
        bQuit->setBackgroundTextureClicked(RootContainer::getInstance()->getIconTexture(1026));
        bQuit->setAlphaBlending(true);
        m_pButtonPanel->addComponent(bQuit);
    }
}

void MenuGui::showCredits(const bool show)
{
    if (m_pButtonPanel)
    {
        m_pButtonPanel->release();
        delete m_pButtonPanel;
        m_pButtonPanel = NULL;
    }
    if (show)
    {
        RootContainer* rc = RootContainer::getInstance();
        m_pButtonPanel = new UIContainer(0, 0, rc->getWidth(), rc->getHeight());
        m_pButtonPanel->setTransparent(true);

        MarginSet* panelSnaps = m_pButtonPanel->getSnapMargins();
        panelSnaps->left = 0;
        panelSnaps->bottom = 0;
        panelSnaps->right = 0;
        panelSnaps->top = 0;
        rc->addComponent(m_pButtonPanel);

        const int w = 512;
        const int h = 512;
        const int x = (rc->getWidth() - w) / 2;
        const int y = (rc->getHeight() - h) / 2;

        BasicButton* bCredits = new BasicButton(w, h, 4, m_pListener);
        bCredits->setPosition(x, y);
        bCredits->setBackgroundTexture(RootContainer::getInstance()->getIconTexture(1020));
        bCredits->setBackgroundTextureClicked(RootContainer::getInstance()->getIconTexture(1020));
        bCredits->setAlphaBlending(true);
        m_pButtonPanel->addComponent(bCredits);
    }
    else
        create();
}
