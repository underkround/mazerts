/**
 * C3DObjectControllerKeyboard.h header file
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 19.2.2009
 * 
 * basic 'fly-around' object controller
 * using keyboard.
 */

#ifndef __3DOBJECTCONTROLLERKEYBOARD_H__
#define __3DOBJECTCONTROLLERKEYBOARD_H__

#include "I3DObjectController.h"

class C3DObjectControllerKeyboard : public I3DObjectController
{
public:
	C3DObjectControllerKeyboard(void);
	virtual ~C3DObjectControllerKeyboard(void);

	/**
	 * Update
	 * simple keyboard object 'fly around' controller
	 * @param fFrametime application frame time
	 */
	virtual void Update(float fFrametime);
};

#endif	// __3DOBJECTCONTROLLERKEYBOARD_H__
