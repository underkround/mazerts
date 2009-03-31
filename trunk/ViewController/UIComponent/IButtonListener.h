/**
 *
 * Interface for button press -listener that receives and handles the clicks.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __IBUTTONLISTENER_H__
#define __IBUTTONLISTENER_H__

class BasicButton;

class IButtonListener
{
public:

    /**
     * Handler for primary button click
     * @param src   pointer to button that received the click
     */
    virtual void onButtonClick(BasicButton* pSrc) = 0;

    /**
     * Handler for alternative button click
     * (all buttons might not support this, and furthermore
     * the implementing handler can leave this empty)..
     * @param src   pointer to button that received the alt click
     */
    virtual void onButtonAltClick(BasicButton* pSrc) = 0;

};

#endif // __IBUTTONLISTENER_H__
