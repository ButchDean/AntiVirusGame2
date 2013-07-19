/*!\file GameAI.cpp
 * \brief Function to handle NPC behaviour.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

#include <windows.h>
#include <math.h>
#include "GameAI.h"
#include "FramerateSync.h"

/*! The rate at which chains gravitate toward each other or power-ups. */
const float GRAVITATION_MOVE_RATE = 10.0f;
/*! The maximum chain length. */
const int SPHERENUMS = 7;
/*! Has a collision occurred? */
bool gbCollision;
/*! Sphere coordinate references to keep them chained. */
TranslationCoords SphereObjectCoords[SPHERENUMS];

/*! Ensure that chain is not extended more than once for function ApplyGameRules(). */
bool chainExtended[] = {false, false, false};

bool EstablishCollision(TranslationCoords SphereA, TranslationCoords SphereB, float TWO_RADIUS)
// Enter Pythagoras to determine if a collision has taken place! Essentially a modification of 'Bounding Sphere'.
{
	static bool bAlreadyNotified = false;
	float		x_squared, y_squared, z_squared, hypotenuse_xy, hypotenuse_yz, hypotenuse_xz;

	// Compute the squares of the differences between the spheres on each of the three axes.
	x_squared = powf(SphereA.xcoord - SphereB.xcoord, 2.0f);
	y_squared = powf(SphereA.ycoord - SphereB.ycoord, 2.0f);
	z_squared = powf(SphereA.zcoord - SphereB.zcoord, 2.0f);

	// Compute hypotenuse for each of the three planes in 3D space.
	hypotenuse_xy = sqrtf(x_squared + y_squared);
	hypotenuse_yz = sqrtf(y_squared + z_squared);
	hypotenuse_xz = sqrtf(x_squared + z_squared);

	if((hypotenuse_xy <= TWO_RADIUS) && (hypotenuse_yz <= TWO_RADIUS) && (hypotenuse_xz <= TWO_RADIUS))
	{
		if(!bAlreadyNotified)		// Notify on first instance of current collision.
			return bAlreadyNotified = gbCollision = true;

		return false;	// Once notified, return false even though collision has been detected.
	}
	else
		return bAlreadyNotified = gbCollision = false;	// No collision occured.
}

TranslationCoords ComputeNextSpherePosition(TranslationCoords alphaSphere, bool longUp, bool longDown, bool latL, bool latR)
// Leading sphere is tracked according to keypress, and following sphere will line-up in that direction.
{
	const float MOVE_INLINE_CONSTANT = 10.0f;
	const float SPHERE_CIRCUMFERENCE = 60.0f;
	static TranslationCoords betaSphere;

	if(longUp)
	{
		betaSphere.xcoord = alphaSphere.xcoord;
		betaSphere.ycoord = alphaSphere.ycoord;
		betaSphere.zcoord = alphaSphere.zcoord - SPHERE_CIRCUMFERENCE;
	}
	else if(longDown)
		 {
			betaSphere.xcoord = alphaSphere.xcoord;
			betaSphere.ycoord = alphaSphere.ycoord;
			betaSphere.zcoord = alphaSphere.zcoord + SPHERE_CIRCUMFERENCE;
		 }

	if(latR)
	{
		betaSphere.xcoord = alphaSphere.xcoord - SPHERE_CIRCUMFERENCE;
		betaSphere.ycoord = alphaSphere.ycoord;
		betaSphere.zcoord = alphaSphere.zcoord;
	}
 	else if(latL)
		{
			betaSphere.xcoord = alphaSphere.xcoord + SPHERE_CIRCUMFERENCE;
			betaSphere.ycoord = alphaSphere.ycoord;
			betaSphere.zcoord = alphaSphere.zcoord;
		}

	return betaSphere;
}

