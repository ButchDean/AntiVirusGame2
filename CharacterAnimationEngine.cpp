/*!\file CharacterAnimationEngine.cpp
 * \brief This file manages the construction of the player character and two AI opponents.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

// This file holds the definitions for the ccharacter chains of the game.

#include "StandardIncludes.h"
#include "CharacterAnimationEngine.h"
#include "GraphicsIncludes.h"
#include "GameAI.h"
#include "FramerateSync.h"

extern float chainBounce[];

/*! Maximum chain length for player/NPC without power up. */
const int MAXCHAINLENTGH = 7;
/*! The greater this value, the more pronounced is the wave motion of characters and power-ups. */
const float   JIGGLE_AMP = 15.0f;

/*! Used to invoke algorithm to line up spheres in a chain when 'up' is pressed. */
bool  bLineUpOnLongitudeUp;
/*! Used to invoke algorithm to line up spheres in a chain when 'down' is pressed. */
bool  bLineUpOnLongitudeDown;
/*! Used to invoke algorithm to line up spheres in a chain when 'right' is pressed. */
bool  bLineUpOnLatitudeR;
/*! Used to invoke algorithm to line up spheres in a chain when 'left' is pressed. */
bool  bLineUpOnLatitudeL;

GLvoid RenderACharacterOnSceneWithoutPowerUp(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLUquadricObj *const Sphere[], unsigned int bindedTexture,
											 bool up, bool down, bool left, bool right, const GLfloat chainColour[],
											 TranslationCoords (*rsPosition)(TranslationCoords spA, bool up, bool down, bool left, bool right),
											 float alpha, int chainLength)
// Renders both player and AI characters on selected environment when power-up is acquired.
{
	TranslationCoords rsThisSphereCoords, rsNextSphereCoords;
	int sphereCounter;	// Counter to index each ball in the chain (do not process leading sphere).

	// Ensure chain does not exceed permitted length...
	if(chainLength > MAXCHAINLENTGH)
		chainLength = MAXCHAINLENTGH;

	rsThisSphereCoords.xcoord = axisX; rsThisSphereCoords.zcoord = axisZ;

	// Render initial sphere chain in game.
	rsThisSphereCoords.ycoord = 170.0f + JIGGLE_AMP * chainBounce[0];
	ConstructGameChainMotion(chainLength);
	RenderPlayerSphere(rsThisSphereCoords.xcoord, rsThisSphereCoords.ycoord, rsThisSphereCoords.zcoord, 50.0f, 50.0f, -50.0f, Sphere[0], bindedTexture, chainColour, alpha);	

	rsNextSphereCoords.xcoord = rsThisSphereCoords.xcoord;
	rsNextSphereCoords.ycoord = rsThisSphereCoords.ycoord;
	rsNextSphereCoords.zcoord = rsThisSphereCoords.zcoord;

	for(sphereCounter = 1; sphereCounter < chainLength; sphereCounter++)
	{
		// Initialise remaining sphere positions.
		rsThisSphereCoords.xcoord = rsNextSphereCoords.xcoord;
		rsThisSphereCoords.ycoord = rsNextSphereCoords.ycoord;
		rsThisSphereCoords.zcoord = rsNextSphereCoords.zcoord;

		// Work out coordinates... (for each link, if applicable)
		if(rsPosition != NULL)	// Only call these functions if it is summoned!
			rsNextSphereCoords = (*rsPosition)(rsThisSphereCoords, up, down, left, right);

		rsNextSphereCoords.ycoord = 170.0f + JIGGLE_AMP * chainBounce[sphereCounter];

		RenderPlayerSphere(rsNextSphereCoords.xcoord, rsNextSphereCoords.ycoord, rsNextSphereCoords.zcoord, 50.0f, 50.0f, -50.0f, Sphere[sphereCounter], bindedTexture, chainColour, alpha);
		ConstructGameChainMotion(chainLength);
	}
	
}

GLvoid RenderACharacterOnSceneWithPowerUp(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLUquadricObj *const Sphere[], unsigned int bindedTexture, unsigned int innerBindedTexture, const float innerSize, const GLfloat alphaVal,
											bool up, bool down, bool left, bool right, const GLfloat chainColour[],
											TranslationCoords (*rspPosition)(TranslationCoords spA, bool up, bool down, bool left, bool right),
											int chainLength)
