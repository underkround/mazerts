/**
 * Singleton Cursor-object, used to show the player where
 * the cursor is and information about pointed object
 * $Revision:$
 * $Date:$
 * $Id:$
 */

#ifndef __CURSOR_H__
#define __CURSOR_H__

#include "d3dx9.h"
#include "../App/vertices.h"

class Cursor
{
public:
    
    /**
     * Different cursor types, the textures have to be
     * drawn in this order into the file
     */
    enum CursorType
    {
      CURS_NORMAL = 0,
      CURS_SELECT,
      CURS_ATTACK,
      CURS_INVALID,
      CURS_AMOUNT,
      CURS_END = 0xFF
    };

    const static int CURSORSIZE = 32;
    const static float TEXOFFSET;

    inline static Cursor* getInstance()
    {
        static Cursor instance;
        return &instance;
    }

    inline void setType(CursorType type)
    {
        m_Type = type;
    }

    /**
     * Creates the cursor object
     * @param pDevice Device to render with
     * @return S_OK or error code
     */
    HRESULT create(LPDIRECT3DDEVICE9 pDevice);
    
    /**
     * Updates the cursor position and texture
     * @return S_OK or error code
     */
    HRESULT update();

    /**
     * Renders the cursor
     * @param pDevice Device to render with
     */
    void render(LPDIRECT3DDEVICE9 pDevice);

    /** 
     * Release cursor-resources (vertexbuffer, texture)
     */
    void release();

private:


    Cursor() 
    {
        m_pVB = NULL;
        m_pTexture = NULL;
        m_Type = CURS_NORMAL;
    }

    virtual ~Cursor() {}


    /**
     * Current cursortype
     */
    CursorType m_Type;

    /**
     * Vertexbuffer for cursor
     */
    LPDIRECT3DVERTEXBUFFER9 m_pVB;

    /**
     * Cursor-textures
     */
    LPDIRECT3DTEXTURE9 m_pTexture;
};

#endif //__CURSOR_H__