#include "Input.h"
#include "KeyboardState.h"

bool* KeyboardState::keyDown = NULL;
bool* KeyboardState::keyOld = NULL;
bool* KeyboardState::keyReleased = NULL;

void KeyboardState::update()
{
    if(SUCCEEDED(Input::keyboard.Update()))
    {
        for (int i = 0; i < 256; i++)
        { 
            if (Input::keyboard.GetButton(i))
            {
                //Button down
                keyDown[i] = true;
                keyReleased[i] = false;
            }
            else
            {
                keyDown[i] = false;
                
                //Detect button release
                if(keyOld[i] == true)
                {
                    keyReleased[i] = true;
                }
                else
                {
                    keyReleased[i] = false;
                }
            }

            keyOld[i] = keyDown[i];
        }
    }
}