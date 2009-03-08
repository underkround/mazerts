/**
 * C3DObjectControllerKeyboard.cpp source file
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 19.2.2009
 * 
 * basic 'fly-around' object controller
 * using keyboard.
 */

#include "C3DObjectControllerKeyboard.h"


C3DObjectControllerKeyboard::C3DObjectControllerKeyboard(void)
{
}


C3DObjectControllerKeyboard::~C3DObjectControllerKeyboard(void)
{
}


void C3DObjectControllerKeyboard::Update(float fFrametime)
{
    float fCurrentSpeed = 0.0f;
    const float fMaxSpeed = 40.0f;
    const float fMaxTurnSpeed = 2.0f;

    D3DXMATRIX mRotationX;
    D3DXMATRIX mRotationY;
    D3DXMatrixIdentity(&mRotationX);
    D3DXMatrixIdentity(&mRotationY);

    // read forward / backwards movement
    if (::GetAsyncKeyState('A'))
    {
        fCurrentSpeed = fMaxSpeed;
    }
    else if (::GetAsyncKeyState('Z'))
    {
        fCurrentSpeed = -fMaxSpeed;
    }


    // read turn left & right controls
    if (::GetAsyncKeyState(VK_LEFT))
    {
        D3DXMatrixRotationY(    &mRotationY,
                                -fMaxTurnSpeed * fFrametime);
    }
    else if (::GetAsyncKeyState(VK_RIGHT))
    {
        D3DXMatrixRotationY(    &mRotationY,
                                fMaxTurnSpeed * fFrametime);
    }


    // apply movements to the object
    D3DXMATRIX& mObject = m_pObject->GetMatrix();



    // read up & down controls
    if (::GetAsyncKeyState(VK_UP))
    {
        D3DXMatrixRotationAxis(    &mRotationX,
                                &D3DXVECTOR3(&mObject._11),
                                fMaxTurnSpeed * fFrametime);
    }
    else if (::GetAsyncKeyState(VK_DOWN))
    {
        D3DXMatrixRotationAxis(    &mRotationX,
                                &D3DXVECTOR3(&mObject._11),
                                -fMaxTurnSpeed * fFrametime);
    }




    // get the object current position
    D3DXVECTOR3 pos(&mObject._41);

    // add speed into the position
    pos.x += mObject._31 * fCurrentSpeed * fFrametime;
    pos.y += mObject._32 * fCurrentSpeed * fFrametime;
    pos.z += mObject._33 * fCurrentSpeed * fFrametime;

    // apply rotations to object matrix
    D3DXMATRIX mTemp;
    D3DXMatrixMultiply(&mTemp, &mRotationX, &mRotationY);
    D3DXMatrixMultiply(&mObject, &mObject, &mTemp);


    // set modified position back to object matrix
    mObject._41 = pos.x;
    mObject._42 = pos.y;
    mObject._43 = pos.z;
}
