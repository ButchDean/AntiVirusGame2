/*!\file DirectInputIncludes.h
 * \brief Function prototypes.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

// Function prototypes for DirectInput source

#ifndef _DIRECTINPUT_SETUP_
#define _DIRECTINPUT_SETUP_

#include <windows.h>
#include <dinput.h>

#define KEYDOWN(buffer, key) (key[buffer] & 0x80)	// Keypressed macro.

bool DirectInputDevicesInitialisation(HINSTANCE &hInst);
bool DirectInputCheckForKeyboard();
bool DeleteDirectInputResources();

#endif