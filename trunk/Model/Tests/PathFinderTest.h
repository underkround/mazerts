#include "../Terrain/Terrain.h"
#include "../Pathfinding/PathFinder.h"
#include "../Common/CConsole.h"
#include "CTimer.h"
#include <stdlib.h>
#include <stdio.h>

#include "../Asset/Unit.h"

void testPathFinder()
{
    Terrain::getInstance()->initialize(Terrain::DEFAULT_MAPSIZE);
    Terrain* terrain = Terrain::getInstance();    
    
    CConsole* pConsole = new CConsole();
    //pConsole->SetConsoleSize(terrain.getSize(), terrain.getSize());
    pConsole->ClearBuffer();
    
    
    srand(100000);
    
    for(int i = 0; i < 100; i++)
    {
        int len = rand() % 10;
        int x = rand() % pConsole->swScreenSizeX;
        int y = rand() % pConsole->swScreenSizeY;
        
        for(int j = 0; j < len; j++)
        {
            terrain->setTerrainVertexHeight(x++, y, j*30 % 255);
        }
    }

    /*
    for(int i = 0; i < 40; i++)
    {
        terrain->setTerrainVertexHeight(20+i, 0, 0);
        terrain->setTerrainVertexHeight(60, i, 0);
        terrain->setTerrainVertexHeight(20+i, 40, 0);        
        terrain->setTerrainVertexHeight(20, 20+(i %20), 0);

        terrain->setTerrainVertexHeight(60+(i%16), 20, 0);
        terrain->setTerrainVertexHeight(76, 20+(i % 20), 0);
    }*/
    
    /*for(int i = 0; i < pConsole->swScreenSizeY; i++)
    {
        for(int j = 0; j < pConsole->swScreenSizeX; j++)
        {
            int loc = pConsole->swScreenSizeX * i + j;
            if(!terrain->isPassable(j, i))
            {
                pConsole->pCHAR_INFOScreenBuffer[loc].Attributes = FOREGROUND_RED | BACKGROUND_RED;
            }
        }
    }*/
    
    
    CTimer* timer = new CTimer();
    timer->Create();

//    Unit* pUnit = new Unit();
//    pUnit->setWidth(2);

    char* strMsg = new char[100];
    int mostSteps = 0;
    float worstTime = 0.0f;

    //How many paths to find
    for(int k = 0; k < 20; k++)
    {
        pConsole->ClearBuffer();
        for(int i = 0; i < pConsole->swScreenSizeY; i++)
        {
            for(int j = 0; j < pConsole->swScreenSizeX; j++)
            {
                int loc = pConsole->swScreenSizeX * i + j;
                if(!terrain->isPassable(j, i, 1))
                {
                    pConsole->pCHAR_INFOScreenBuffer[loc].Char.AsciiChar = 'E';
                    pConsole->pCHAR_INFOScreenBuffer[loc].Attributes = FOREGROUND_RED | BACKGROUND_RED;
                }
            }
        }


        float tempTime = 0.0f;
        int steps = 0;

        //Measeure creation time
        timer->BeginTimer();
        PathFinder* pFinder = new PathFinder(0, 0, rand() % 80, rand() % 50, 1);  //, 78, 38);
        timer->EndTimer();
        tempTime += timer->GetElapsedSeconds();
            
        PathFinder::PathingState pathState = PathFinder::NOT_FINISHED;

        while(pFinder->isInitialized() && pathState == PathFinder::NOT_FINISHED)
        {
            timer->BeginTimer();
            pathState = pFinder->advance(10);
            timer->EndTimer();                        

            tempTime += timer->GetElapsedSeconds();
            //steps = pFinder->DEBUG_steps;
            
            for(int i = 0; i < pConsole->swScreenSizeY; i++)
            {
                for(int j = 0; j < pConsole->swScreenSizeX; j++)
                {
                    int loc = pConsole->swScreenSizeX * i + j;
                    /*if(pFinder->m_pppNodeArray[i][j])
                    {
                        pConsole->pCHAR_INFOScreenBuffer[loc].Attributes = FOREGROUND_BLUE | BACKGROUND_BLUE;
                    }*/
                }
            }

            sprintf_s(strMsg, 100, "Askelia: %d  Aika: %.2fms", steps, tempTime * 1000.0f);
            pConsole->writeMessage(20, 0, strMsg);
            pConsole->DrawScreen();
            Sleep(1);
        }
                

        /*PathFinder::PathNode* pCurrent = pFinder->getPath();    

        timer->BeginTimer();
        delete pFinder;
        timer->EndTimer();

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
        }*/
        
        sprintf_s(strMsg, 100, "%d polkua haettu", k);
        pConsole->writeMessage(5, 47, strMsg);
        
        float time = ((tempTime + timer->GetElapsedSeconds()) * 1000.0f);
        if(time > worstTime)
            worstTime = time;
        if(steps > mostSteps)
            mostSteps = steps;

        sprintf_s(strMsg, 100, "%.3fms PathFinder luonti, haku, poisto", time);
        pConsole->writeMessage(20, 0, strMsg);

        sprintf_s(strMsg, 100, "Hitain: %.2fms, askelia: %d", worstTime, mostSteps);
        pConsole->writeMessage(5, 48, strMsg);

        sprintf_s(strMsg, 100, "%d askelta edellisessä haussa", steps);
        pConsole->writeMessage(5, 49, strMsg);
        pConsole->DrawScreen();
        
        static INPUT_RECORD record;
        memset(&record, 0, sizeof(INPUT_RECORD));
        while(record.EventType != KEY_EVENT)
        {
            pConsole->ReadInput(&record);

            //Wait for key up
            while(record.Event.KeyEvent.bKeyDown)
            {
                pConsole->ReadInput(&record);
            }
            
        }
    }

    timer->Release();
    delete timer;

//    delete pUnit;

    delete pConsole;
}