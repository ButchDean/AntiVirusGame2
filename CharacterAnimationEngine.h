/*!\file CharacterAnimationEngine.h
 * \brief Function prototypes.
 * \author Dean N. Butcher
 * \version 2.0
 * \date August 2004
 */

// This part holds the prototypes for the game characters only.

#ifndef _CHARACTER_CONSTRUCTION_ROUTINES_
#define _CHARACTER_CONSTRUCTION_ROUTINES_

#include "DefinedStructures.h"
#include "GraphicsIncludes.h"

GLvoid RenderACharacterOnSceneWithoutPowerUp(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLUquadricObj *const Sphere[], unsigned int bindedTexture,
											 bool up, bool down, bool left, bool right, const GLfloat chainColour[],
											 TranslationCoords (*rsPosition)(TranslationCoords spA, bool up, bool down, bool left, bool right),
											 float alpha, int chainLength);

GLvoid RenderACharacterOnSceneWithPowerUp(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLUquadricObj *const Sphere[], unsigned int bindedTexture, unsigned int innerBindedTexture, const float innerSize, const GLfloat alphaVal,
											bool up, bool down, bool left, bool right, const GLfloat chainColour[],
											TranslationCoords (*position)(TranslationCoords spA, bool up, bool down, bool left, bool right),
											int chainLength);

void ConstructGameChainMotion(const int playerChainLength);

#endif