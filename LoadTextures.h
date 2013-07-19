/*!\file LoadTextures.h
 * \brief Function prototypes.
 * \author Dean N. Butcher
 * \version 2.0
 * \date August 2004
 */

// loadtxtr.h: Creates mipmaps for loaded textures.

#ifndef _LOADTEXTURES_
#define _LOADTEXTURES_

bool LoadTextures();
GLvoid BindTexturesAndSetMipmaps();
GLvoid BindTexturesAndSetMipmapsForPowerUp();
GLvoid BindTexturesAndSetMipmapsForPowerUpsInEnvironments();
GLvoid LoadBackdropTexture();
void CleanUpLoadTextures();

#endif