// Renders both player and AI characters on selected environment when power-up is acquired.
{
//	static bool coordsAllocated = false;		// Ensure space is allocated for coordinates.
	TranslationCoords rspThisSphereCoords, rspNextSphereCoords;
	int sphereCounter;	// Counter to index each ball in the chain (do not process leading sphere).

	// Ensure chain does not exceed permitted length...
	if(chainLength > MAXCHAINLENTGH)
		chainLength = MAXCHAINLENTGH;

	rspThisSphereCoords.xcoord = axisX; rspThisSphereCoords.zcoord = axisZ;

	// Render initial sphere chain in game.
	rspThisSphereCoords.ycoord = 170.0f + JIGGLE_AMP * chainBounce[0];
	ConstructGameChainMotion(chainLength);
	RenderPlayerSphereWithPowerUp(rspThisSphereCoords.xcoord, rspThisSphereCoords.ycoord, rspThisSphereCoords.zcoord, 50.0f, 50.0f, -50.0f, Sphere[0], bindedTexture,  innerBindedTexture, chainColour, innerSize, alphaVal);

	rspNextSphereCoords.xcoord = rspThisSphereCoords.xcoord;
	rspNextSphereCoords.ycoord = rspThisSphereCoords.ycoord;
	rspNextSphereCoords.zcoord = rspThisSphereCoords.zcoord;

	for(sphereCounter = 1; sphereCounter < chainLength; sphereCounter++)
	{
		// Initialise remaining sphere positions.
		rspThisSphereCoords.xcoord = rspNextSphereCoords.xcoord;
		rspThisSphereCoords.ycoord = rspNextSphereCoords.ycoord;
		rspThisSphereCoords.zcoord = rspNextSphereCoords.zcoord;

		// Work out coordinates... (for each link, if applicable)
		if(rspPosition != NULL)	// Only call these functions if it is summoned!
			rspNextSphereCoords = (*rspPosition)(rspThisSphereCoords, up, down, left, right);

		rspNextSphereCoords.ycoord = 170.0f + JIGGLE_AMP * chainBounce[sphereCounter];

		RenderPlayerSphereWithPowerUp(rspNextSphereCoords.xcoord, rspNextSphereCoords.ycoord, rspNextSphereCoords.zcoord, 50.0f, 50.0f, -50.0f, Sphere[sphereCounter], bindedTexture,  innerBindedTexture, chainColour, innerSize, alphaVal);
		ConstructGameChainMotion(chainLength);
	}
	
}

void ConstructGameChainMotion(const int playerChainLength)
// Ensures chain linkage is maintained.
{
	const float UPPERBOUND = 1.7f;
	const float LOWERBOUND = -1.2f;
	const float LINKDELAY = -0.75f;
	const float WAVE_FREQ = 0.05f;		// Controls the chain wave frequency.

	static bool boolVal = false;
	static float sineAngle = 0.0f;
	static int counter;

	// Create sine wave motion for player characters.
	sineAngle += WAVE_FREQ;
	if(sineAngle >= 360.0f)
		sineAngle = 0.0f;

	if(chainBounce[0] > UPPERBOUND)
		boolVal = false;
	if(chainBounce[0] < LOWERBOUND)
		boolVal = true; 

	if(!boolVal)
	{
		for(counter = 0; counter < playerChainLength; counter++)
		{
			(counter == 0) ? chainBounce[counter] = static_cast <float> (-sin(static_cast <double> (sineAngle))) :
							 chainBounce[counter] = static_cast <float> (-sin(static_cast <double> (sineAngle+static_cast <float> (counter)*LINKDELAY)));
		}
	}
	if(boolVal)
	{
		for(counter = 0; counter < playerChainLength; counter++)
		{
			(counter == 0) ? chainBounce[counter] = static_cast <float> (sin(static_cast <double> (sineAngle))) :
							 chainBounce[counter] = static_cast <float> (sin(static_cast <double> (sineAngle+static_cast <float> (counter)*LINKDELAY)));
		}
	}
	
}