void AttractGameObjects(TranslationCoords &alphaObject, TranslationCoords &betaObject)
// Depending on the outcome of priorities, this function brings objects together.
{
	const float FAST_GRAVITATION_MOVE_RATE = 2.0f * GRAVITATION_MOVE_RATE;

	alphaObject.ycoord = 170.0f; betaObject.ycoord = 170.0f;	// Used to refine collision detection.

	if(alphaObject.xcoord <= (betaObject.xcoord - SPHERE_RADIUS))
		betaObject.xcoord -= FAST_GRAVITATION_MOVE_RATE;
	else if(alphaObject.xcoord >= (betaObject.xcoord + SPHERE_RADIUS))
			betaObject.xcoord += FAST_GRAVITATION_MOVE_RATE;
			

	if(alphaObject.ycoord <= (betaObject.ycoord - SPHERE_RADIUS))
		betaObject.ycoord -= FAST_GRAVITATION_MOVE_RATE;
	else if(alphaObject.ycoord >= (betaObject.ycoord + SPHERE_RADIUS))
			betaObject.ycoord += FAST_GRAVITATION_MOVE_RATE;
	
	if(alphaObject.zcoord <= (betaObject.zcoord - SPHERE_RADIUS))
		betaObject.zcoord -= FAST_GRAVITATION_MOVE_RATE;
	else if(alphaObject.zcoord >= (betaObject.zcoord + SPHERE_RADIUS))
			betaObject.zcoord += FAST_GRAVITATION_MOVE_RATE;
}

void RepelGameObjects(TranslationCoords &alphaObject, TranslationCoords &betaObject, float leftLimit, float rightLimit, float farendLimit, float nearendLimit)
// Depending on the outcome of priorities, this function progressively sets object further apart.
{
	const float FAST_GRAVITATION_MOVE_RATE = 1.8f * GRAVITATION_MOVE_RATE;
	const float TOO_CLOSE_OUTER = 35.0f * SPHERE_DIAMETER;
	const float TOO_CLOSE_INNER = 0.0f * SPHERE_DIAMETER;

	// Check against boundaries of environment to ensure object don't fly off the edge!
	if(betaObject.xcoord <= leftLimit)
		betaObject.xcoord += FAST_GRAVITATION_MOVE_RATE;
	if(betaObject.xcoord >= rightLimit)
		betaObject.xcoord -= FAST_GRAVITATION_MOVE_RATE;

	if(betaObject.zcoord <= nearendLimit)
		betaObject.zcoord += FAST_GRAVITATION_MOVE_RATE;
	if(betaObject.zcoord >= farendLimit)
		betaObject.zcoord -= FAST_GRAVITATION_MOVE_RATE;

	// Ensure objects are sufficiently spaced apart.
	// On the x-axis.
	if(alphaObject.xcoord > betaObject.xcoord)
		if(betaObject.xcoord > (alphaObject.xcoord - TOO_CLOSE_OUTER) && betaObject.xcoord < (alphaObject.xcoord - TOO_CLOSE_INNER))
			betaObject.xcoord -= FAST_GRAVITATION_MOVE_RATE;

	if(alphaObject.xcoord < betaObject.xcoord)
		if(betaObject.xcoord < (alphaObject.xcoord + TOO_CLOSE_OUTER) && betaObject.xcoord > (alphaObject.xcoord + TOO_CLOSE_INNER))
			betaObject.xcoord += FAST_GRAVITATION_MOVE_RATE;
}

void PrioritisationEngine(TranslationCoords *alphaObject, TranslationCoords *betaObject, TranslationCoords *gammaObject,
						  int alphaLength, int betaLength, int gammaLength,
						  POWERUP_TYPE alphaType, POWERUP_TYPE betaType, POWERUP_TYPE gammaType, void (*AttractObj)(TranslationCoords &alphaObject, TranslationCoords &betaObject),
						  void (*RepelObj)(TranslationCoords &alphaObject, TranslationCoords &betaObject, float leftLimit, float rightLimit, float farendLimit, float nearendLimit),
						  float leftLimit, float rightLimit, float farendLimit, float nearendLimit,
						  TranslationCoords *SupBlastCoord1, TranslationCoords *RebirthCoord1, TranslationCoords *InvisiCoord,
						  TranslationCoords *SupBlastCoord2, TranslationCoords *RebirthCoord2, const bool SupBlast1, const bool Rebirth1,
						  const bool Invisi, const bool SupBlast2, const bool Rebirth2)
