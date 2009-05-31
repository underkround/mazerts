#ifndef __MENUGUI_H__
#define __MENUGUI_H__

#include "IButtonListener.h"
#include "UIContainer.h"

class MenuGui
{
public:
	MenuGui(IButtonListener* pListener);
	~MenuGui();

    void showCredits(const bool show);
    void create();
	
private:
	UIContainer*                m_pButtonPanel;
    IButtonListener*            m_pListener;
};

#endif