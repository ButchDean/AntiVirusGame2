/*!\file StandardIncludes.h
 * \brief Program setup.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

// This header file includes all header files included in most, if not all, of the source files.

#ifndef _STANDARD_INCLUDES_
#define _STANDARD_INCLUDES_

#define DIRECTINPUT_VERSION 0x9000

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <mmsystem.h>
#include <dsound.h>
#include "GameMessages.h"

#endif
