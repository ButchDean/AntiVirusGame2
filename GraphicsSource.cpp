/*!\file GraphicsSource.cpp
 * \brief The core of the graphic engine resides here.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

// Function definitions associated with GraphicsIncludes.h.

#include "StandardIncludes.h"
#include "LoadBitmaps.h"
#include "LoadTextures.h"
#include "CharacterAnimationEngine.h"	// "GraphicsIncludes.h" contained in here.
#include "GameAI.h"
#include "DirectSoundIncludes.h"
#include "Scoreboard.h"
#include "RandSeqGenerator.h"
#include "FramerateSync.h"


extern GLvoid RenderBackdrop(GLfloat xProj, GLfloat yProj, GLfloat zProj, GLfloat scaleFactor);
extern unsigned int powerUpTextureGeneration[5];
extern GLUquadricObj *powerUpSphere[4];
extern bool  bLineUpOnLongitudeUp, bLineUpOnLongitudeDown, bLineUpOnLatitudeR, bLineUpOnLatitudeL;
extern bool chainExtended[];

// For sound.
extern DWORD dwStatus;
extern const int NUMSOUNDS = 4;
extern LPDIRECTSOUND		lpDS;
extern LPDIRECTSOUNDBUFFER lpDSBSounds[NUMSOUNDS];

/*! String to show when bitmap loading fails. */
const char szBitmapLoadingError[] = "Bitmap loading error...";
/*! Used to cease animation. */
const float animationStop	= 343.0f;
/*! Used for Supreme Blast + Rebirth effect. */
const int	switchSphere	= 5;
/*! Width size of map on screen. */
const int MAP_X_SIZE		= 64;
/*! Depth size of map on screen. */
const int MAP_Z_SIZE		= 64;
/*! Scale factor of map on screen. */
const float MAP_SCALE		= 20.0f;
/*! The ratio of the circumfirance of a circle to its diameter. */
const float PI				= 3.1415f;
/*! Maximum chain length in spheres. */
const int CHAINMAXLENGTH	= 7;
/*! Used to maintain links in chain by causing a delay in following links for the character models. */
const float LINKDELAY		= -0.25f;
/*! For sphere resizing. */
const float SCALING			= 0.5f;
/*! Used to initialise RGB channels to this value. */
const float INITIALTRANSFORMATIONCOLOURS = 0.5f;
/*! Marks the moment that morphing takes place. */
const float TRANSDECREMENT = 0.001f;
/*! Used for morphing timing. */
const float ANIMATIONSTART = 1.1f;	

// Limits of movement across the environment.
/*! Lefthand limit that player and NPCs can travel in game. */
const float LEFT_LIMIT = 200.0f;
/*! Righthand limit that player and NPCs can travel in game. */
const float RIGHT_LIMIT = 1350.0f;
/*! Far end limit that player and NPCs can travel in game. */
const float FAREND_LIMIT = -2000.0f;
/*! Near end limit that player and NPCs can travel in game. */
const float NEAREND_LIMIT = -300.0f;

/*! Vertical motion guide. */
float bounce = 0.0f;
/*! Rotation angle. */
float angle = 0.0f;
/*! Used to start and stop animation. */
float accumulator;
/*! Used for Supreme Blast + Rebirth effect. */
int	  modExtraction, delay;
/*! Camera 'look' coordinates. */
float lookX, lookY, lookZ;
/*! Used for terraine texture mapping. */
float terrain[MAP_X_SIZE][MAP_Z_SIZE][3];
/*! Used to compute sine wave for character motions. */
float sineAngle = 0.0f;
/*! Retains current location on sine wave for each sphere in chain. */
float bounceChain[CHAINMAXLENGTH];
/*! Counter used for various purposes. */
int	  counter;
/*! Used to maintain character chain linkage. */
float animationCounter = ANIMATIONSTART;
/*! Retains current location on sine wave for each sphere in chain. */
float chainBounce[CHAINMAXLENGTH];
/*! Sets the red attribute of any colour configuration to zero. */
float colorR = 0;
/*! Sets the blue attribute of any colour configuration to zero. */
float colorB = 0;
/*! Used to transform the red attribute on a per frame basis. */
float factorR = INITIALTRANSFORMATIONCOLOURS;	
/*! Used to transform the green attribute on a per frame basis. */
float factorG = INITIALTRANSFORMATIONCOLOURS;
/*! Used to transform the blue attribute on a per frame basis. */
float factorB = INITIALTRANSFORMATIONCOLOURS;
/*! If true link moves upward, else downward. */
bool  boolVal;
/*! Notification that memory was allocated for chain. */
bool  chainUsed = false;								
/*! Counter index for x-axis for water wave. */
int xIndex;												
/*! Counter index for y-axis for water wave. */
int yIndex;												

// Colour saturation for power ups.
/*! Blue colour saturation for power up. */
GLfloat bleu[]	= {0.0f, 0.0f, 0.5f};					
/*! Red colour saturation for power up. */
GLfloat rouge[] = {0.5f, 0.0f, 0.0f};					
/*! Green colour saturation for power up. */
GLfloat verte[] = {0.0f, 0.5f, 0.0f};					
/*! White colour saturation for power up. */
GLfloat blanc[] = {0.5f, 0.5f, 0.5f};					
/*! Grey colour saturation for power up. */
GLfloat grey[]  = {0.07f, 0.07f, 0.07f};					

// lighting variables for sphere
/*! Ambient light setting. */
float ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};		
/*! Diffuse light setting. */
float diffuseLight[] = {1.0f, 1.0f, 1.0f, 1.0f};		

/*! Spotlight position behind viewer. */
float spotlightPosition[4];								
/*! Where the spotlight is pointing from behind the viewer. */
float spotlightDirection[3];							
/*! Spotlight position infront of viewer. */
float spotlightPositionBack[4];							
/*! Where the spotlight is pointing from infront the viewer. */
float spotlightDirectionBack[3];						

// material variables
/*! Ambient material properties. */
float matAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};			// ambient material
/*! Diffuse material properties. */
float matDiff[] = {1.0f, 1.0f, 1.0f, 1.0f};				// diffuse material
/*! Specular material properties. */
float matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};			// specular material
/*! Light model. */
float lightModel[] = {0.5f, 0.5f, 0.5f, 1.0f};

/*! The maximum players in the game: 1 x human + 2 x NPC. */
const int MAX_PLAYERS_IN_GAME = 3;

/*! Power ups held by repective players initialised to no power up. */
static POWERUP_TYPE powerUpTypeHeld[MAX_PLAYERS_IN_GAME] = {NoPowerUp, NoPowerUp, NoPowerUp};
/*! Current music playing to reflect power up held, initialised to no power up. */
POWERUP_TYPE musicCurrentlyPlaying = NoPowerUp;

// Chain lengths.
/*! Shortest chain length for player. */
int player1ChainLength = 2;
/*! Shortest chain length for NPC1. */
int player2ChainLength = 2;
/*! Shortest chain length for NPC2. */
int player3ChainLength = 2;

