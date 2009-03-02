/**
 * vertices.h header file
 * declaration of the different vertex types
 *
 * 
 *
 */


#ifndef __VERTICES_H__
#define __VERTICES_H__

// include Direct3D for the FVF flags
#include <d3dx9.h>


// define basic untransformed, unlit vertex type
struct VERTEX
{
    static DWORD GetFVF()    { return    D3DFVF_XYZ |
                                        D3DFVF_NORMAL |
                                        D3DFVF_TEX1;										
                            }

    float            x, y, z;    // vertex coordinate
    float            nx, ny, nz;    // vertex normal
    float            tu, tv;        // texture coordinates	
};

//Untransformed, unlit vertex with two separate uv-sets (used mainly for terrain, colormap + "roughing texture")
struct VERTEX2UV
{
    static DWORD GetFVF()    { return    D3DFVF_XYZ |
                                        D3DFVF_NORMAL |
                                        D3DFVF_TEX2;										
                            }

    float            x, y, z;    // vertex coordinate
    float            nx, ny, nz;    // vertex normal
    float            tu, tv;        // First set of texture coordinates
	float			 tu2, tv2;		// Second set of texture coordinates
};


// untransformed and lit vertex type
struct LITVERTEX
{
    static DWORD GetFVF() { return D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1; }

    float            x, y, z;
    DWORD            dwColor;
    float            tu, tv;        // one set of texture coordinates
};


// transformed and lit vertex type
struct TRANSLITVERTEX
{
    static DWORD GetFVF() { return D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1; }

    float            x, y, z;    // position in screen coordinates
    float            rhw;        // vertex position on view z-axis
    DWORD            dwColor;    // vertex diffuse color
    float            tu, tv;        // one set of texture coordinates
};


#endif    // __VERTICES_H__

