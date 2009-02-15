/*
	Header for console-module
*/

/*
Date/Ver	Author				Comment
070420		Esa J‰‰skel‰inen	Imported this library from an old project, some modifications,
								translation to english... The whole thing really needs a rewrite
								and some additional features
*/


//Include guards
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

//Include needed headers
#include <windows.h>

//Change to #define WriteConsoleOutput WriteConsoleOutputW  to use wide-characters (Unicode)
#ifdef WriteConsoleOutput
	#undef WriteConsoleOutput
#endif
#define WriteConsoleOutput WriteConsoleOutputA

class CConsole
{
public:

	/**
	 * Size of console window
	 */
	static short swScreenSizeX, swScreenSizeY;

	/**
		Pointer to CHAR_INFO -buffer, which includes all the char and color data to display.
		CHAR_INFO supports fore- and backcolor and unicode. Public so it can be modified
		at will.
	*/
	CHAR_INFO* pCHAR_INFOScreenBuffer;
	
	
	/**
		Constructor
	*/
	CConsole();

	/**
		Sets console size.
		\param wSizeX		Buffer width in characters
		\param wSizeY		Buffer heigth in characters
		\return 0 if succeeded, -1 if the maximum width or height is lower than given or -2 if the
			sizing didn't succeed for some other reason
	*/
	int SetConsoleSize(SHORT wSizeX, SHORT wSizeY);


	/**
		Clears the screen (but not fore- and background colors)
	*/
	void ClearScreen(void);

	/**
		Clears the buffer (better way to clear screen, if whole screen is drawn at once)
	*/
	void ClearBuffer(void);

	/**
		Non-blocking method to read input events.
		\param pInputRecord		Pointer to INPUT_RECORD
		\return 0 if no errors occured and there were no new events, 1 no errors and an event has taken place or
			-1 if it fails
	*/
	/*
	EXAMPLE OF USAGE within a method:
		
		static INPUT_RECORD rInput;
		
		char key = 0;

		int iReturnCode = pConsole->ReadInput(rInput);
		
		//Check that iReturnCode is other than 0 (so old input won't be used twice), eventtype and keydown etc. (if needed)
		if(rInput.EventType == KEY_EVENT && iReturnCode && rInput.Event.KeyEvent.bKeyDown == TRUE)
		{
			key = rInput.Event.KeyEvent.uChar.AsciiChar;
		}
		else if(rInput.EventType == MOUSE_EVENT && iReturnCode)
		{			
			pPlayer.iMouseX = rInput.Event.MouseEvent.dwMousePosition.X;
			pPlayer.iMouseY = rInput.Event.MouseEvent.dwMousePosition.Y;
			return 0;
		}
		else
		{
			return 0;
		}
	*/
	int ReadInput(INPUT_RECORD* pInputRecord);


	/**
		Sets name (in the titlebar) of the console window
		\param strName		Pointer to null-terminated string to use as the name 
		\return 0 if succeeded, otherwise -1
	*/

	int NameConsole(char strName[]);

	/**
		Draws the buffer to screen.
		\return 0 if succeeded, otherwise -1
	*/
	int DrawScreen();


	/**
		Draws given part of the buffer to screen.
		\BufferSize			COORD-value that definex xy-size
		\BufferLocation		COORD-value that defines xy-location
		\rArea				Reference to SMALL_RECT
		\return 0 if succeeded, otherwise -1
	*/
	int DrawPartOfScreen(COORD BufferSize, COORD BufferLocation, SMALL_RECT &rArea);

	/**
		Sets the cursor style.
		\param dwSize		DWORD for the cursor size (1-100, how much of the character is filled when cursor blinks)
		\param cVisible		Is the cursor visible, 0 or 1
	*/
	void SetCursorSize(DWORD dwSize, char cVisible);

	/**
		Write a message to screen at given location
		\param iCol			Column to begin writing at (x)
		\param iRow			Row to write at (y)
		\param strMsg		Zero-terminated character string to write		
		\param iAttributes	Attributes for text, defaults to FOREGROUND_GREEN | FOREGROUND_INTENSITY
	*/
	void writeMessage(int iCol, int iRow, char* strMsg, int iAttributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY);

private:
	
	/**
		Console's input-stream handle
	*/
	HANDLE	hIn;

	/**
		Console's output-stream handle
	*/
	HANDLE hOut;

	/**
		Console's error-stream handle
	*/
	HANDLE hErr;

};


#endif