/*! Global handle to device context. */
HDC g_HDC;

extern BITMAPINFOHEADER backdropInfo;
extern unsigned char *imageData;
extern unsigned int  textureGeneration;
extern unsigned int  backdropTexture;
extern unsigned int  land;
extern unsigned char *bdTexture;

void Bouncer()
// Controls power up object vertical motion.
{
	static bool Up = false;
	float BOUNCE_CONST = 0.03f;

	if(bounce > 0.2f)
		Up = false;
	if(bounce < -0.2f)
		Up = true; 

	if(!Up) // if "Down"
		bounce -= BOUNCE_CONST;
	else // if"Up"
		bounce += BOUNCE_CONST;
}

GLvoid InitialiseTransparents()
// Set modelview for rendering tranparent objects (spheres and water).
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);			

	glEnable(GL_BLEND);								// Transparency blending.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_SMOOTH);						// smooth shading
	glEnable(GL_DEPTH_TEST);						// hidden surface removal
	glEnable(GL_CULL_FACE);							// inside of polys not calculated
	glFrontFace(GL_CCW);							// counterclockwise polygons are out

	glEnable(GL_LIGHTING);							// enable lighting
	glEnable(GL_AUTO_NORMAL);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);

	// setting up LIGHT0 for forground illumination and LIGHT1 for backdrop illumination.
	// ambient element
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);

	// diffuse element
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);

	// place light in world
	glLightfv(GL_LIGHT0, GL_POSITION, spotlightPosition);
	glLightfv(GL_LIGHT1, GL_POSITION, spotlightPositionBack);

	// spotlight properties
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 40.0f);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 50.0f);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 30.0f);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 50.0f);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotlightDirection);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotlightDirectionBack);

	// Enable the lights
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	// track the colors set by glColor() for ambient and diffuse light
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// materials have minimal shine
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, 128.0f);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightModel);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 0.5f);
}

GLvoid InitialiseTerrain()
// Set the geometry for the coordinates of the terrain for Post Apocalyptic Garden of Eden.
{
	if(imageData == NULL)
		MessageBox(NULL, "Bitmap Info Header not read.", "Allocation Error", MB_OK);

	for(int z = 0; z < MAP_Z_SIZE; z++)
		for(int x = 0; x < MAP_X_SIZE; x++)
		{
			terrain[x][z][0] = static_cast <float> (x) * MAP_SCALE;	// Width.
			terrain[x][z][1] = static_cast <float> (imageData[(z * MAP_Z_SIZE + x) * 3]); // Height of terrain.
			terrain[x][z][2] = -static_cast <float> (z*2) * MAP_SCALE; // Terrain depth.
		}
}

GLvoid InitialiseBackdrop()
// Enable OpenGL features for sky backdrop.
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);		// clear to white

	glShadeModel(GL_SMOOTH);					// use smooth shading
	glEnable(GL_DEPTH_TEST);					// hidden surface removal
	glEnable(GL_CULL_FACE);						// do not calculate inside of poly's
	glFrontFace(GL_CCW);						// counter clock-wise polygons are out

	glEnable(GL_TEXTURE_2D);					// enable 2D texturing
}

GLvoid RenderSphere(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLUquadricObj *const Sphere, unsigned int *const bindedTexture, GLfloat alphaVal, GLvoid (*BackdropFunc)(GLfloat, GLfloat, GLfloat, GLfloat))
// Rendering of an opaque sphere.
{
	if(BackdropFunc != NULL)
	{
		glBindTexture(GL_TEXTURE_2D, backdropTexture);
		(*BackdropFunc)(0.0f, 5.0f, -20.0f, 7.0f);
	}

	if(bindedTexture != NULL)
	{
		glPushMatrix();
			glTranslatef(axisX, axisY, axisZ);
				glPushMatrix();
					glBindTexture(GL_TEXTURE_2D, *bindedTexture);
					glRotatef(angle, 0.0f, 1.0f, 0.0f);
					glColor4f(0.5f+factorR, 0.5f+factorG, 0.5f+factorB, alphaVal);
					gluQuadricTexture(Sphere, GL_TRUE);
					gluSphere(Sphere, SCALING, 30, 30);		
				glPopMatrix();   
		glPopMatrix();
	}
	else 
	if(bindedTexture == NULL)
	{
		glPushMatrix();
			glTranslatef(axisX, axisY, axisZ);
				glPushMatrix();
					glRotatef(angle, 0.0f, 1.0f, 0.0f);
					glColor4f(0.5f+factorR, 0.5f+factorG, 0.5f+factorB, alphaVal);
					gluSphere(Sphere, SCALING, 30, 30);		
				glPopMatrix();   
		glPopMatrix();
	}
}

GLvoid RenderSphereWithPowerUp(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLUquadricObj *const Sphere, unsigned int *const bindedTexture, unsigned int *const innerBindedTexture, GLfloat innerSize, GLvoid (*BackdropFunc)(GLfloat, GLfloat, GLfloat, GLfloat))
// Rendering concentric and opaque sphere.
{
	if(BackdropFunc != NULL)
	{
		glBindTexture(GL_TEXTURE_2D, backdropTexture);
		(*BackdropFunc)(0.0f, 5.0f, -20.0f, 7.0f);
	}

	glBindTexture(GL_TEXTURE_2D, *innerBindedTexture);
	glPushMatrix();
		glTranslatef(axisX, axisY, axisZ);
		
		// Opaque Sphere...
		glPushMatrix();
			glRotatef(-angle, 0.0f, 1.0f, 0.0f);
			glColor4f(0.5f, 0.5f, 0.5f, 0.6f);
			gluQuadricTexture(Sphere, GL_TRUE);
			gluSphere(Sphere, innerSize, 30, 30);	// Create actual sphere.
		glPopMatrix();
		
		glBindTexture(GL_TEXTURE_2D, *bindedTexture);

		// Transparent Sphere...
		glPushMatrix();
			glRotatef(angle, 0.0f, 1.0f, 0.0f);
			glColor4f(factorR, factorG, factorB, 0.2f);
			gluQuadricTexture(Sphere, GL_TRUE);
			gluSphere(Sphere, SCALING, 30, 30);
		glPopMatrix();
	glPopMatrix();
}

