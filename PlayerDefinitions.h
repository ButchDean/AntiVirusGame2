/*!\file PlayerDefinitions.h
 * \brief Player and NPC character datatypes.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

#ifndef _PLAYER_DEFINITIONS_
#define _PLAYER_DEFINITIONS_

// The actual player chains that will populate the environments.
const int MAXPLAYERCHAINLENGTH = 10;

GLUquadricObj *player1Chain[MAXPLAYERCHAINLENGTH];
GLUquadricObj *player2Chain[MAXPLAYERCHAINLENGTH];
GLUquadricObj *player3Chain[MAXPLAYERCHAINLENGTH];
GLUquadricObj *markerSphere;	// Used to mark the player's chain.

#endif