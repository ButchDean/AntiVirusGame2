/*!\file loadBitmaps.cpp
 * \brief Loads bitmaps.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */


#include <windows.h>
#include <stdio.h>

/*! Bitmap tpye ID. */
const DWORD BITMAP_ID =	0x4D42;

/*! Bitmap information header. */
BITMAPINFOHEADER	bitmapInfoHeader;
/*! Pointer to bitmap file to be manipulated. */
FILE				*pFile = NULL;
/*! Bitmap file header. */
BITMAPFILEHEADER	bitmapFileHeader;
/*! Pointer to a bitmap image loaded from file. */
unsigned char		*bitmapImage = NULL;
/*! Temporary bitmap storgae location. */
unsigned char       tempRGB;

unsigned char *LoadBitmapFile(char *bmpFile, BITMAPINFOHEADER *bitmapInfoHeader)
// Loads a bitmap into memory.
{


	if((pFile = fopen(bmpFile, "rb")) == NULL)
		return NULL;

	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);

	if(bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(pFile);
		return NULL;
	}

	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

	fseek(pFile, bitmapFileHeader.bfOffBits, SEEK_SET);
	
	bitmapImage = (unsigned char*) malloc(bitmapInfoHeader->biSizeImage);

	if(!bitmapImage)
	{
		free(bitmapImage);
		fclose(pFile);
		return NULL;
	}

	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, pFile);

	if(bitmapImage == NULL)
	{
		fclose(pFile);
		return NULL;
	}

	for(unsigned int imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	fclose(pFile);
	return bitmapImage;
}

void CleanUpLoadBitmap()
// Cleans up memory used by file pointer.
{
	if(pFile)
		fclose(pFile);

	if(bitmapImage)
		free(bitmapImage);
}

