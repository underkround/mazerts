#ifndef __MENUGUI_H__
#define __MENUGUI_H__

#include "IButtonListener.h"
#include "UIContainer.h"

class MenuGui
{
public:
	MenuGui(IButtonListener* pListener);
	~MenuGui();
	
private:
	UIContainer*                m_pButtonPanel;
};

#endif