GLvoid RenderPowerUp(void (*bounceFunc)(), GLfloat c4fa, GLfloat c4fb, GLfloat c4fc, GLUquadricObj *const Sphere, GLvoid (*BackdropFunc)(GLfloat, GLfloat, GLfloat, GLfloat))
// Rendering of whole power-up object in modelview.
{
	angle += 2.5f; // Rotation rate.

	if(angle >= 360.0f)
		angle = 0.0f;  

	glBindTexture(GL_TEXTURE_2D, backdropTexture);
	(*BackdropFunc)(0.0f, 5.0f, -20.0f, 7.0f);

	glBindTexture(GL_TEXTURE_2D, textureGeneration);

	glTranslatef(0.0f, bounce, -5.0f);

	glPushMatrix();
		glRotatef(angle, 0.0f, 1.0f, 0.0f);
		glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
		gluSphere(Sphere, 0.15f, 30, 30);
		glTranslatef(0.0f, 0.0f, -0.52f);
		glRotatef(-angle, 0.0f, 1.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		gluSphere(Sphere, 0.05f, 30, 30);
	glPopMatrix();

	glPushMatrix();
		glRotatef(-2.0f*angle, 0.0f, 1.0f, 0.0f);
		glColor4f(c4fa, c4fb, c4fc, 0.2f);
		gluSphere(Sphere, 0.6f, 30, 30);
	glPopMatrix();

	(*bounceFunc)();
}

GLvoid RenderPlayerSphere(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ, GLUquadricObj *const Sphere, unsigned int bindedTexture, const GLfloat colour[],  GLfloat alphaVal)
// Models the sphere segment for both Player 1 and NPC. Created in addition to 'RenderSphere' for more flexibility, such as reducing stacks and slices.
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bindedTexture);

	glPushMatrix();
		glTranslatef(axisX, axisY, axisZ);
		glScalef(scaleX, scaleY, scaleZ);
		glPushMatrix();
			glRotatef(angle, 0.0f, 1.0f, 0.0f);
			glColor4f(colour[0], colour[1], colour[2], alphaVal);
			gluQuadricTexture(Sphere, GL_TRUE);
			gluSphere(Sphere, 0.6f, 15, 15);		
		glPopMatrix();   
	glPopMatrix();
}

GLvoid RenderPlayerSphereWithPowerUp(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ, GLUquadricObj *const Sphere, unsigned int bindedTexture,  unsigned int innerBindedTexture, const GLfloat colour[], GLfloat innerSize, GLfloat alphaVal)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, innerBindedTexture);

	glPushMatrix();
		glTranslatef(axisX, axisY, axisZ);
		glScalef(scaleX, scaleY, scaleZ);
		// Opaque Sphere...
		glPushMatrix();
			glRotatef(-angle, 0.0f, 1.0f, 0.0f);
			glColor4f(0.5f, 0.5f, 0.5f, 0.6f);
			gluQuadricTexture(Sphere, GL_TRUE);
			gluSphere(Sphere, innerSize, 15, 15);
		glPopMatrix();
		
		glBindTexture(GL_TEXTURE_2D, bindedTexture);

		// Transparent Sphere...
		glPushMatrix();
			glRotatef(angle, 0.0f, 1.0f, 0.0f);
			glColor4f(colour[0], colour[1], colour[2], 0.2f);
			gluQuadricTexture(Sphere, GL_TRUE);
			gluSphere(Sphere, 0.6f, 15, 15);
		glPopMatrix();
	glPopMatrix();
}

GLvoid RenderPlayerSphereMarker(GLfloat axisX, GLfloat axisY, GLfloat axisZ, GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ, GLUquadricObj *const Sphere, const GLfloat colour[])
{
	glPushMatrix();
		glTranslatef(axisX, axisY, axisZ);
		glScalef(scaleX, scaleY, scaleZ);
		glPushMatrix();
			glColor3f(1.0f, 1.0f, 0.5f);		// Yellow marker.
			gluQuadricTexture(Sphere, GL_FALSE);
			gluSphere(Sphere, 0.8f, 15, 15);		
		glPopMatrix();   
	glPopMatrix();
}

GLvoid RenderPostApocalypticGardenOfEden(unsigned int landtxtr, unsigned int watertxtr, void (*bounceFunc)(), GLUquadricObj *marker,
										 GLUquadricObj *p1[], GLUquadricObj *p2[], GLUquadricObj *p3[],
										 TranslationCoords playerCoords, bool up, bool down, bool left, bool right,
										 TranslationCoords (*position)(TranslationCoords spA, bool up, bool down, bool left, bool right),
										 float alpha)
