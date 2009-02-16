#include "../Terrain/Terrain.h"
#include "../Unit/Unit.h"
#include "../Pathfinding/PathFinder.h"
#include "../Common/CConsole.h"
#include "CTimer.h"
#include <stdlib.h>
#include <stdio.h>

void testPathFinder()
{
    Terrain* terrain = Terrain::getInstance();    
    
    CConsole* pConsole = new CConsole();
    //pConsole->SetConsoleSize(terrain.getSize(), terrain.getSize());
    pConsole->ClearBuffer();
    
    
    srand(100000);
    
    for(int i = 0; i < 1000; i++)
    {
        /*srand(rand());
        int len = rand() % 10;
        int x = rand() % pConsole->swScreenSizeX;
        int y = rand() % pConsole->swScreenSizeY;
        
        for(int j = 0; j < len; j++)
        {
            terrain->setTerrainVertexHeight(x++, y, j*30 % 255);
        }*/
    }

    for(int i = 0; i < 40; i++)
    {
        terrain->setTerrainVertexHeight(20, i, i*30 % 255);
        terrain->setTerrainVertexHeight(i+24, 25, i*30 % 255);
        terrain->setTerrainVertexHeight(i+44, 25, i*30 % 255);
        terrain->setTerrainVertexHeight(60, 45-i, i*30 % 255);
    }
    
    for(int i = 0; i < pConsole->swScreenSizeY; i++)
    {
        for(int j = 0; j < pConsole->swScreenSizeX; j++)
        {
            int loc = pConsole->swScreenSizeX * i + j;
            if(!terrain->isPassable(j, i))
            {
                pConsole->pCHAR_INFOScreenBuffer[loc].Attributes = FOREGROUND_RED | BACKGROUND_RED;
            }
        }
    }

    CTimer* timer = new CTimer();
    timer->Create();

    Unit* pUnit = new Unit();
    
    char* strMsg = new char[100];
    int steps = 0;

    for(int k = 0; k < 2000; k++)
    {
        timer->BeginTimer();
        PathFinder* pFinder = new PathFinder(pUnit, rand() % 80, rand() % 50);  //, 78, 38);
        
        while(!pFinder->advance(100))
        {
        }

        PathFinder::PathNode* pCurrent = pFinder->getPath();    
        
        steps = pFinder->DEBUG_steps;

        delete pFinder;
        timer->EndTimer();        

        pConsole->ClearBuffer();
        for(int i = 0; i < pConsole->swScreenSizeY; i++)
        {
            for(int j = 0; j < pConsole->swScreenSizeX; j++)
            {
                int loc = pConsole->swScreenSizeX * i + j;
                if(!terrain->isPassable(j, i))
                {
                    pConsole->pCHAR_INFOScreenBuffer[loc].Attributes = FOREGROUND_RED | BACKGROUND_RED;
                }
            }
        }

        while(pCurrent != NULL)
        {            
            if(pCurrent->pParent)
            {
                delete pCurrent->pParent;
            }

            int loc = pConsole->swScreenSizeX * pCurrent->y + pCurrent->x;
            pConsole->pCHAR_INFOScreenBuffer[loc].Char.AsciiChar = '@';
            pConsole->pCHAR_INFOScreenBuffer[loc].Attributes = FOREGROUND_GREEN;
            
            if(pCurrent->pChild)
            {
                pCurrent = pCurrent->pChild;
            }
            else
            {
                delete pCurrent;
                break;
            }
        }

        sprintf_s(strMsg, 100, "%d polkua haettu", k);
        pConsole->writeMessage(5, 47, strMsg);

        sprintf_s(strMsg, 100, "%.3fms PathFinder luonti, haku, poisto", (timer->GetElapsedSeconds() * 1000.0f));
        pConsole->writeMessage(5, 48, strMsg);

        sprintf_s(strMsg, 100, "%d askelta haussa", steps);
        pConsole->writeMessage(5, 49, strMsg);


        pConsole->DrawScreen();
    }

    timer->Release();
    delete timer;

    delete pUnit;
    delete pConsole;
}