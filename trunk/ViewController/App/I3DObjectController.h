/**
 * I3DObjectController.h header file
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 19.2.2009
 * 
 * abstract interface for the object
 * controllers
 */

#ifndef __3DOBJECTCONTROLLER_H__
#define __3DOBJECTCONTROLLER_H__

#include "I3DObject.h"


class I3DObjectController
{
public:
	I3DObjectController(void);
	virtual ~I3DObjectController(void);

	/**
	 * Create
	 * initialise the object controller
	 * @param pObject pointer to object to control
	 */
	inline virtual void Create(I3DObject* pObject) { m_pObject = pObject; }

	/**
	 * Release
	 * release object controller
	 */
	inline virtual void Release(void) { m_pObject = NULL; }

	/**
	 * Update
	 * pure virtual controls update
	 * @param fFrametime application frame time
	 */
	virtual void Update(float fFrametime) = 0;

protected:
	I3DObject*			m_pObject;
};

#endif	// __3DOBJECTCONTROLLER_H__
