/**
 * Interface for general controller that receives calls for
 * reading inputs and doing stuff with it.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __IUICONTROLLER_H__
#define __IUICONTROLLER_H__

class IUIController
{
public:

    // TODO: are these needed? propably not?
    IUIController() { }
    virtual ~IUIController() { }

    /**
     * Just if you need one.. will be called before deletion
     */
    virtual void release() { };

    /**
     * Called when the controller is in use and should read input
     * devices and do something.
     * @param frameTime seconds elapsed from previous call
     */
    virtual void updateControls(const float frameTime) = 0;

    /**
     * Load configurations from config object
     */
    virtual void loadConfiguration() { };

};

#endif // __IUICONTROLLER_H__