// Texture map and render Post Apocalyptic Garden of Eden.
{
	const GLfloat groundTranslation = 2350.0f;  // Used to translate overall environment.
	const GLfloat groundRotation	= 90.0f;   // Used to rotate environment into camera's view.
	const float   JIGGLE_AMP		= 15.0f;   // Emphasizes jiggle motion of objects.

	float radians = PI * (angle - 90.0f)/180.0f;
	float cameraX, cameraY, cameraZ;
	float lookX, lookY, lookZ;
	float waterHeight = 100.0f;
	bool waterDir = true;
	static bool chainIsSetForSupBlast = false, destroySpecificPowerUp[5] = {false, false, false, false, false};   // Do not render a power up once picked up!

	// AI character coordinate stores and auxilliary information.
	static TranslationCoords aiPlayer2Coords;	// Positioned to the left of the environment when game starts.
	static TranslationCoords aiPlayer3Coords;	// Positioned to the right of the environment when game starts.
	static bool p2up, p2down, p2left, p2right;	// Direction for NPC Player 2.
	static bool p3up, p3down, p3left, p3right;	// Direction for NPC Player 3.
	static bool initialiseAIcoords = false;

	// Environment positions for power ups.
	static int   powerUpPositions[] = {0, 1, 2, 3, 4};
	static float x_Pos[] = {LEFT_LIMIT, LEFT_LIMIT, 750.0f, RIGHT_LIMIT, RIGHT_LIMIT};		// Coordinates moved to these arrays for randomisation.
	static float z_Pos[] = {-1000.0f, NEAREND_LIMIT, FAREND_LIMIT, NEAREND_LIMIT, -1000.0f};
	static GLfloat powerUpPos[][3] = {{x_Pos[powerUpPositions[0]], 170.0f+bounce*JIGGLE_AMP, z_Pos[powerUpPositions[0]]},	// This one is for Supreme Blast (to the left).
									 {x_Pos[powerUpPositions[1]], 170.0f+bounce*-JIGGLE_AMP, z_Pos[powerUpPositions[1]]},	// This one is for Rebirth (to the left).
									 {x_Pos[powerUpPositions[2]], 170.0f+bounce*JIGGLE_AMP, z_Pos[powerUpPositions[2]]},	// This one is for Invisibility (far end).
									 {x_Pos[powerUpPositions[3]], 170.0f+bounce*-JIGGLE_AMP, z_Pos[powerUpPositions[3]]},	// This one is for Supreme Blast (to the right).
									 {x_Pos[powerUpPositions[4]], 170.0f+bounce*JIGGLE_AMP, z_Pos[powerUpPositions[4]]}};	// This one is for Rebirth (to the right).

	// Player/AI scores.
	static int player1score = 0, aiAlphaScore = 0, aiBetaScore = 0;

	// Temporary variables for collision detection.
	TranslationCoords tempPowerUp;
	bool changePlayerPowerUp[3] = {false, false, false};

	// Character animation support variables
	int sphereCounter = 1;	// Counter to index each ball in the chain (do not process leading sphere).

	// Camera point coordinates
	lookX = ((MAP_X_SIZE-5) * MAP_SCALE) / 1.5f;
	lookY = 400.0f;
	lookZ = -(MAP_Z_SIZE * MAP_SCALE) / 2.0f;

	// Actual camera position
	cameraX = lookX - 25.0f;	
	cameraY = lookY + 180.0f;
	cameraZ = 850.0f;

	// Now the player character...
	if(!initialiseAIcoords)
	{
		aiPlayer2Coords.xcoord = 400.0f; aiPlayer2Coords.ycoord = 270.0f; aiPlayer2Coords.zcoord = -1400.0f;
		aiPlayer3Coords.xcoord = 1160.0f; aiPlayer3Coords.ycoord = 270.0f; aiPlayer3Coords.zcoord = -1400.0f;
		powerUpTypeHeld[1] = NoPowerUp;
		powerUpTypeHeld[2] = NoPowerUp;
		initialiseAIcoords = true;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	gluLookAt(cameraX, cameraY, cameraZ, lookX, lookY, lookZ, 0.0, 1.0, 0.0);

	// Render scorboard or notification of winner.
	DisplayScoreboard(375.0f, 290.0f, 410.0f, player1score, aiAlphaScore, aiBetaScore);

	// Populate scene with object from here
	angle += 3.5f;		// Rotation speed.
	if(angle >= 360.0f)
		angle = 0.0f;

	// This one is for Supreme Blast (to the left)...
	if(!destroySpecificPowerUp[0])
	{
		glBindTexture(GL_TEXTURE_2D, powerUpTextureGeneration[1]);

		glPushMatrix();
			glTranslatef(powerUpPos[0][0], powerUpPos[0][1], powerUpPos[0][2]);
			glScalef(50.0f, 50.0f, -50.0f);
			// Opaque Sphere...
			glPushMatrix();
				glRotatef(angle, 0.0f, 1.0f, 0.0f);
				glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
				gluSphere(powerUpSphere[0], 0.15f, 30, 30);
				glTranslatef(0.0f, 0.0f, -0.52f);
				glRotatef(-angle, 0.0f, 1.0f, 0.0f);
				glColor3f(1.0f, 1.0f, 1.0f);
				gluSphere(powerUpSphere[0], 0.05f, 30, 30);
			glPopMatrix();
			// Transparent Sphere...
			glPushMatrix();
				glRotatef(-2.0f*angle, 0.0f, 1.0f, 0.0f);
				glColor4f(0.0f, 0.0f, 1.0f, 0.2f);
				gluSphere(powerUpSphere[0], 0.6f, 30, 30);
			glPopMatrix();
		glPopMatrix();
		(*bounceFunc)();
	// End Supreme Blast.
	}

	// This one is for Rebirth (to the left)...
	if(!destroySpecificPowerUp[1])
	{
		glBindTexture(GL_TEXTURE_2D, powerUpTextureGeneration[2]);

		glPushMatrix();
			glTranslatef(powerUpPos[1][0], powerUpPos[1][1], powerUpPos[1][2]);
			glScalef(50.0f, 50.0f, -50.0f);
			// Opaque Sphere...
			glPushMatrix();
				glRotatef(angle, 0.0f, 1.0f, 0.0f);
				glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
				gluSphere(powerUpSphere[1], 0.15f, 30, 30);
				glTranslatef(0.0f, 0.0f, -0.52f);
				glRotatef(-angle, 0.0f, 1.0f, 0.0f);
				glColor3f(1.0f, 1.0f, 1.0f);
				gluSphere(powerUpSphere[1], 0.05f, 30, 30);
			glPopMatrix();
			// Transparent Sphere...
			glPushMatrix();
				glRotatef(-2.0f*angle, 0.0f, 1.0f, 0.0f);
				glColor4f(1.0f, 0.0f, 0.0f, 0.2f);
				gluSphere(powerUpSphere[1], 0.6f, 30, 30);
			glPopMatrix();
		glPopMatrix();
		(*bounceFunc)();
	// End Rebirth.
	}

	// This one is for Invisibility (far end)...
	if(!destroySpecificPowerUp[2])
	{
		glBindTexture(GL_TEXTURE_2D, powerUpTextureGeneration[4]);

		glPushMatrix();
			glTranslatef(powerUpPos[2][0], powerUpPos[2][1], powerUpPos[2][2]);
			glScalef(50.0f, 50.0f, -50.0f);
			// Opaque Sphere...
			glPushMatrix();
				glRotatef(angle, 0.0f, 1.0f, 0.0f);
				glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
				gluSphere(powerUpSphere[2], 0.15f, 30, 30);
				glTranslatef(0.0f, 0.0f, -0.52f);
				glRotatef(-angle, 0.0f, 1.0f, 0.0f);
				glColor3f(1.0f, 1.0f, 1.0f);
				gluSphere(powerUpSphere[2], 0.05f, 30, 30);
			glPopMatrix();
			// Transparent Sphere...
			glPushMatrix();
				glRotatef(-2.0f*angle, 0.0f, 1.0f, 0.0f);
				glColor4f(0.5f, 0.5f, 0.5f, 0.2f);
				gluSphere(powerUpSphere[2], 0.6f, 30, 30);
			glPopMatrix();
		glPopMatrix();
		(*bounceFunc)();
	// End Invisibility.
	}

	// This one is for Supreme Blast (to the right)...
	if(!destroySpecificPowerUp[3])
	{
		glBindTexture(GL_TEXTURE_2D, powerUpTextureGeneration[1]);

		glPushMatrix();
			glTranslatef(powerUpPos[3][0], powerUpPos[3][1], powerUpPos[3][2]);
			glScalef(50.0f, 50.0f, -50.0f);
			// Opaque Sphere...
			glPushMatrix();
				glRotatef(angle, 0.0f, 1.0f, 0.0f);
				glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
				gluSphere(powerUpSphere[3], 0.15f, 30, 30);
				glTranslatef(0.0f, 0.0f, -0.52f);
				glRotatef(-angle, 0.0f, 1.0f, 0.0f);
				glColor3f(1.0f, 1.0f, 1.0f);
				gluSphere(powerUpSphere[3], 0.05f, 30, 30);
			glPopMatrix();
			// Transparent Sphere...
			glPushMatrix();
				glRotatef(-2.0f*angle, 0.0f, 1.0f, 0.0f);
				glColor4f(0.0f, 0.0f, 1.0f, 0.2f);
				gluSphere(powerUpSphere[3], 0.6f, 30, 30);
			glPopMatrix();
		glPopMatrix();
		(*bounceFunc)();
	// End Supreme Blast.
	}

	// This one is for Rebirth (to the right)...
	if(!destroySpecificPowerUp[4])
	{
		glBindTexture(GL_TEXTURE_2D, powerUpTextureGeneration[2]);

		glPushMatrix();
			glTranslatef(powerUpPos[4][0], powerUpPos[4][1], powerUpPos[4][2]);
			glScalef(50.0f, 50.0f, -50.0f);
			// Opaque Sphere...
			glPushMatrix();
				glRotatef(angle, 0.0f, 1.0f, 0.0f);
				glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
				gluSphere(powerUpSphere[4], 0.15f, 30, 30);
				glTranslatef(0.0f, 0.0f, -0.52f);
				glRotatef(-angle, 0.0f, 1.0f, 0.0f);
				glColor3f(1.0f, 1.0f, 1.0f);
				gluSphere(powerUpSphere[4], 0.05f, 30, 30);
			glPopMatrix();
			// Transparent Sphere...
			glPushMatrix();
				glRotatef(-2.0f*angle, 0.0f, 1.0f, 0.0f);
				glColor4f(1.0f, 0.0f, 0.0f, 0.2f);
				gluSphere(powerUpSphere[4], 0.6f, 30, 30);
			glPopMatrix();
		glPopMatrix();
		(*bounceFunc)();
	// End Rebirth.
	}

	// Human Player 1 chain character rendered here.

	// Render the sphere(s)... paying attention to the power up to be rendered with the switch statement
	RenderPlayerSphereMarker(playerCoords.xcoord, 230.0f, playerCoords.zcoord, 10.0f, 10.0f, 10.0f, marker, verte);
	CharacterRenderSupportFunction(powerUpTypeHeld[0], playerCoords, p1, up, down, left, right, position, alpha, player1ChainLength, true);
	// End Player 1's rendering.

	// NPC Player 2 chain character rendered here.
	p2up = up; p2down = down; p2left = left; p2right = right;

	CharacterRenderSupportFunction(powerUpTypeHeld[1], aiPlayer2Coords, p2, p2up, p2down, p2left, p2right, position, alpha, player2ChainLength, false);
	// End NPC Player 2's rendering.

	// NPC Player 3 chain character rendered here.
	p3up = up; p3down = down; p3left = left; p3right = right;

	CharacterRenderSupportFunction(powerUpTypeHeld[2], aiPlayer3Coords, p3, p3up, p3down, p3left, p3right, position, alpha, player3ChainLength, false);
	// End NPC Player 3's rendering.

	// Work out AI interactions.
	
	PrioritisationEngine(&playerCoords, &aiPlayer2Coords, &aiPlayer3Coords,
						 player1ChainLength, player2ChainLength, player3ChainLength,
						 powerUpTypeHeld[0], powerUpTypeHeld[1], powerUpTypeHeld[2], AttractGameObjects,
						 RepelGameObjects,
						 LEFT_LIMIT, RIGHT_LIMIT, FAREND_LIMIT, NEAREND_LIMIT,
						 (TranslationCoords *) powerUpPos[0], (TranslationCoords *) powerUpPos[1], (TranslationCoords *) powerUpPos[2],
						 (TranslationCoords *) powerUpPos[3], (TranslationCoords *) powerUpPos[4], destroySpecificPowerUp[0], destroySpecificPowerUp[1],
						 destroySpecificPowerUp[2], destroySpecificPowerUp[3], destroySpecificPowerUp[4]);
	
	// Check for collision between player and AI and AI-AI.
	ApplyGameRules(playerCoords, aiPlayer2Coords, aiPlayer3Coords, player1ChainLength, player2ChainLength, player3ChainLength,
					powerUpTypeHeld[0], powerUpTypeHeld[1], powerUpTypeHeld[2], player1score, aiAlphaScore, aiBetaScore);

	// End AI interactions.

// End scene population here.

// Establish if collision has taken place.
///	playerCoords.ycoord = 170.0f; aiPlayer2Coords.ycoord = 170.0f; aiPlayer3Coords.ycoord = 170.0f;

	tempPowerUp.xcoord = powerUpPos[0][0]; tempPowerUp.ycoord = powerUpPos[0][1]; tempPowerUp.zcoord = powerUpPos[0][2];
	if((changePlayerPowerUp[0] = EstablishCollision(tempPowerUp, playerCoords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[0] ||
		(changePlayerPowerUp[1] = EstablishCollision(tempPowerUp, aiPlayer2Coords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[0] ||
		(changePlayerPowerUp[2] = EstablishCollision(tempPowerUp, aiPlayer3Coords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[0])
	{
		destroySpecificPowerUp[0] = true;

		if(changePlayerPowerUp[0])
		{
			// Set power up for player.
			if(powerUpTypeHeld[0] == Rebirth)	// If power up was previously Rebirth, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[0] = SupaFly;
			}
			else if(powerUpTypeHeld[0] != SupremeBlast)
			{
				if(musicCurrentlyPlaying != SupremeBlast)
				{
					musicCurrentlyPlaying = SupremeBlast;
					PlaySupremeBlastMusic();
					PlaySupremeBlastSpeechAudio();
				}
				powerUpTypeHeld[0] = SupremeBlast;
			}
		}

		if(changePlayerPowerUp[1])
		{
			if(powerUpTypeHeld[1] == Rebirth)	// If power up was previously Rebirth, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[1] = SupaFly;
			}
			else if(powerUpTypeHeld[1] != SupremeBlast)
			{
				if(musicCurrentlyPlaying != SupremeBlast)
				{
					musicCurrentlyPlaying = SupremeBlast;
					PlaySupremeBlastMusic();
					PlaySupremeBlastSpeechAudio();
				}
				powerUpTypeHeld[1] = SupremeBlast;
			}
		}

		if(changePlayerPowerUp[2])
		{
			if(powerUpTypeHeld[2] == Rebirth)	// If power up was previously Rebirth, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[2] = SupaFly;
			}
			else if(powerUpTypeHeld[2] != SupremeBlast)
			{
				if(musicCurrentlyPlaying != SupremeBlast)
				{
					musicCurrentlyPlaying = SupremeBlast;
					PlaySupremeBlastMusic();
					PlaySupremeBlastSpeechAudio();
				}
				powerUpTypeHeld[2] = SupremeBlast;
			}
		}
	}

	tempPowerUp.xcoord = powerUpPos[1][0]; tempPowerUp.ycoord = powerUpPos[1][1]; tempPowerUp.zcoord = powerUpPos[1][2];
	if((changePlayerPowerUp[0] = EstablishCollision(tempPowerUp, playerCoords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[1] ||
		(changePlayerPowerUp[1] = EstablishCollision(tempPowerUp, aiPlayer2Coords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[1] ||
		(changePlayerPowerUp[2] = EstablishCollision(tempPowerUp, aiPlayer3Coords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[1])
	{
		destroySpecificPowerUp[1] = true;

		if(changePlayerPowerUp[0])
		{
			// Set power up for player.
			if(powerUpTypeHeld[0] == SupremeBlast)	// If power up was previously SupremeBlast, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[0] = SupaFly;
			}
			else if(powerUpTypeHeld[0] != Rebirth)
			{
				if(musicCurrentlyPlaying != Rebirth)
				{
					musicCurrentlyPlaying = Rebirth;
					PlayRebirthMusic();
					PlayRebirthSpeechAudio();
				}
				powerUpTypeHeld[0] = Rebirth;
			}
		}

		if(changePlayerPowerUp[1])
		{
			// For AI player 2.
			if(powerUpTypeHeld[1] == SupremeBlast)	// If power up was previously SupremeBlast, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[1] = SupaFly;
			}
			else if(powerUpTypeHeld[1] != Rebirth)
			{
				if(musicCurrentlyPlaying != Rebirth)
				{
					musicCurrentlyPlaying = Rebirth;
					PlayRebirthMusic();
					PlayRebirthSpeechAudio();
				}
				powerUpTypeHeld[1] = Rebirth;
			}
		}

		if(changePlayerPowerUp[2])
		{
			// For AI player 3.
			if(powerUpTypeHeld[2] == SupremeBlast)	// If power up was previously SupremeBlast, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[2] = SupaFly;
			}
			else if(powerUpTypeHeld[2] != Rebirth)
			{
				if(musicCurrentlyPlaying != Rebirth)
				{
					musicCurrentlyPlaying = Rebirth;
					PlayRebirthMusic();
					PlayRebirthSpeechAudio();
				}
				powerUpTypeHeld[2] = Rebirth;
			}
		}
	}

	tempPowerUp.xcoord = powerUpPos[2][0]; tempPowerUp.ycoord = powerUpPos[2][1]; tempPowerUp.zcoord = powerUpPos[2][2];
	if((changePlayerPowerUp[0] = EstablishCollision(tempPowerUp, playerCoords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[2] ||
		(changePlayerPowerUp[1] = EstablishCollision(tempPowerUp, aiPlayer2Coords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[2] ||
		(changePlayerPowerUp[2] = EstablishCollision(tempPowerUp, aiPlayer3Coords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[2])
	{
		destroySpecificPowerUp[2] = true;
		// Set power up for player.
		if(changePlayerPowerUp[0])
		{
			if(musicCurrentlyPlaying != Invisibility)
			{
				musicCurrentlyPlaying = Invisibility;
				PlayInvisibilityMusic();
				PlayInvisibilitySpeechAudio();
			}
			powerUpTypeHeld[0] = Invisibility;
		}

		if(changePlayerPowerUp[1])
		{
			if(musicCurrentlyPlaying != Invisibility)
			{
				musicCurrentlyPlaying = Invisibility;
				PlayInvisibilityMusic();
				PlayInvisibilitySpeechAudio();
			}
			powerUpTypeHeld[1] = Invisibility;
		}

		if(changePlayerPowerUp[2])
		{
			if(musicCurrentlyPlaying != Invisibility)
			{
				musicCurrentlyPlaying = Invisibility;
				PlayInvisibilityMusic();
				PlayInvisibilitySpeechAudio();
			}
			powerUpTypeHeld[2] = Invisibility;
		}
	}

	tempPowerUp.xcoord = powerUpPos[3][0]; tempPowerUp.ycoord = powerUpPos[3][1]; tempPowerUp.zcoord = powerUpPos[3][2];
	if((changePlayerPowerUp[0] = EstablishCollision(tempPowerUp, playerCoords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[3] ||
		(changePlayerPowerUp[1] = EstablishCollision(tempPowerUp, aiPlayer2Coords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[3] ||
		(changePlayerPowerUp[2] = EstablishCollision(tempPowerUp, aiPlayer3Coords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[3])
	{
		destroySpecificPowerUp[3] = true;

		if(changePlayerPowerUp[0])
		{
			// Set power up for player.
			if(powerUpTypeHeld[0] == Rebirth)	// If power up was previously Rebirth, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[0] = SupaFly;
			}
			else if(powerUpTypeHeld[0] != SupremeBlast)
			{
					if(musicCurrentlyPlaying != SupremeBlast)
					{
						musicCurrentlyPlaying = SupremeBlast;
						PlaySupremeBlastMusic();
						PlaySupremeBlastSpeechAudio();
					}
					powerUpTypeHeld[0] = SupremeBlast;
			}
		}

		if(changePlayerPowerUp[1])
		{
			// For AI player 2.
			if(powerUpTypeHeld[1] == Rebirth)	// If power up was previously Rebirth, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[1] = SupaFly;
			}
			else if(powerUpTypeHeld[1] != SupremeBlast)
			{
					if(musicCurrentlyPlaying != SupremeBlast)
					{
						musicCurrentlyPlaying = SupremeBlast;
						PlaySupremeBlastMusic();
						PlaySupremeBlastSpeechAudio();
					}
					powerUpTypeHeld[1] = SupremeBlast;
			}
		}

		if(changePlayerPowerUp[2])
		{
			// For AI player 3.
			if(powerUpTypeHeld[2] == Rebirth)	// If power up was previously Rebirth, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[2] = SupaFly;
			}
			else if(powerUpTypeHeld[2] != SupremeBlast)
			{
					if(musicCurrentlyPlaying != SupremeBlast)
					{
						musicCurrentlyPlaying = SupremeBlast;
						PlaySupremeBlastMusic();
						PlaySupremeBlastSpeechAudio();
					}
					powerUpTypeHeld[2] = SupremeBlast;
			}
		}
	}

	tempPowerUp.xcoord = powerUpPos[4][0]; tempPowerUp.ycoord = powerUpPos[4][1]; tempPowerUp.zcoord = powerUpPos[4][2];
	if((changePlayerPowerUp[0] = EstablishCollision(tempPowerUp, playerCoords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[4] ||
		(changePlayerPowerUp[1] = EstablishCollision(tempPowerUp, aiPlayer2Coords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[4] ||
		(changePlayerPowerUp[2] = EstablishCollision(tempPowerUp, aiPlayer3Coords, COLLISION_SENSITIVITY)) && !destroySpecificPowerUp[4])
	{
		destroySpecificPowerUp[4] = true;

		if(changePlayerPowerUp[0])
		{
			// Set power up for player.
			if(powerUpTypeHeld[0] == SupremeBlast)	// If power up was previously SupremeBlast, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[0] = SupaFly;
			}
			else if(powerUpTypeHeld[0] != Rebirth)
			{
				if(musicCurrentlyPlaying != Rebirth)
				{
					musicCurrentlyPlaying = Rebirth;
					PlayRebirthMusic();
					PlayRebirthSpeechAudio();
				}
				powerUpTypeHeld[0] = Rebirth;
			}
		}

		if(changePlayerPowerUp[1])
		{
			// For AI player 2.
			if(powerUpTypeHeld[1] == SupremeBlast)	// If power up was previously SupremeBlast, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[1] = SupaFly;
			}
			else if(powerUpTypeHeld[1] != Rebirth)
			{
				if(musicCurrentlyPlaying != Rebirth)
				{
					musicCurrentlyPlaying = Rebirth;
					PlayRebirthMusic();
					PlayRebirthSpeechAudio();
				}
				powerUpTypeHeld[1] = Rebirth;
			}
		}

		if(changePlayerPowerUp[2])
		{
			// For AI player 3.
			if(powerUpTypeHeld[2] == SupremeBlast)	// If power up was previously SupremeBlast, then upgrade to SupaFly as both power ups are held!
			{
				if(musicCurrentlyPlaying != SupaFly)
				{
					musicCurrentlyPlaying = SupaFly;
					PlaySupaFlyMusic();
					PlaySupaFlySpeechAudio();
				}
				powerUpTypeHeld[2] = SupaFly;
			}
			else if(powerUpTypeHeld[2] != Rebirth)
			{
				if(musicCurrentlyPlaying != Rebirth)
				{
					musicCurrentlyPlaying = Rebirth;
					PlayRebirthMusic();
					PlayRebirthSpeechAudio();
				}
				powerUpTypeHeld[2] = Rebirth;
			}
		}
	}

	// Maintain vertical motion of power ups.
	powerUpPos[0][1] = 170.0f+bounce*JIGGLE_AMP;
	powerUpPos[1][1] = 170.0f-bounce*JIGGLE_AMP;
	powerUpPos[2][1] = 170.0f+bounce*JIGGLE_AMP;
	powerUpPos[3][1] = 170.0f-bounce*JIGGLE_AMP;
	powerUpPos[4][1] = 170.0f+bounce*JIGGLE_AMP;

	// Restore all power ups once consumed and update scoreboard...
	if(destroySpecificPowerUp[0] && destroySpecificPowerUp[1] && destroySpecificPowerUp[2] && destroySpecificPowerUp[3] && destroySpecificPowerUp[4])
	{
		// Update scoreboard...
		if(player1ChainLength > player2ChainLength && player1ChainLength > player3ChainLength)
			player1score++;
		if(player2ChainLength > player1ChainLength && player2ChainLength > player3ChainLength)
			aiAlphaScore++;
		if(player3ChainLength > player1ChainLength && player3ChainLength > player2ChainLength)
			aiBetaScore++;

		// Reset scores...
		PlayPAGoEMusic();

		for(int i = 0; i < 5; i++)
			destroySpecificPowerUp[i] = false;

		for(int j = 0; j < 3; j++)
		{
			powerUpTypeHeld[j] = NoPowerUp;
			chainExtended[j] = false;
		}

		player1ChainLength = 2;
		player2ChainLength = 2;
		player3ChainLength = 2;

		// Reassign power up positions in random order
		PositionPowerUps((int *) &powerUpPositions);		// Randomise order.
		powerUpPos[0][0] = x_Pos[powerUpPositions[0]]; powerUpPos[0][2] = z_Pos[powerUpPositions[0]];
		powerUpPos[1][0] = x_Pos[powerUpPositions[1]]; powerUpPos[1][2] = z_Pos[powerUpPositions[1]];
		powerUpPos[2][0] = x_Pos[powerUpPositions[2]]; powerUpPos[2][2] = z_Pos[powerUpPositions[2]];
		powerUpPos[3][0] = x_Pos[powerUpPositions[3]]; powerUpPos[3][2] = z_Pos[powerUpPositions[3]];
		powerUpPos[4][0] = x_Pos[powerUpPositions[4]]; powerUpPos[4][2] = z_Pos[powerUpPositions[4]];
	}

// End collision detection here.

	glBindTexture(GL_TEXTURE_2D, landtxtr);

	glPushMatrix();
		glTranslatef(groundTranslation, 0.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, 200.0f);
		glRotatef(groundRotation, 0.0f, 1.0f, 0.0f);
		glScalef(1.5f, 1.05f, 3.5f);
		for(int z = 0; z < MAP_Z_SIZE - 1; z++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			for(int x = 0; x < MAP_X_SIZE - 1; x++)
			{
				// Draw vertex 0: Vertices drawn in 'Z' formation. Texture coords pasted upside down.
				glColor3f(terrain[x][z][1]/255.0f, terrain[x][z][1]/255.0f, terrain[x][z][1]/255.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(terrain[x][z][0], terrain[x][z][1], terrain[x][z][2]);

				// Vertex 1...
				glTexCoord2f(1.0f, 0.0f);
				glColor3f(terrain[x+1][z][1]/255.0f, terrain[x+1][z][1]/255.0f, terrain[x+1][z][1]/255.0f);
				glVertex3f(terrain[x+1][z][0], terrain[x+1][z][1], terrain[x+1][z][2]);

				// Vertex 2...
				glTexCoord2f(0.0f, 1.0f);
				glColor3f(terrain[x][z+1][1]/255.0f, terrain[x][z+1][1]/255.0f, terrain[x][z+1][1]/255.0f);
				glVertex3f(terrain[x][z+1][0], terrain[x][z+1][1], terrain[x][z+1][2]);

				// Vertex 3...
				glTexCoord2f(1.0f, 1.0f);
				glColor3f(terrain[x+1][z+1][1]/255.0f, terrain[x+1][z+1][1]/255.0f, terrain[x+1][z+1][1]/255.0f);
				glVertex3f(terrain[x+1][z+1][0], terrain[x+1][z+1][1], terrain[x+1][z+1][2]);

			}
			glEnd();
		}

	glPopMatrix();

	glEnable(GL_BLEND);

	// Read-only depth buffer
	glDepthMask(GL_FALSE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glColor4f(0.5f, 0.5f, 1.0f, 0.7f); // Transparent blue water
	glBindTexture(GL_TEXTURE_2D, watertxtr);

	// Render water...
	glPushMatrix();
		glTranslatef(groundTranslation, 0.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, 200.0f);
		glRotatef(groundRotation, 0.0f, 1.0f, 0.0f);
		glScalef(1.5f, 1.05f, 3.5f);
		glBegin(GL_QUADS);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(terrain[0][0][0], waterHeight, terrain[0][0][2]);

			glTexCoord2f(10.0f, 0.0f);
			glVertex3f(terrain[MAP_X_SIZE - 1][0][0], waterHeight, terrain[MAP_X_SIZE - 1][0][2]);

			glTexCoord2f(10.0f, 10.0f);
			glVertex3f(terrain[MAP_X_SIZE - 1][MAP_Z_SIZE - 1][0], waterHeight, terrain[MAP_X_SIZE - 1][MAP_Z_SIZE - 1][2]);

			glTexCoord2f(0.0f, 10.0f);
			glVertex3f(terrain[0][MAP_Z_SIZE - 1][0], waterHeight, terrain[0][MAP_Z_SIZE - 1][2]);
		glEnd();
	glPopMatrix();

	// Writeable depth buffer
	glDepthMask(GL_TRUE);

	glDisable(GL_BLEND);

	if(waterHeight > 500.0f)
		waterDir = false;
	else if(waterHeight < 100.0f)
		waterDir = true;

	if(waterDir)
		waterHeight += 10.8f;
	else
		waterHeight -= 10.8f;

	glFlush();
	SwapBuffers(g_HDC);
}

void CharacterRenderSupportFunction(POWERUP_TYPE plPowerUpHeld, TranslationCoords &plCoords, GLUquadricObj *plChain[], bool up, bool down, bool left, bool right,
									TranslationCoords (*position)(TranslationCoords spA, bool up, bool down, bool left, bool right), float pAlpha, int plChainLength, bool isHuman)
// Function to remove character code clutter from main scene rendering function.
{
	switch(plPowerUpHeld)
	{
		case NoPowerUp:
			RenderACharacterOnSceneWithoutPowerUp(plCoords.xcoord, 170.0f, plCoords.zcoord, &plChain[0], powerUpTextureGeneration[0], up, down, left, right, verte, position, pAlpha, plChainLength);
			break;
		case SupremeBlast:
			RenderACharacterOnSceneWithPowerUp(plCoords.xcoord, 170.0f, plCoords.zcoord, &plChain[0], powerUpTextureGeneration[1], powerUpTextureGeneration[0], 0.45f, pAlpha,
											up, down, left, right, bleu, position, plChainLength);
			break;
		case Rebirth:
			RenderACharacterOnSceneWithPowerUp(plCoords.xcoord, 170.0f, plCoords.zcoord, &plChain[0], powerUpTextureGeneration[2], powerUpTextureGeneration[0], 0.45f, pAlpha,
											up, down, left, right, rouge, position, plChainLength);
			break;
		case Invisibility:
			RenderACharacterOnSceneWithoutPowerUp(plCoords.xcoord, 170.0f, plCoords.zcoord, &plChain[0], powerUpTextureGeneration[4], up, down, left, right, grey, position, pAlpha, plChainLength);
			break;
		case SupaFly:
			RenderACharacterOnSceneWithPowerUp(plCoords.xcoord, 170.0f, plCoords.zcoord, &plChain[0], powerUpTextureGeneration[3], powerUpTextureGeneration[0], 0.45f, pAlpha,
											up, down, left, right, blanc, position, plChainLength);
			break;
		default:
			MessageBox(NULL, "GraphicsSource.cpp: Undefined effect in code!", "Error...", MB_OK);
			break;
	}
}

GLvoid FixBackdropTexture()
// Used to set up backdrop texture for display.
{
	if(!bdTexture)									// If not loaded then load backdrop texture.
		bdTexture = LoadBitmapFile("KBSky.bmp", &backdropInfo);	

	if(!bdTexture)									// If still not loaded, kick a fuss.
	{
		MessageBox(NULL, "Scene bitmap is missing.", szBitmapLoadingError, MB_OK);
	}

	LoadBackdropTexture();
}

GLvoid RenderBackdrop(GLfloat xProj, GLfloat yProj, GLfloat zProj, GLfloat scaleFactor)
// Render texture mapped backdrop.
{
	glPushAttrib(GL_COLOR_BUFFER_BIT);		// Save colour attributes.
		glPushMatrix();
			glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
			glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
			glTranslatef(xProj, yProj, zProj);
			glScalef(scaleFactor, scaleFactor, scaleFactor);
			glColor4f(0.9f, 0.9f, 1.0f, 0.99f);

			glBegin(GL_QUADS);	
				glTexCoord2f(0.0f, 0.0f); glVertex3f(1.5f, -1.5f, 0.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(1.5f, 1.5f, 1.5f);	
				glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.5f, 1.5f, 1.5f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.5f, -1.5f, 1.5f);
			glEnd();
		glPopMatrix();
	glPopAttrib();
}

GLvoid DisableSphereOGLFunctions()
// Reset OpenGL abilities after sphere rendering.
{
	glDisable(GL_DEPTH_TEST);					
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
}

GLvoid DisableTransparentsOGLFunctions()
// Reset OpenGL abilities after transparent object rendering.
{
	if(glIsEnabled(GL_FOG))
		glDisable(GL_FOG);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);							
	glDisable(GL_AUTO_NORMAL);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
}

void ConstructModelChainMotion()
// Ensures chain linkage is maintained when you choose the modelview options.
{
	if(animationCounter < (0.8f+TRANSDECREMENT))
		animationCounter = 0.8f;
	else
		animationCounter -= TRANSDECREMENT;

	if(chainBounce[0] > 1.7f)
		boolVal = false;
	if(chainBounce[0] < -1.2f)
		boolVal = true; 

	if(!boolVal)
	{
		for(counter = 0; counter < CHAINMAXLENGTH; counter++)
		{
			(counter == 0) ? chainBounce[counter] = static_cast <float> (-sin(static_cast <double> (sineAngle))) :
							 chainBounce[counter] = static_cast <float> (-sin(static_cast <double> (sineAngle+static_cast <float> (counter)*LINKDELAY)));
		}
	}
	if(boolVal)
	{
		for(counter = 0; counter < CHAINMAXLENGTH; counter++)
		{
			(counter == 0) ? chainBounce[counter] = static_cast <float> (sin(static_cast <double> (sineAngle))) :
							 chainBounce[counter] = static_cast <float> (sin(static_cast <double> (sineAngle+static_cast <float> (counter)*LINKDELAY)));
		}
	}
}

void SetSphereSpotlight()
// Sets spotlight variables to illuminate spheres.
{
	// Spotlight coordinates to highlight sphere...
	spotlightPosition[0] = 3.0f;	// On x-axis
	spotlightPosition[1] = 0.5f;	// On y-axis
	spotlightPosition[2] = 0.0f;	// On z-axis
	spotlightPosition[3] = 1.0f;	// 0 = Vector, 1 = Positional.
	spotlightDirection[0] = -1.0f;	// On x-axis
	spotlightDirection[1] = 0.0f;	// On y-axis
	spotlightDirection[2] = -1.0f;	// On z-axis
	spotlightPositionBack[0] = -0.5f;	
	spotlightPositionBack[1] = 0.9f;	
	spotlightPositionBack[2] = -15.0f;	
	spotlightPositionBack[3] = 1.0f;	
	spotlightDirectionBack[0] = 0.0f;	
	spotlightDirectionBack[1] = 0.0f;
	spotlightDirectionBack[2] = 1.0f;
}

void SetEnvironmentSpotlight()
// Sets spotlight variables to illuminate Post Apocalyptic Garden of Eden..
{
	// Spotlight coordinates to highlight environment...
	spotlightPosition[0] = 2.0f;
	spotlightPosition[1] = 0.5f;
	spotlightPosition[2] = 0.5f;
	spotlightPosition[3] = 1.0f;
	spotlightDirection[0] = 0.0f;
	spotlightDirection[1] = 0.0f;
	spotlightDirection[2] = -1.0f;
}

