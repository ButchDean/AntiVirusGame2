/*!\file DirectInputSource.cpp
 * \brief Functions to handle input using DirectInput.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

// These are the routines that support DirectInput that 
// offers the alternative to Win32 messaging.

#define DIRECTINPUT_VERSION 0x0700		// create definitions for DirectX 7.

#include <windows.h>
#include <dinput.h>
#include "DirectInputIncludes.h"

const char			DIERROR[] = "DirectInput Error...";
const int			KEYBUFCOUNT = 256;
LPDIRECTINPUT		lpDI;
LPDIRECTINPUTDEVICE pKeyboard;
char                keyboardBuffer[KEYBUFCOUNT];
HRESULT				directInputErrorDesc; // Handle used to store Direct Input Error.

extern HWND ghwnd;

bool DirectInputDevicesInitialisation(HINSTANCE &hInst)
{
	// Create DI object.
	if(DirectInputCreate(hInst, DIRECTINPUT_VERSION, &lpDI, NULL) != DI_OK)
    {
		MessageBox(NULL, "Failed to create DirectInput Object", DIERROR, MB_OK | MB_ICONEXCLAMATION);
        return false;
    }
	
	// Create device
	if(lpDI->CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL) != DI_OK)
    {
		MessageBox(NULL, "Failed to create system keyboard", DIERROR, MB_OK | MB_ICONEXCLAMATION);
		return false;
    }

	// Set data format for keyboard
	if(pKeyboard->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
    {
		MessageBox(NULL, "Failed to set data format for system keyboard", DIERROR, MB_OK | MB_ICONEXCLAMATION);
		pKeyboard->Release();
		pKeyboard = NULL;
        return false;
    }

	// Set keyboard's priority
    if(pKeyboard->SetCooperativeLevel(ghwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
    {
		MessageBox(NULL, "Failed to set cooperative level for system keyboard", DIERROR, MB_OK | MB_ICONEXCLAMATION);
		pKeyboard->Release();
		pKeyboard = NULL;
        return false;
    }

	if(!DirectInputCheckForKeyboard())
		return false;	//The keyboard was not grabbed!

	// All keyboard initialisation attempts successful.
	return true;
}

bool DirectInputCheckForKeyboard()
{
	// Acquire the keyboard device
    if((directInputErrorDesc = pKeyboard->Acquire()) != DI_OK)
    {
		switch(directInputErrorDesc)
		{
			case DIERR_INVALIDPARAM:
			{
				MessageBox(NULL, "Failed to aquire system keyboard - INVALID PARAMETER", DIERROR, MB_OK | MB_ICONEXCLAMATION);
				break;
			}
			case DIERR_NOTINITIALIZED:
			{
				MessageBox(NULL, "Failed to aquire system keyboard - NOT INITIALISED", DIERROR, MB_OK | MB_ICONEXCLAMATION);
				break;
			}
			case DIERR_OTHERAPPHASPRIO:
			{
				MessageBox(NULL, "Failed to aquire system keyboard - OTHER APPLICATION HAS PRIORITY", DIERROR, MB_OK | MB_ICONEXCLAMATION);
				break;
			}
			default:
			{
				MessageBox(NULL, "Failed to aquire system keyboard - ERROR UNKNOWN", DIERROR, MB_OK | MB_ICONEXCLAMATION);
				break;
			}
		}
		pKeyboard->Release();
		pKeyboard = NULL;
        return false;
    }
	return true;
}

bool DeleteDirectInputResources()
{
	if(pKeyboard)
	{
		pKeyboard->Unacquire();
		pKeyboard->Release();
		pKeyboard = NULL;
	}

	// Just incase something should go wrong.
	if(pKeyboard) // If the keyboard should still exist.
	{
		MessageBox(NULL, "Keyboard device freed without having been acquired.", DIERROR, MB_OK | MB_ICONEXCLAMATION);
	}

	if(lpDI)
	{
		lpDI->Release();
		lpDI = NULL;
	}

	// Just incase something should go wrong.
	if(lpDI) // If the keyboard object should still exist.
	{
		MessageBox(NULL, "Keyboard object freed without having been acquired.", DIERROR, MB_OK | MB_ICONEXCLAMATION);
	}

	if(!pKeyboard && !lpDI)  // If releases were successful
		return true;
	else
		return false;
}