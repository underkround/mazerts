/**
 * Static keyboard helpers, keeps track of keystates
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __KEYBOARDSTATE_H__ 
#define __KEYBOARDSTATE_H__

class KeyboardState
{
public:

    /**
     * Creates arrays to keep track of pressed and released buttons
     */
    inline static void create()    
    {
        keyDown = new bool[256];
        keyOld = new bool[256];
        keyReleased = new bool[256];
        ::memset(keyDown, 0, 256);
        ::memset(keyOld, 0, 256);
        ::memset(keyReleased, 0, 256);
    }

    /**
     * Updates the keystates
     */
    static void update(const float frameTime);

    inline static void release()
    {
        if(keyDown)
        {
            delete [] keyDown;
            keyDown = NULL;
        }
        if(keyOld)
        {
            delete [] keyOld;
            keyOld = NULL;
        }
        if(keyReleased)
        {
            delete [] keyReleased;
            keyReleased = NULL;
        }
    }

    /**
     * Keyboard state data, left public on purpose
     */
    static bool* keyDown;       //If true, key pressed down
    static bool* keyReleased;   //If true, key was released between last update and the
                                //one before that

private:
    /**
     * Used to keep track of key releasing
     */
    static bool* keyOld;
};

#endif //__KEYBOARDSTATE_H__