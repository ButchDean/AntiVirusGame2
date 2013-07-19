/*!\file LoadBitmaps.h
 * \brief Function prototypes.
 * \author Dean N. Butcher
 * \version 2.0
 * \date August 2004
 */

// Header file containing function ddeclarations for loading bitmap files.

#ifndef _LOADBITMAP_
#define _LOADBITMAP_		// Guarantee single instance of functions.

#include <windows.h>

unsigned char *LoadBitmapFile(char *bmpFile, BITMAPINFOHEADER *bitmapInfoHeader);
void CleanUpLoadBitmap();

#endif