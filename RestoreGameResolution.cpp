/*!\file RestoreGameResolution.cpp
 * \brief Restores game screen resolution when reentering from menuing system.
 * \author Dean N. Butcher
 * \version 2.1
 * \date November 2004
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "ScreenResolutionManagement.h"

bool WriteResFile(int width, int height)
{
	const char RESOLUTION[][10] = {"640 480", "800 600", "1024 768", "1280 1024"};

	FILE *resFile = NULL;

	if((resFile = fopen("csr.a2g", "w")) == NULL)
	{
		MessageBox(NULL, "Can't create resolution file! Defaulting to 800 x 600.", "Resolution Write...", MB_ICONEXCLAMATION | MB_OK);
		ChangeScreenResolution(800, 600);
		return false;
	}

	//	Determine resolution required.
	if(width == 640 && height == 480)
		fwrite(RESOLUTION[0], sizeof(char), sizeof(RESOLUTION[0]), resFile);
	else
		if(width == 800 && height == 600)
			fwrite(RESOLUTION[1], sizeof(char), sizeof(RESOLUTION[1]), resFile);
		else
			if(width == 1024 && height == 768)
				fwrite(RESOLUTION[2], sizeof(char), sizeof(RESOLUTION[2]), resFile);
			else
				if(width == 1280 && height == 1024)
					fwrite(RESOLUTION[3], sizeof(char), sizeof(RESOLUTION[3]), resFile);
				else
				{
					MessageBox(NULL, "Resolution not supported!", "Resolution Change...", MB_ICONEXCLAMATION | MB_OK);
					fclose(resFile);
					return false;
				}

	fclose(resFile);
	return true;
}

void FetchAndConvertStringToInt(char resolution[], size_t sizeBytes)
{
	const char SPACE = ' ';

	char textCoordinates[2][5];
	int resWidth, resHeight;

	if(resolution[3] == SPACE)	// resolution is of the form xxx yyy
	{
		textCoordinates[0][0] = resolution[0];
		textCoordinates[0][1] = resolution[1];
		textCoordinates[0][2] = resolution[2];
		textCoordinates[0][3] = SPACE;
		textCoordinates[0][4] = SPACE;
		textCoordinates[1][0] = resolution[4];
		textCoordinates[1][1] = resolution[5];
		textCoordinates[1][2] = resolution[6];  // Full resolution copied over at this point!
		textCoordinates[1][3] = SPACE;
		textCoordinates[1][4] = SPACE;
	}
	else
		if(resolution[3] != SPACE && resolution[4] == SPACE && !(resolution[8] >= '0' && resolution[8] <= '9'))	// resolution is of the form xxxx yyy
		{
			textCoordinates[0][0] = resolution[0];
			textCoordinates[0][1] = resolution[1];
			textCoordinates[0][2] = resolution[2];
			textCoordinates[0][3] = resolution[3];
			textCoordinates[0][4] = SPACE;
			textCoordinates[1][0] = resolution[5];
			textCoordinates[1][1] = resolution[6];
			textCoordinates[1][2] = resolution[7];
			textCoordinates[1][3] = SPACE;
			textCoordinates[1][4] = SPACE;
		}
		else	// resolution is of the form xxxx yyyy
			{
				textCoordinates[0][0] = resolution[0];
				textCoordinates[0][1] = resolution[1];
				textCoordinates[0][2] = resolution[2];
				textCoordinates[0][3] = resolution[3];
				textCoordinates[0][4] = SPACE;
				textCoordinates[1][0] = resolution[5];
				textCoordinates[1][1] = resolution[6];
				textCoordinates[1][2] = resolution[7];
				textCoordinates[1][3] = resolution[8];
				textCoordinates[1][4] = SPACE;
			}

	//	Convert to ints
	resWidth = atoi(textCoordinates[0]);
	resHeight = atoi(textCoordinates[1]);

	if(!WriteResFile(resWidth, resHeight))
		MessageBox(NULL, "Unable to write resolution to file! Continue to play.", "Resolution file update...", MB_ICONEXCLAMATION | MB_OK);

	if(!CheckCurrentResolution(resWidth, resHeight))
		ChangeScreenResolution(resWidth, resHeight);
}

bool FetchResolution()
{
	char buffer[11];
	size_t stringLength = 0;
	FILE *resFile = NULL;

	if((resFile = fopen("csr.a2g", "r")) == NULL)
		return false;

	while(!feof(resFile))
	{
		fread(buffer, sizeof(char), sizeof(buffer), resFile);
		if(ferror(resFile))
		{ 
			MessageBox(NULL, "Can't read resolution file! Refreshing.", "Resolution Read...", MB_ICONEXCLAMATION | MB_OK);
			return false;
		}
	}

	fclose(resFile);

	stringLength = sizeof(buffer);

	FetchAndConvertStringToInt(buffer, stringLength);

	return true;
}