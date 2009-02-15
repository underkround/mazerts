/*	Konsoli-kirjaston koodi

Date		Author				Comment
24.2.2006	Esa J‰‰skel‰inen	TyhjennaRuutu-funktio, n‰pp‰in-eventtien ei-keskeytt‰v‰ (jatkuva) luku -funktio
25.2.2006	Esa J‰‰skel‰inen	NimeaKonsoli-funktio
8.3.2006	Esa J‰‰skel‰inen	PiirraKonsoli-funktio, pieni‰ muutoksia aiempiin funktioihin
13.3.2006	Esa J‰‰skel‰inen	PiirraOsaKonsoli-funktio, AsetaKonsolinKoko-funktio + pieni‰ muutoksia aiempiin funktioihin
8.4.2006	Esa J‰‰skel‰inen	Lis‰tty AsetaKursori-funktio
12.5.2006	Esa J‰‰skel‰inen	Koko moduulin toiminta siirretty CConsole-luokkaan
070420		Esa J‰‰skel‰inen	Translation to english, ClearBuffer()
								This whole library needs a major rewrite =P
*/

#include "CConsole.h"

#ifdef _DEBUG
#include "stdio.h"
#endif

short CConsole::swScreenSizeX = 80;
short CConsole::swScreenSizeY = 50;

//Constructor
CConsole::CConsole()
{
	//Get handles
 	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hErr = GetStdHandle(STD_ERROR_HANDLE);

	pCHAR_INFOScreenBuffer = NULL;

	SetConsoleSize(80, 50);	
}


int CConsole::SetConsoleSize(SHORT wSizeX, SHORT wSizeY)
{
	CConsole::swScreenSizeX = wSizeX;
	CConsole::swScreenSizeY = wSizeY;

	//If old buffer exists, destroy it
	if(pCHAR_INFOScreenBuffer)
	{
		delete [] pCHAR_INFOScreenBuffer;
		pCHAR_INFOScreenBuffer = NULL;
	}

	int iSize = wSizeX * wSizeY;

	pCHAR_INFOScreenBuffer = new CHAR_INFO[iSize];

	for(int i = 0; i < iSize; i++)
	{
		pCHAR_INFOScreenBuffer[i].Char.AsciiChar = ' ';
		pCHAR_INFOScreenBuffer[i].Attributes = 0x00;
	}
	
	//New sizes for buffer (-1 because the coordinates are 0-based)
	SMALL_RECT Size = {0, 0, (wSizeX-1), (wSizeY-1)};
	COORD BufferSize = {wSizeX, wSizeY};

	//Buffer-structure
	CONSOLE_SCREEN_BUFFER_INFO Buffer;

	//Get current buffer info
	GetConsoleScreenBufferInfo(hOut, &Buffer);

	//Check that maximum sizes aren't exceeded
	if((Buffer.dwMaximumWindowSize.X) < (wSizeX-1) || (Buffer.dwMaximumWindowSize.Y) < (wSizeY-1))
	{
		//Trying to set too big buffer, return error code
		return -1;
	}
	else
	{
		//Set console size
		int iReturnValue = SetConsoleWindowInfo(hOut, TRUE, &Size);

		if (iReturnValue == 0)
		{

#ifdef _DEBUG
			//Get error code
			iReturnValue = GetLastError();
			printf("SetConsoleWindowInfo failed, %s\n", strerror_s((char*)stdout, 200, iReturnValue));
			Sleep(4000);
#endif

			//Return own error code
			return -2;
		}
		
		//Setting size succeeded, change the buffer size to match it
		SetConsoleScreenBufferSize(hOut, BufferSize);

		//All ok
		return 0;
	}


}




void CConsole::ClearScreen()
{

	//Clear screen

	//COORD-struct
	COORD Location;
	Location.X = 0;
	Location.Y = 0;
	
	//Number of characters written
	//DWORD iNumChars = 0;

	//Fill screen with spaces (won't clear out colors!)
	FillConsoleOutputCharacter(hOut, ' ', (swScreenSizeX * swScreenSizeY-1), Location, NULL);//&iNumChars);

	//Set cursor back to upper left corner
	SetConsoleCursorPosition(hOut, Location);

}

