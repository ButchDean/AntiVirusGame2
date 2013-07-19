/*!\file GameAI.h
 * \brief Function prototypes.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

// This is the AI engine used for motion, collision detection and game object interaction.

#ifndef _AI_ROUTINES_
#define _AI_ROUTINES_

#include "DefinedStructures.h"
#include "PowerUpDefinitions.h"

const float COLLISION_SENSITIVITY = 110.0f;				// The greater this value, the more 'forgiving' the collision detection is.
const float SPHERE_RADIUS	= 2.4f;
const float SPHERE_DIAMETER = 2.0f * SPHERE_RADIUS;		// d = 2r.

bool EstablishCollision(TranslationCoords SphereA, TranslationCoords SphereB, float TWO_RADIUS);
TranslationCoords ComputeNextSpherePosition(TranslationCoords alphaSphere, bool longUp, bool longDown, bool latL, bool latR);
void AttractGameObjects(TranslationCoords &alphaObject, TranslationCoords &betaObject);
void RepelGameObjects(TranslationCoords &alphaObject, TranslationCoords &betaObject, float leftLimit, float rightLimit, float farendLimit, float nearendLimit);
void PrioritisationEngine(TranslationCoords *alphaObject, TranslationCoords *betaObject, TranslationCoords *gammaObject,
						  int alphaLength, int betaLength, int gammaLength,
						  POWERUP_TYPE alphaType, POWERUP_TYPE betaType, POWERUP_TYPE gammaType, void (*AttractObj)(TranslationCoords &alphaObject, TranslationCoords &betaObject),
						  void (*RepelObj)(TranslationCoords &alphaObject, TranslationCoords &betaObject, float leftLimit, float rightLimit, float farendLimit, float nearendLimit),
						  float leftLimit, float rightLimit, float farendLimit, float nearendLimit,
						  TranslationCoords *SupBlastCoord1, TranslationCoords *RebirthCoord1, TranslationCoords *InvisiCoord,
						  TranslationCoords *SupBlastCoord2, TranslationCoords *RebirthCoord2, const bool SupBlast1, const bool Rebirth1,
						  const bool Invisi, const bool SupBlast2, const bool Rebirth2);
void ApplyGameRules(TranslationCoords alphaObject, TranslationCoords betaObject, TranslationCoords gammaObject, int &alphaLength, int &betaLength, int &gammaLength,
					POWERUP_TYPE alphaType, POWERUP_TYPE betaType, POWERUP_TYPE gammaType, int &alphaScore, int &betaScore, int &gammaScore);
void SetChainExtended(bool &chainA, bool &chainB, bool &chainC);

#endif