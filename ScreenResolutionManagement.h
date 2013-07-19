/*!\file ScreenResolutionManagement.h
 * \brief Function prototypes.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

#ifndef _RESOLUTION_SETTINGS_
#define _RESOLUTION_SETTINGS_

bool CheckCurrentResolution(int width, int height);
void ChangeScreenResolution(int width, int height);
bool SwitchToFullScreen(HWND &passedhWnd, HINSTANCE &passedhInst, RECT winDimentions, DWORD &exStyle, DWORD &style, DWORD scrWidth, DWORD scrHeight, DWORD bitsPerPel);
bool SwitchToWindowed(HWND &passedhWnd, HINSTANCE &passedhInst);

#endif