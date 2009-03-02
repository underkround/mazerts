/**
 * I3DObject.cpp source file
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 19.2.2009
 * 
 * abstract 3d object interface
 */


#include "I3DObject.h"


I3DObject::I3DObject(void)
{
	// init the members
	D3DXMatrixIdentity(&m_mWorld);
	D3DXMatrixIdentity(&m_mLocal);

	m_pParent = NULL;

	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vAcceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_bActive = TRUE;
	m_bVisible = TRUE;
}


I3DObject::~I3DObject(void)
{
}


void I3DObject::Release(void)
{
	DWORD i;
	for (i=0; i<m_arrChildren.size(); i++)
	{
		I3DObject* pChild = m_arrChildren[i];
		pChild->Release();
		delete pChild;
	}

	m_arrChildren.clear();
}


void I3DObject::AddChild(I3DObject* pChild)
{
	// add child to array
	m_arrChildren.push_back(pChild);

	// link to the parent
	pChild->m_pParent = this;
}


void I3DObject::Update(float fFrametime)
{
	if (IsActive())
	{
		// apply velocity to position
		m_mLocal._41 += m_vVelocity.x * fFrametime;
		m_mLocal._42 += m_vVelocity.y * fFrametime;
		m_mLocal._43 += m_vVelocity.z * fFrametime;

		// apply the acceleration to velocity
		m_vVelocity.x += m_vAcceleration.x * fFrametime;
		m_vVelocity.y += m_vAcceleration.y * fFrametime;
		m_vVelocity.z += m_vAcceleration.z * fFrametime;

		if (m_pParent)
		{
			// we have the parent object:
			// create combined world matrix from
			// parent 'world' and our 'local'
			D3DXMatrixMultiply(	&m_mWorld,
								&m_mLocal,
								&m_pParent->m_mWorld);

		}
		else
		{
			// we don't have the parent object
			// In sense, 'World' is our parent.
			// world matrix is the same as our local
			// matrix
			::memcpy(&m_mWorld, &m_mLocal, sizeof(D3DXMATRIX));
		}
	}

	// update the children
	DWORD i;
	for (i=0; i<m_arrChildren.size(); i++)
	{
		m_arrChildren[i]->Update(fFrametime);
	}
}