void CConsole::ClearBuffer()
{
	
	int iSize = this->swScreenSizeX * this->swScreenSizeY;

	for(int i = 0; i < iSize; ++i)
	{
		this->pCHAR_INFOScreenBuffer[i].Attributes = 0x00;
		this->pCHAR_INFOScreenBuffer[i].Char.AsciiChar = ' ';		
	}

}

int CConsole::ReadInput(INPUT_RECORD* pInputRecord)
{

	//Eventcounter
	DWORD dwEventCounter;
	
	//Number of inputs read
	DWORD dwRead;

	//Error
	BOOL bError;
	
	//Check whether there are any input events, the number will be written to dwEventCounter
	GetNumberOfConsoleInputEvents(hIn, &dwEventCounter);
    
	//Read event (if one occurred)
	if (dwEventCounter > 0)
        {
			/*ReadConsoleInput will be called only if event count is more than zero (to prevent blocking when
			no input has occurred)*/
            
			bError = ReadConsoleInput(hIn, pInputRecord, 1, &dwRead);
			if(bError == FALSE) return -1;

			//Input has occurred, return 1 so the caller knows to check the pInputRecord contents
			return 1;
		}
	
	//No errors, no events
	return 0;
}


int CConsole::NameConsole(char strName[])
{
	
	//Return value
	unsigned int uiReturnValue;

	//Set console name
	uiReturnValue = SetConsoleTitleA(strName);
    
	if (uiReturnValue == 0)
    {

#ifdef _DEBUG
		//Get error code
		int iReturnValue = GetLastError();
		printf("SetConsoleTitle failed, %s\n", strerror_s((char*)stdout, 200,iReturnValue));
#endif

		//Error occurred
        return -1;
    }
	
	//No errors
	return 0;
}


int CConsole::DrawScreen()
{

	//Return value
	int iReturnValue;
	
	//Defines how big part of the buffer is drawn to screen
	COORD BufferSize = {swScreenSizeX, swScreenSizeY};
	
	//Defines where to start drawing from
	COORD BufferLocation = { 0, 0 }; 

	//Defines area to draw in screen coordinates
	SMALL_RECT Area = {0, 0, swScreenSizeX-1, swScreenSizeY-1};
	
	iReturnValue = WriteConsoleOutput(hOut, pCHAR_INFOScreenBuffer, BufferSize, BufferLocation, &Area);

	if (iReturnValue == 0)
    {

#ifdef _DEBUG
		//Get error code
        iReturnValue = GetLastError();
		printf("WriteConsoleOutput failed, %s\n", strerror_s((char*)stdout, 200,iReturnValue));
#endif

        return -1;
    }

	return 0;
}


int CConsole::DrawPartOfScreen(COORD BufferSize, COORD BufferLocation, SMALL_RECT &Area)
{

	//Return value
	int iReturnValue;
	
	iReturnValue = WriteConsoleOutput(hOut, pCHAR_INFOScreenBuffer, BufferSize, BufferLocation, &Area );

	if (iReturnValue == 0)
    {

#ifdef _DEBUG
		//Error code
		iReturnValue = GetLastError();
		printf("WriteConsoleOutput failed, %s\n", strerror_s((char*)stdout, 200,iReturnValue));
#endif

        return -1;
    }
	return 0;
}

void CConsole::SetCursorSize(DWORD dwSize, char cVisible)
{
	//Cursor struct
	CONSOLE_CURSOR_INFO Cursor;

	//Visible
	Cursor.bVisible = cVisible;

	//If dwSize is 0, SetConsoleCursorInfo will fail!
	if(dwSize < 1) dwSize = 1;
	if(dwSize > 100) dwSize = 100;
	Cursor.dwSize = dwSize;

	//Change the cursor
    SetConsoleCursorInfo(hOut, &Cursor);
}


void CConsole::writeMessage(int iCol, int iRow, char* strMsg, int iAttributes)
{
	int i = 0;

	//Go through the message character by character
	while(strMsg[i] != '\0')
	{
		//Calculate character location
		int iLoc = (iCol + i) + (iRow * swScreenSizeX);

		//Set character and color
		pCHAR_INFOScreenBuffer[iLoc].Char.AsciiChar = strMsg[i];
		pCHAR_INFOScreenBuffer[iLoc].Attributes = iAttributes;

		//Increment index
		++i;
	}	
}