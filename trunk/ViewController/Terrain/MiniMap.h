/**
 * Minimap showing the terrain with visible unit and building locations
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include <d3dx9.h>
#include "../App/Vertices.h"

class MiniMap
{
public:
	MiniMap();
	virtual ~MiniMap();

	/**
	 * Creates the minimap vertex-array
	 */
	void create();

	/**
	 * Release minimap resources
	 */
	void release();

	/**
	 * Set the base texture used by minimap
	 * @param pTexture Pointer to texture used as base colormap (usually terrain colormap)
	 */
	inline void setTexture(LPDIRECT3DTEXTURE9 pTexture)
	{
		m_pTexture = pTexture;
	}

	/**
	 * Render the minimap
	 */
	void render(LPDIRECT3DDEVICE9 pDevice);

private:

	/**
	 * Minimap uses terrain pixel-texture direcly, so only pointer to texture is needed
	 */
	LPDIRECT3DTEXTURE9 m_pTexture;

	/**
	 * Vertices of the quad
	 */
	TRANSLITVERTEX* m_pVertices;
};


#endif //__MINIMAP_H__