// Decides the actions of NPC chains: alpha = player, beta = first AI, gamma = second AI. This function is not specific to any environment.
// The invisibility power up is ignored by engine so that AI cannot "see" you.
{
	// If all characters exist in environment.
	if(betaObject)
	{
		if((betaLength > alphaLength) && alphaType != Invisibility)
			(*AttractObj)(*alphaObject, *betaObject);
		else 
			if(betaType == NoPowerUp || betaType == Rebirth || betaType == Invisibility)
			{
				if(!Rebirth2)
				(*AttractObj)(*RebirthCoord2, *betaObject);
				else
					if(!Rebirth1)
						(*AttractObj)(*RebirthCoord1, *betaObject);
					else
						if(!Invisi)
							(*AttractObj)(*InvisiCoord, *betaObject);
						else
							if(!SupBlast2)	
								(*AttractObj)(*SupBlastCoord2, *betaObject);
							else
								if(!SupBlast1)
									(*AttractObj)(*SupBlastCoord1, *betaObject);
			}

		if((betaType == SupremeBlast || betaType == SupaFly) && alphaType != Invisibility)
			(*AttractObj)(*alphaObject, *betaObject);

		if(alphaType == Invisibility)
		{
			if(!Rebirth2)
				(*AttractObj)(*RebirthCoord2, *betaObject);
			else
				if(!Rebirth1)
					(*AttractObj)(*RebirthCoord1, *betaObject);
				else
					if(!Invisi)
						(*AttractObj)(*InvisiCoord, *betaObject);
					else
						if(!SupBlast2)	
							(*AttractObj)(*SupBlastCoord2, *betaObject);
						else
							if(!SupBlast1)
								(*AttractObj)(*SupBlastCoord1, *betaObject);
		}
	}

	if(gammaObject)
	{
		if((gammaLength > alphaLength) && alphaType != Invisibility)
			(*AttractObj)(*alphaObject, *gammaObject);
		else 
			if(gammaType == NoPowerUp || gammaType == Rebirth || gammaType == Invisibility)
			{
				if(!SupBlast1)	
					(*AttractObj)(*SupBlastCoord1, *gammaObject);
				else
					if(!SupBlast2)
						(*AttractObj)(*SupBlastCoord2, *gammaObject);
					else
						if(!Invisi)
							(*AttractObj)(*InvisiCoord, *gammaObject);
						else
							if(!Rebirth1)
								(*AttractObj)(*RebirthCoord1, *gammaObject);
							else
								if(!Rebirth2)
									(*AttractObj)(*RebirthCoord2, *gammaObject);
			}

		if((gammaType == SupremeBlast || gammaType == SupaFly) && alphaType != Invisibility)
			(*AttractObj)(*alphaObject, *gammaObject);

		if(alphaType == Invisibility)
		{
			if(!SupBlast1)	
				(*AttractObj)(*SupBlastCoord1, *gammaObject);
			else
				if(!SupBlast2)
					(*AttractObj)(*SupBlastCoord2, *gammaObject);
				else
					if(!Invisi)
						(*AttractObj)(*InvisiCoord, *gammaObject);
					else
						if(!Rebirth1)
							(*AttractObj)(*RebirthCoord1, *gammaObject);
						else
							if(!Rebirth2)
								(*AttractObj)(*RebirthCoord2, *gammaObject);
		}
	}

	if(alphaType == SupremeBlast || alphaType == SupaFly)
	{
		if(gammaObject)
			(*RepelObj)(*alphaObject, *gammaObject, leftLimit, rightLimit, farendLimit, nearendLimit);
		if(betaObject)
			(*RepelObj)(*alphaObject, *betaObject, leftLimit, rightLimit, farendLimit, nearendLimit);
	}

	if(gammaObject && betaObject)
	{
		(*RepelObj)(*betaObject, *gammaObject, leftLimit, rightLimit, farendLimit, nearendLimit);
		(*RepelObj)(*gammaObject, *betaObject, leftLimit, rightLimit, farendLimit, nearendLimit);
	}

}

void ApplyGameRules(TranslationCoords alphaObject, TranslationCoords betaObject, TranslationCoords gammaObject, int &alphaLength, int &betaLength, int &gammaLength,
					POWERUP_TYPE alphaType, POWERUP_TYPE betaType, POWERUP_TYPE gammaType, int &alphaScore, int &betaScore, int &gammaScore)
