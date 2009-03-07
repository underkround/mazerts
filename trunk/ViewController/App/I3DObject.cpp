/**
 * I3DObject.cpp source file
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
    ListNode<I3DObject*>* node = m_arrChildren.headNode();    
    
    while(node)
    {
        I3DObject* pChild = node->item;
        pChild->Release();
        delete pChild;

        node = node->next;
    }
    
    m_arrChildren.release();
}


void I3DObject::AddChild(I3DObject* pChild)
{
	// add child to linked list
    m_arrChildren.pushTail(pChild);

	// link to the parent
	pChild->m_pParent = this;
}

void I3DObject::RemoveChild(I3DObject* pChild)
{
    m_arrChildren.remove(pChild);
    pChild->m_pParent = NULL;
}

bool I3DObject::Update(float fFrametime)
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
    ListNode<I3DObject*>* node = m_arrChildren.headNode();    
    
    while(node)
    {
        if (node->item->Update(fFrametime))
            node = node->next;
        else {
		    node->item->Release();
            delete node->item;
            node = m_arrChildren.removeGetNext(node);
        }
    }

    return true;
}

