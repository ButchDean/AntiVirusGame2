/*!\file ScreenResolutionManagement.cpp
 * \brief Screen resolutions are altered and restored.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

#include <windows.h>


extern char szWinName[];						// name of window class.

/*! Device mode */
DEVMODE dm;

bool CheckCurrentResolution(int width, int height)
// Check screen resolution.
{
	memset(&dm, 0, sizeof(DEVMODE));	// Reset dm values.

	if(!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm))		// Retrieve information to see if correct setting is set/available.
	{
		MessageBox(NULL, "Unable to determine display settings", "Display settings exception...", MB_ICONSTOP | MB_OK);
		MessageBox(NULL, "Returning to desktop settings", "Display settings exception...", MB_ICONINFORMATION | MB_OK);
		return true;					// Leave resolution at current setting.
	}

	// Check current screen resolution.
	if(dm.dmPelsWidth != width || dm.dmPelsHeight != height)
		return false;					// If the current screen resolution doesn't match the current resolution, return false.
	else
		return true;					// If the current resolution does match the current resolution, resolution will remain unchanged.
}

void ChangeScreenResolution(int width, int height)
// Alter the screen resolution.
{
	dm.dmPelsWidth	= width;					
	dm.dmPelsHeight	= height;					
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	if(ChangeDisplaySettings(&dm, 0) != DISP_CHANGE_SUCCESSFUL)		// If resolution change is not successful.
	{
		MessageBox(NULL, "Resolution not supported!", "Display settings...", MB_ICONSTOP | MB_OK);
		MessageBox(NULL, "Returning to desktop settings", "Display settings exception...", MB_ICONINFORMATION | MB_OK);
		return;
	}
}

bool SwitchToFullScreen(HWND &passedhWnd, HINSTANCE &passedhInst, RECT winDimentions, DWORD &exStyle, DWORD &style, DWORD scrWidth, DWORD scrHeight, DWORD bitsPerPel)
{
	memset(&dm,0,sizeof(dm));
	dm.dmSize = sizeof(dm);	
	dm.dmPelsWidth = scrWidth;		
	dm.dmPelsHeight = scrHeight;		
	dm.dmBitsPerPel = bitsPerPel;		
	dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	exStyle = WS_EX_APPWINDOW;		
	style = WS_POPUP;	

	AdjustWindowRectEx(&winDimentions, style, false, exStyle);

	// Create a window.
	passedhWnd = CreateWindowEx(
					exStyle,
					szWinName,     
					NULL,   
					style | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
					0, 0,
					winDimentions.right - winDimentions.left, 
					winDimentions.bottom - winDimentions.top, 
					NULL,          
					NULL,          
					passedhInst,     
					NULL );

	if(ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		// setting display mode failed, switch to windowed
		return false;
	else
		return true;
}

bool SwitchToWindowed(HWND &passedhWnd, HINSTANCE &passedhInst)
{
	passedhWnd = CreateWindowEx(
					NULL,								
					szWinName,						
					"AntiVirus 2 Version 2.1 by Dean N. Butcher (2004/5)",	
					WS_VISIBLE | 
					WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
					100, 100,							
					800, 600,							
					NULL,								
					NULL,								
					passedhInst,						
					NULL);

	if(passedhWnd != NULL)
		return true;
	else
		return false;
}