/**
 * I3DObject.h header file
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 19.2.2009
 * 
 * abstract 3d object interface
 */

#ifndef __I3DOBJECT_H__
#define __I3DOBJECT_H__


// matrices etc from direct3dx
#include <d3dx9.h>

// stl vector as a dynamic array
#include <vector>
using namespace std;


class I3DObject
{
public:
	I3DObject(void);
	virtual ~I3DObject(void);

	/**
	 * Release
	 * release object and its children
	 */
	virtual void Release(void);


	/**
	 * Update
	 * update 3d object hierarchy
	 * function creates combined matrix from parent&child
	 * @param fFrametime application frame time
	 */
	virtual void Update(float fFrametime);


	/**
	 * Render
	 * pure virtual drawing function
	 * @param pDevice 3d device to render with
	 */
	virtual void Render(LPDIRECT3DDEVICE9 pDevice) = 0;


	/**
	 * AddChild
	 * add child to child array
	 * NOTE! ownership of the child object
	 * is passed to its parent, and child is
	 * deleted in parent 'Release' function.
	 * @param pChild pointer to new child object
	 */
	void AddChild(I3DObject* pChild);


	/**
	 * GetParent
	 * @return pointer to parent object or NULL
	 */
	inline I3DObject* GetParent(void) { return m_pParent; }

	/**
	 * GetChildren
	 * @return reference to child array
	 */
	inline vector<I3DObject*>& GetChildren(void) { return m_arrChildren; }

	/**
	 * GetMatrix
	 * @return reference to local matrix
	 */
	inline D3DXMATRIX& GetMatrix(void) { return m_mLocal; }

	/**
	 * SetMatrix
	 * set the object local matrix
	 * @param m matrix to set to object
	 */
	inline void SetMatrix(const D3DXMATRIX& m) { m_mLocal = m; }

	/**
	 * GetWorldMatrix
	 * @return reference to object 'world' matrix
	 */
	inline D3DXMATRIX& GetWorldMatrix(void) { return m_mWorld; }

	/**
	 * GetVelocity
	 * @return reference to velocity vector
	 */
	inline D3DXVECTOR3& GetVelocity(void) { return m_vVelocity; }

	/**
	 * GetAcceleration
	 * @return reference to acceleration vector
	 */
	inline D3DXVECTOR3& GetAcceleration(void) { return m_vAcceleration; }



	/**
	 * active/visible get/set
	 */
	inline BOOL IsActive(void) const { return m_bActive; }
	inline BOOL IsVisible(void) const { return m_bVisible; }
	inline void SetActive(BOOL bSet) { m_bActive = bSet; }
	inline void SetVisible(BOOL bSet) { m_bVisible = bSet; }

protected:
	// pointer to parent object
	I3DObject*			m_pParent;

	// child array
	vector<I3DObject*>	m_arrChildren;

	// 3d object matrices
	// m_mWorld: where object is related to the world
	// m_mLocal: where object is related to its parent
	D3DXMATRIX			m_mWorld;
	D3DXMATRIX			m_mLocal;

	// object movement speed
	D3DXVECTOR3			m_vVelocity;

	// object acceleration
	D3DXVECTOR3			m_vAcceleration;


	BOOL				m_bActive;
	BOOL				m_bVisible;
};


#endif	// __I3DOBJECT_H__

