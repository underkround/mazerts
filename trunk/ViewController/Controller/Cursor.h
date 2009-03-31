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

#include "../App/ID3DApplication.h" // for tooltip text drawing
#include "../Input/MouseState.h" // for tooltip position

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
        CURS_AMOUNT,      //When adding new cursortypes, place the enumerations before CURS_AMOUNT
        CURS_END = 0xFF
    };

    const static int CURSORSIZE = 16;
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
    HRESULT update(const float frameTime);

    /**
     * Renders the cursor
     * @param pDevice Device to render with
     */
    void render(LPDIRECT3DDEVICE9 pDevice);

    /** 
     * Release cursor-resources (vertexbuffer, texture)
     */
    void release();

    /**
     * Set tooltip for cursor.
     * Tooltip is short text that can be rendered with the cursor.
     * If there is previously set tooltip, this overrides it.
     * You can also set lifetime for the tooltip, give 0 or negative
     * value to keep the tooltip until clear is called, or another
     * tooltip is set.
     */
    void setTooltip(LPCTSTR tooltipStr, float lifeTime)
    {
        clearTooltip();
        m_pTooltip = tooltipStr;
        m_TooltipDowncounter = (lifeTime <= 0) ? -1 : lifeTime;
    }

    /**
     * Clear tooltip, if any
     */
    void clearTooltip()
    {
        m_TooltipDowncounter = -1;
        if(m_pTooltip)
        {
//            delete m_pTooltip;
            m_pTooltip = NULL;
        }
    }

    /**
     * Render the tooltip, called from between begintext and endtext
     */
    void renderText(ID3DApplication* app)
    {
        if(m_pTooltip)
        {
            app->DrawTextW( MouseState::mouseX + m_TooltipOffsetX,
                            MouseState::mouseY + m_TooltipOffsetY,
                            m_pTooltip,
                            m_TooltipColor);
        }
    }

private:


    Cursor() 
    {
        m_pVB = NULL;
        m_pTexture = NULL;
        m_Type = CURS_NORMAL;
        m_pTooltip = NULL;
        m_TooltipDowncounter = -1;
        m_TooltipOffsetX = CURSORSIZE + 10;
        m_TooltipOffsetY = CURSORSIZE;
        m_TooltipColor = 0xFFCCFF00;
    }

    virtual ~Cursor() 
    {
        release();
    }

    // Tooltip text
    LPCTSTR     m_pTooltip;
    float       m_TooltipDowncounter;
    int         m_TooltipOffsetX;
    int         m_TooltipOffsetY;
    int         m_TooltipColor;

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
