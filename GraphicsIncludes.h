/*!\file GraphicsIncludes.h
 * \brief Function prototypes.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

// Graphics header files are put here for inclusion in source.

#ifndef _GRAPHICS_
#define _GRAPHICS_

#include "DefinedStructures.h"
#include "PowerUpDefinitions.h"

GLvoid InitialiseTransparents();
GLvoid InitialiseTerrain();
GLvoid InitialiseBackdrop();
GLvoid RenderSphere(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLUquadricObj *const Sphere, unsigned int *const bindedTexture, GLfloat alphaVal, GLvoid (*BackdropFunc)(GLfloat, GLfloat, GLfloat, GLfloat));
GLvoid RenderSphereWithPowerUp(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLUquadricObj *const Sphere, unsigned int *const bindedTexture, unsigned int *const innerBindedTexture, GLfloat innerSize, GLvoid (*BackdropFunc)(GLfloat, GLfloat, GLfloat, GLfloat));
GLvoid RenderPowerUp(void (*bounceFunc)(), GLfloat c4fa, GLfloat c4fb, GLfloat c4fc, GLUquadricObj *const Sphere, GLvoid (*BackdropFunc)(GLfloat, GLfloat, GLfloat, GLfloat));
GLvoid RenderPlayerSphere(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ, GLUquadricObj *const Sphere, unsigned int bindedTexture, const GLfloat colour[], GLfloat alphaVal);
GLvoid RenderPlayerSphereWithPowerUp(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ, GLUquadricObj *const Sphere, unsigned int bindedTexture,  unsigned int innerBindedTexture, const GLfloat colour[], GLfloat innerSize, GLfloat alphaVal);
GLvoid RenderPlayerSphereMarker(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ, GLUquadricObj *const Sphere, const GLfloat colour[]);
GLvoid RenderPostApocalypticGardenOfEden(unsigned int landtxtr, unsigned int watertxtr, void (*bounceFunc)(), GLUquadricObj *marker,
										 GLUquadricObj *p1[], GLUquadricObj *p2[], GLUquadricObj *p3[],
										 TranslationCoords playerCoords, bool up, bool down, bool left, bool right,
										TranslationCoords (*position)(TranslationCoords spA, bool up, bool down, bool left, bool right),
										float alpha);
void CharacterRenderSupportFunction(POWERUP_TYPE plPowerUpHeld, TranslationCoords &plCoords, GLUquadricObj *plChain[], bool up, bool down, bool left, bool right,
									TranslationCoords (*position)(TranslationCoords spA, bool up, bool down, bool left, bool right), float pAlpha, int plChainLength, bool isHuman);
GLvoid FixBackdropTexture();
GLvoid RenderBackdrop(GLfloat xProj, GLfloat yProj, GLfloat zProj, GLfloat scaleFactor);
GLvoid DisableSphereOGLFunctions();
GLvoid DisableTransparentsOGLFunctions();
void Bouncer();
void ConstructModelChainMotion();
void SetSphereSpotlight();
void SetEnvironmentSpotlight();

#endif
