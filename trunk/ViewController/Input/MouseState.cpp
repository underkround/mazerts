#include "MouseState.h"
#include "Input.h"
#include <vector>

//Mouse sensitivity default
float MouseState::mouseSens = 1.0f;
//Static stuff (needs to be here, otherwise throws a bunch of "unresolved external" -errors)
int MouseState::mouseX = 0;
int MouseState::mouseY = 0;
int MouseState::mouseZ = 0;
int MouseState::mouseXSpeed = 0;
int MouseState::mouseYSpeed = 0;
int MouseState::mouseZSpeed = 0;

int MouseState::mouseButtonCount = 0;

bool* MouseState::mouseButtonPressed = NULL;
bool* MouseState::mouseButton = NULL;
bool* MouseState::mouseButtonReleased = NULL;

int MouseState::mouseButtonPressedBits = 0;
int MouseState::mouseButtonBits = 0;
int MouseState::mouseButtonReleasedBits = 0;

bool* MouseState::mouseButtonOld = NULL;

bool MouseState::mouseMoved = false;
float MouseState::mouseIdle = 0.0f;
int MouseState::lastPosX = 0;
int MouseState::lastPosY = 0;

IApplication* MouseState::pApp = NULL;

void MouseState::create(IApplication* pApplication)
{

    pApp = pApplication;

     //init mouse coordinates
    if(pApplication->IsWindowed())
    {
        //Get cursor position
        POINT mousepos;
        ::GetCursorPos(&mousepos);
        ::ScreenToClient(pApplication->GetWindow(), &mousepos);
        mouseX = mousepos.x;
        mouseY = mousepos.y;
    }
    else
    {
        //On fullscreen, set to middle of screen
        mouseX = pApplication->GetWindowRect().right / 2;
        mouseY = pApplication->GetWindowRect().bottom / 2;
    }

    DWORD buttons = Input::mouse.GetButtonCount();
    mouseButtonCount = (int)buttons;
    mouseButtonPressed = new bool[buttons];
    mouseButton = new bool[buttons];
    mouseButtonReleased = new bool[buttons];
    mouseButtonOld = new bool[buttons];

    ::memset(&mouseButtonPressed[0], 0, buttons);
    ::memset(&mouseButton[0], 0, buttons);
    ::memset(&mouseButtonReleased[0], 0, buttons);
    ::memset(&mouseButtonOld[0], 0, buttons);
}


void MouseState::update(const float frameTime)
{
    if(SUCCEEDED(Input::mouse.Update()))
    {
        DIMOUSESTATE2 mouseState = Input::mouse.GetState();
        mouseXSpeed = (int)(mouseState.lX * mouseSens);
        mouseYSpeed = (int)(mouseState.lY * mouseSens);
        mouseZSpeed = mouseState.lZ;

        mouseX += mouseXSpeed;
        mouseY += mouseYSpeed;
        mouseZ += mouseZSpeed;

        //For windowed application, ask cursor position
        if(pApp->IsWindowed())
        {
            //Get cursor position
            POINT mousepos;
            ::GetCursorPos(&mousepos);
            ::ScreenToClient(pApp->GetWindow(), &mousepos);
            mouseX = mousepos.x;
            mouseY = mousepos.y;
        }

        // keep mouse positions inside our window
        if(mouseX <0) 
        {
            mouseX = 0;
        }
        
        if(mouseY <0) 
        {
            mouseY = 0;
        }
        
        const int iMaxX = Input::pApp->GetWindowRect().right;
        if(mouseX > iMaxX) 
        {
            mouseX = iMaxX;
        }

        const int iMaxY = Input::pApp->GetWindowRect().bottom;
        if(mouseY > iMaxY) 
        {
            mouseY = iMaxY;
        }

        for (DWORD i = 0; i < Input::mouse.GetButtonCount(); i++)
        { 
            if (Input::mouse.GetButton(i))
            {
                //Detect the first frame when pressed
                if(!mouseButton[i])
                {
                    mouseButtonPressed[i] = true;
                    mouseButtonPressedBits |= (1 << i);
                } else {
                    mouseButtonPressed[i] = false;
                    mouseButtonPressedBits &= ~(1 << i);
                }
                //Button down
                mouseButton[i] = true;
                mouseButtonReleased[i] = false;
                mouseButtonBits |= (1 << i);
                mouseButtonReleasedBits &= ~(1 << i);
            }
            else
            {
                mouseButton[i] = false;
                mouseButtonPressed[i] = false;
                mouseButtonBits &= ~(1 << i);
                mouseButtonPressedBits &= ~(1 << i);

                //Detect button release
                if(mouseButtonOld[i] == true)
                {
                    mouseButtonReleased[i] = true;
                    mouseButtonReleasedBits |= (1 << i);
                }
                else
                {
                    mouseButtonReleased[i] = false;
                    mouseButtonReleasedBits &= ~(1 << i);
                }
            }

            mouseButtonOld[i] = mouseButton[i];
        }

        // detect movement
        if(mouseX != lastPosX || mouseY != lastPosY)
        {
            lastPosX = mouseX;
            lastPosY = mouseY;
            mouseMoved = true;
            mouseIdle = 0;
        }
        else
        {
            mouseMoved = false;
            mouseIdle += frameTime;
        }

    }
}


void MouseState::transformTo3D(D3DXMATRIX& matView, D3DXMATRIX& matProj, D3DXVECTOR3 &rayOrigin, D3DXVECTOR3 &rayDir, const int mX, const int mY)
{
    float w = (float)Input::pApp->GetWindowRect().right;
    float h = (float)Input::pApp->GetWindowRect().bottom;

    D3DXVECTOR3 v;
    v.x =  (((2.0f * mX) / w) - 1) / matProj._11;
    v.y = -(((2.0f * mY) / h) - 1) / matProj._22;
    v.z =  1.0f;

    D3DXMATRIX m;
    D3DXMatrixInverse( &m, NULL, &matView );

    //Transform the screen space into pick ray in 3D space
    rayDir.x  = v.x * m._11 + v.y * m._21 + v.z * m._31;
    rayDir.y  = v.x * m._12 + v.y * m._22 + v.z * m._32;
    rayDir.z  = v.x * m._13 + v.y * m._23 + v.z * m._33;
    rayOrigin.x = m._41;
    rayOrigin.y = m._42;
    rayOrigin.z = m._43;
}
