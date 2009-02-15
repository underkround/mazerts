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
    
    
    srand(10000);
    
    for(int i = 0; i < 1000; i++)
    {
        srand(rand());
        int len = rand() % 10;
        int x = rand() % (terrain->getSize() >> 1);
        int y = rand() % (terrain->getSize() >> 1);
        
        for(int j = 0; j < len; j++)
        {
            terrain->setTerrainVertexHeight(x++, y, j*10);
        }
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
    char* cCount = new char[100];
    char* cTime = new char[100];
    for(int k = 0; k < 2000; k++)
    {
        timer->BeginTimer();
        PathFinder* pFinder = new PathFinder(pUnit, rand() % 80, rand() % 50);  //, 78, 38);
        
        while(!pFinder->advance(100))
        {
        }

        PathFinder::PathNode* pCurrent = pFinder->getPath();    
                
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

        sprintf_s(cCount, 100, "%d polkua haettu", k);
        sprintf_s(cTime, 100, "%.3fms PathFinder luonti, haku, poisto", timer->GetElapsedSeconds());
        
        pConsole->writeMessage(5, 44, cCount);
        pConsole->writeMessage(5, 45, cTime);
        pConsole->DrawScreen();
    }

    timer->Release();
    delete timer;

    delete pUnit;
    delete pConsole;
}