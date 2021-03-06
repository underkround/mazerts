/**
 * Concrete application class
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __THEAPP_H__
#define __THEAPP_H__

#include "ID3DApplication.h"
#include "../States/IState.h"
#include "../../Model/Common/DoubleLinkedList.h"
#include "GameConsole.h"

class CTheApp : public ID3DApplication
{
public:
    CTheApp(void);
    virtual ~CTheApp(void);

    /**
     * OnCreate
     * pure virtual function for the application to initialise
     * all application specific resources, graphics, sounds, whatever.
     * Called after application initialisation is complete.
     * @return S_OK if successful, or any error code to cancel app startup
     */
    virtual HRESULT OnCreate(void);

    /**
     * OnRelease
     * pure virtual function for the application to release
     * all application specific resources, graphics, sounds, whatever.
     * Called just before the application is shut down.
     */
    virtual void OnRelease(void);

    /**
     * OnFlip
     * pure virtual application main loop function
     * this function is called by the engine just before
     * graphics are transferred to the screen
     */
    virtual void OnFlip(void);

    /**
     * Handles releasing non-managed resources when device is lost
     */
    virtual void OnDeviceLost()
    {
        ID3DApplication::OnDeviceLost();
        if(m_pCurrentState)
        {
            m_pCurrentState->onDeviceLost();
        }
    }

    /**
     * Handles recreating non-managed resources after device reset
     */
    virtual HRESULT OnRestore()
    {
        HRESULT hres = ID3DApplication::OnRestore();
        if(FAILED(hres))
        {
            return hres;
        }
        
        if(m_pCurrentState)
        {
            hres = m_pCurrentState->onRestore();
        }

        return hres;
    }

    /**
     * Destroys current state and switches to next state in the states-list
     * @return S_OK or error code
     */
    virtual HRESULT nextState();

    /**
     * Puts given state to execution and pushes current
     * to list as next one (used for entering menu while in-game, as
     * the menu terminates, game is returned from states-list)
     * @return S_OK or error code
     */
    virtual HRESULT pushState(IState* pState);

    void gameover();
    void setDontClear(bool b) { m_DontClear = b; }

private:

    /**
     * DrawTextRow
     * writes text to screen and advances cursor one row
     * @param text to be written on screen
     * @param color text color
     */
    void DrawTextRow(LPCTSTR text, DWORD color);

    /**
     * OnKeyDown
     * virtual keypress message handler
     */
    virtual void OnKeyDown(DWORD dwKey);

    /**
     * OnChar
     * character input handler
     */
    virtual void OnChar(TCHAR c);

    /**
     * CheckTexturingCaps
     * check what kind of texturing capabilities
     * the display accelerator has
     */
    void CheckTexturingCaps(void);

    /**
     * handleConfig
     * loads config.ini hand handles settings if needed
     */
    void handleConfig(void);

    IState* m_pCurrentState;

    DoubleLinkedList<IState*>* m_pStates;
    
    //Text row for DrawTextRow
    int m_TextRow;

    //for game console mode
    bool m_CatchInput;

    //Help texts
    bool m_Help;

    bool m_DontClear;

};

#endif //__THEAPP_H__