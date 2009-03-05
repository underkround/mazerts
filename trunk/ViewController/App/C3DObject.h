/**
 * C3DObject.h header file
 * Interface for the C3DObject class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 19.2.2009
 * 
 * concrete 3d object that is capable of rendering
 * direct3d mesh objects
 */

#ifndef __C3DOBJECT_H__
#define __C3DOBJECT_H__

#include "I3Dobject.h"

#include <vector>
using namespace std;


class C3DObject : public I3DObject
{
public:
	// struct for storing the object materials and
	// textures
	struct MESHDATA
	{
		D3DMATERIAL9*		pMaterial;
		LPDIRECT3DTEXTURE9	pTexture;
	};

	C3DObject(void);
	virtual ~C3DObject(void);

	/**
	 * Create
	 * initialise the 3d object
	 * @param pMesh the mesh to use with object
	 */
	void Create(LPD3DXMESH pMesh);

	/**
	 * Release
	 * release the 3d object
	 */
	virtual void Release(void);

	/**
	 * Render
	 * render the mesh object
	 * @param pDevice device to render with
	 */
	virtual void Render(LPDIRECT3DDEVICE9 pDevice);

	/**
	 * AddMeshData
	 * add new meshdata structure to object
	 * @param meshdata reference to new meshdata
	 */
	void AddMeshData(MESHDATA& meshdata);



	/**
	 * GetMesh
	 * @return 3d object mesh
	 */
	inline LPD3DXMESH GetMesh(void) { return m_pMesh; }

	/**
	 * GetMeshDataArray
	 * @return reference to array of mesh data structures
	 */
	inline vector<MESHDATA>& GetMeshDataArray(void) { return m_arrMeshData; }

protected:
	// an array of mesh data structures
	vector<MESHDATA>	m_arrMeshData;

	// object mesh
	LPD3DXMESH			m_pMesh;
};

#endif	// __C3DOBJECT_H__

