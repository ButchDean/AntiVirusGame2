/*!\file RestoreGameResolution.h
 * \brief Function prototypes.
 * \author Dean N. Butcher
 * \version 2.1
 * \date November 2004
 */

#ifndef _RESTORE_RES_CONVERSION_
#define _RESTORE_RES_CONVERSION_

bool WriteResFile(int width, int height);
void FetchAndConvertStringToInt(char resolution[], size_t sizeBytes);
bool FetchResolution();

#endif