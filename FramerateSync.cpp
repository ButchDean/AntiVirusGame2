/*!\file CPUTimer.cpp
 * \brief Handles CPU timings to scale performance to system speed.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

#include <windows.h>
#include "FramerateSync.h"

/* The start time for this iteration. */
DWORD lastTime;
/* The end time for this iteration. */
DWORD thisTime;

void FetchCurrentTime()
{
	lastTime = timeGetTime();
}

bool CheckTimeDelta()
{
	const DWORD timeDelta = 35;	// Delay in ms.

	thisTime = timeGetTime();

	if((thisTime - lastTime) < timeDelta)
		return false;			// Do not allow game loop to iterate.
	else
	{
		lastTime = thisTime;	// Store new time checkpoint.
		return true;			// Permit game loop to iterate.
	}
}