//	Applies the game rules on establishing a collision between two objects.
{
	if(EstablishCollision(alphaObject, betaObject, COLLISION_SENSITIVITY) && !chainExtended[0])
	{
		if(alphaType == NoPowerUp || alphaType == Rebirth || alphaType == Invisibility)
		{
			if(betaType == SupremeBlast || betaType == SupaFly)
			{
				betaLength += alphaLength;
				alphaLength = 2;
				SetChainExtended(chainExtended[0], chainExtended[1], chainExtended[2]);
			}
		}

		if(alphaType == SupremeBlast)
		{
			if(betaType == NoPowerUp || betaType == Rebirth || betaType == Invisibility)
			{
				alphaLength += betaLength;
				betaLength = 2;
				SetChainExtended(chainExtended[0], chainExtended[1], chainExtended[2]);
			}
		}

		if(alphaType == SupaFly)
		{
			if(betaType == NoPowerUp || betaType == Rebirth || betaType == Invisibility || betaType == SupremeBlast)
			{
				alphaLength += betaLength;
				betaLength = 2;
				SetChainExtended(chainExtended[0], chainExtended[1], chainExtended[2]);
			}

			if(betaType == SupaFly)
			{
				alphaLength = 2;
				betaLength = 2;
				SetChainExtended(chainExtended[0], chainExtended[1], chainExtended[2]);
			}
		}

	}

	if(EstablishCollision(alphaObject, gammaObject, COLLISION_SENSITIVITY) && !chainExtended[1])
	{
		if(alphaType == NoPowerUp || alphaType == Rebirth || alphaType == Invisibility)
		{
			if(gammaType == SupremeBlast || gammaType == SupaFly)
			{
				gammaLength += alphaLength;
				alphaLength = 2;
				SetChainExtended(chainExtended[1], chainExtended[3], chainExtended[2]);
			}
		}

		if(alphaType == SupremeBlast)
		{
			if(gammaType == NoPowerUp || gammaType == Rebirth || gammaType == Invisibility)
			{
				alphaLength += gammaLength;
				gammaLength = 2;
				SetChainExtended(chainExtended[1], chainExtended[3], chainExtended[2]);
			}
		}

		if(alphaType == SupaFly)
		{
			if(gammaType == NoPowerUp || gammaType == Rebirth || gammaType == Invisibility || gammaType == SupremeBlast)
			{
				alphaLength += gammaLength;
				gammaLength = 2;
				SetChainExtended(chainExtended[1], chainExtended[3], chainExtended[2]);
			}

			if(gammaType == SupaFly)
			{
				alphaLength = 2;
				gammaLength = 2;
				SetChainExtended(chainExtended[1], chainExtended[3], chainExtended[2]);
			}
		}		
	}

	if(EstablishCollision(betaObject, gammaObject, COLLISION_SENSITIVITY) && !chainExtended[2])
	{
		if(betaType == NoPowerUp || betaType == Rebirth || betaType == Invisibility)
		{
			if(gammaType == SupremeBlast || gammaType == SupaFly)
			{
				gammaLength += betaLength;
				betaLength = 2;
				SetChainExtended(chainExtended[2], chainExtended[1], chainExtended[3]);
			}
		}

		if(betaType == SupremeBlast)
		{
			if(gammaType == NoPowerUp || gammaType == Rebirth || gammaType == Invisibility)
			{
				betaLength += gammaLength;
				gammaLength = 2;
				SetChainExtended(chainExtended[2], chainExtended[1], chainExtended[3]);
			}
		}

		if(betaType == SupaFly)
		{
			if(gammaType == NoPowerUp || gammaType == Rebirth || gammaType == Invisibility || gammaType == SupremeBlast)
			{
				betaLength += gammaLength;
				gammaLength = 2;
				SetChainExtended(chainExtended[2], chainExtended[1], chainExtended[3]);
			}

			if(gammaType == SupaFly)
			{
				betaLength = 2;
				gammaLength = 2;
				SetChainExtended(chainExtended[2], chainExtended[1], chainExtended[3]);
			}
		}				
	}

	// Ensure that if Rebirth power up held then chain length is equal to the longest length...
	if(alphaType == Rebirth)
	{
		if(alphaLength < betaLength)
			alphaLength = betaLength;

		if(alphaLength < gammaLength)
			alphaLength = betaLength;
	}

	if(betaType == Rebirth)
	{
		if(betaLength < alphaLength)
			betaLength = alphaLength;

		if(betaLength < gammaLength)
			betaLength = gammaLength;
	}

	if(gammaType == Rebirth)
	{
		if(gammaLength < alphaLength)
			gammaLength = alphaLength;

		if(gammaLength < betaLength)
			gammaLength = betaLength;
	}
}

void SetChainExtended(bool &chainA, bool &chainB, bool &chainC)
// Support function for ApplyGameRules() to control chain length extentions.
// Prevents chains from extending too long or too short.
{
	chainA = true;

	if(chainB)
		chainB = false;

	if(chainC)
		chainC = false;
}
