/*!\file LoaTextures.cpp
 * \brief Texture management is implemented here.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

// Containes texture manipulation functions.

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

// Texture information
/*! Bitmap information header for land texture. */
BITMAPINFOHEADER landInfo;
/*! Bitmap information header for water texture. */
BITMAPINFOHEADER waterInfo;
/*! Bitmap information header for background texture. */
BITMAPINFOHEADER backdropInfo;
/*! Bitmap information header for additional texture info. */
BITMAPINFOHEADER textureInfo[2];

/*! Bitmap image data. */
unsigned char *imageData	 = NULL;
/*! Bitmap image data for the environment (in this case the grass). */
unsigned char *landTexture   = NULL;
/*! Bitmap image data for the environment (in this case the water). */
unsigned char *waterTexture  = NULL;
/*! Bitmap image data for the background. */
unsigned char *bdTexture	 = NULL;
/*! Bitmap image data for an individual sphere. */
unsigned char *sphereTexture = NULL;
/*! Bitmap image data for a powered-up sphere. */
unsigned char *quadSphereTexture = NULL;
/*! Handle to land texture. */
unsigned int  land;
/*! Handle to water texture. */
unsigned int  water;
/*! Handle to background texture. */
unsigned int  backdropTexture;
/*! Handle to sphere texture. */
unsigned int  textureGeneration;
/*! Handle to powered-up sphere texture. */
unsigned int  quadTextureGeneration;

// powerUpImageData and powerUpBitmapInfoHeader Exclusive to Post Apocalyptic Garden of Eden.
/*! Total number of effects: four power-ups plus the standard chain. */
const int NUMEFFECTS = 5;
/*! Used to bind the texture for each of the power-up plus standard for Post Apocalyptic Garden of Eden. */
unsigned int powerUpTextureGeneration[NUMEFFECTS];
/*! Used to hold the texture image data for each of the power-up plus standard for Post Apocalyptic Garden of Eden. */
unsigned char *powerUpImageData[NUMEFFECTS];
/*! Used to hold the pointer to the texture for each of the power-up plus standard for Post Apocalyptic Garden of Eden. */
unsigned char *powerUpTexture[NUMEFFECTS];
/*! The bitmap info header for each texture of each power-up. */
BITMAPINFOHEADER powerUpBitmapInfoHeader[NUMEFFECTS];
/*! The bitmap info for each texture of each power-up. */
BITMAPINFOHEADER powerUpBitmapInfo[NUMEFFECTS];

bool LoadTextures()
// Loads textures for rendering. 
{
	// Land mipmap
	glGenTextures(1, &land);
	glBindTexture(GL_TEXTURE_2D, land);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, landInfo.biWidth, landInfo.biHeight, 
					  GL_RGB, GL_UNSIGNED_BYTE, landTexture);

	// Water mipmap
	glGenTextures(1, &water);
	glBindTexture(GL_TEXTURE_2D, water);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, waterInfo.biWidth, waterInfo.biHeight, 
					  GL_RGB, GL_UNSIGNED_BYTE, waterTexture);

	return true;
}

GLvoid BindTexturesAndSetMipmaps()
// Sets up mipmaps.
{
	glGenTextures(1, &textureGeneration);
	glBindTexture(GL_TEXTURE_2D, textureGeneration);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, textureInfo[0].biWidth, textureInfo[0].biHeight, 
					  GL_RGB, GL_UNSIGNED_BYTE, sphereTexture);
}

GLvoid BindTexturesAndSetMipmapsForPowerUp()
// Sets up mipmaps for spheres.
{
	glGenTextures(1, &quadTextureGeneration);
	glBindTexture(GL_TEXTURE_2D, quadTextureGeneration);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, textureInfo[1].biWidth, textureInfo[1].biHeight, 
					  GL_RGB, GL_UNSIGNED_BYTE, quadSphereTexture);
}

GLvoid BindTexturesAndSetMipmapsForPowerUpsInEnvironments()
// Sets up mipmaps for power ups and other objects that populate the environments.
{
	for(int i = 0; i < NUMEFFECTS; i++)
	{
		glGenTextures(1, &powerUpTextureGeneration[i]);
		glBindTexture(GL_TEXTURE_2D,  powerUpTextureGeneration[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, powerUpBitmapInfo[i].biWidth, powerUpBitmapInfo[i].biHeight,
						  GL_RGB, GL_UNSIGNED_BYTE, powerUpTexture[i]);
	}
}

GLvoid LoadBackdropTexture()
// Sets up mipmap for sky backdrop.
{
	glGenTextures(1, &backdropTexture);
	glBindTexture(GL_TEXTURE_2D, backdropTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, backdropInfo.biWidth, backdropInfo.biHeight, 
					  GL_RGB, GL_UNSIGNED_BYTE, bdTexture);
}

void CleanUpLoadTextures()
//	Only delete from OpenGL free store if used.
{
	if(glIsTexture(land) == GL_TRUE)
	{
		glDeleteTextures(1, (GLuint *) landTexture);
		landTexture = NULL;
	}
	if(glIsTexture(backdropTexture) == GL_TRUE)
	{
		glDeleteTextures(1, (GLuint *) bdTexture);
		bdTexture = NULL;
	}
	if(glIsTexture(textureGeneration) == GL_TRUE)
	{
		glDeleteTextures(1, (GLuint *) sphereTexture);
		sphereTexture = NULL;
	}
	if(glIsTexture(quadTextureGeneration) == GL_TRUE)
	{
		glDeleteTextures(1, (GLuint *) quadSphereTexture);
		quadSphereTexture = NULL;
	} 
}
