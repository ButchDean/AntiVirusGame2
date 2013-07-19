/*!\file AntiVirusGame.cpp
 * \brief Main source file for AntiVirus 2 Game.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

// AntiVirusGame.cpp: Version 2.

/*! \var unsigned long first;
    \brief First number in the sequence equal to 1.
*/

/*! \var unsigned long second
    \brief Second number in the sequence equal to 1.
*/

/*! \var unsigned long prevSum
    \brief Stores the sum for the previous iteration.
*/

/*! \var unsigned long theSum
    \brief Stores the sum for the current iteration.
*/

#pragma warning(disable: 4098)

#include "StandardIncludes.h"
#include "DirectSoundIncludes.h"
#include "DirectInputIncludes.h"
#include "LoadBitmaps.h"
#include "LoadTextures.h"
#include "CharacterAnimationEngine.h"
#include "GameAI.h"
#include "ScreenResolutionManagement.h"
#include "PlayerDefinitions.h"
#include "FramerateSync.h"
#include "Scoreboard.h"
#include "GUIMenu.h"
#include "RestoreGameResolution.h"
#include "resource.h"


// Global constants...
extern const char szBitmapLoadingError[] = "Bitmap loading error...";

extern const float animationStop	= 343.0f;

extern const int	switchSphere	= 5;						

extern const int MAP_X_SIZE			= 32;
extern const int MAP_Z_SIZE			= 32;
extern const float MAP_SCALE		= 20.0f;
extern const float PI				= 3.1415927f;
extern const int CHAINMAXLENGTH		= 7;						// Maximum chain length in spheres.
extern const float XTRAVELSTART		= 2.6f;					// Start of chain.
extern const float LINKDELAY		= -0.25f;					// Used to maintain links in chain.
extern const float SCALING			= 0.5f;						// For sphere resizing.
extern const int waterWIDTH			= 150;
extern const int waterHEIGHT		= 100;
extern const int waterDEPTH			= 3;
extern const int waterSURFACES		= 3;						// Set this value to 1 for testing purposes.
extern const float INITIALTRANSFORMATIONCOLOURS = 0.5f;
extern const float ANIMATIONSTART	= 1.1f;						// Used for morphing timing.
extern const int   KEYBUFCOUNT		= 256;

extern const int SPHERENUMS = 7;

extern BITMAPINFOHEADER	bitmapInfoHeader;
extern BITMAPINFOHEADER landInfo;
extern BITMAPINFOHEADER backdropInfo;
extern BITMAPINFOHEADER textureInfo[2];

extern unsigned char *imageData;
extern unsigned char *landTexture;
extern unsigned char *bdTexture;
extern unsigned char *sphereTexture;
extern unsigned char *quadSphereTexture;
extern unsigned int  land;
extern unsigned int  backdropTexture;	   
extern unsigned int  textureGeneration;
extern unsigned int  quadTextureGeneration;

extern TranslationCoords SphereObjectCoords[SPHERENUMS];


// Global variables...
HWND ghwnd = NULL;													// Global window handle.
GLUquadricObj *texturedSphere = NULL;
GLUquadricObj *ASphere[CHAINMAXLENGTH];	


char szWinName[] = "AVG 2";						// name of window class. 
extern float bounce;									// sphere animation guide.
extern float angle;										// rotation angle.
extern float accumulator;										// Used to start and stop animation.
extern float waterAccumulator;							// Used for water animation.
extern int	  modExtraction, delay;								// Used for Supreme Blast + Rebirth effect.
extern float colorR;									// Red colour saturation for standard sphere.
extern float colorB;									// Blue colour saturation for standard sphere.
extern int   mouseX, mouseY, oldMouseX, oldMouseY;
extern float lookX, lookY, lookZ;
extern float terrain[MAP_X_SIZE][MAP_Z_SIZE][3];
extern float sineAngle;
extern float bounceChain[CHAINMAXLENGTH];
extern int counter;
extern float animationCounter;
extern float chainBounce[CHAINMAXLENGTH];
extern float factorR;			// For birth/destruction colour manipulation.
extern float factorG;
extern float factorB;
extern float scaling;									// For sphere resizing.
extern bool  boolVal;
extern bool  chainUsed;								// Notification that memory was allocated for chain.
extern float blue[];
extern DWORD dwStatus;
								
// lighting variables for sphere
extern float ambientLight[];		// ambient light
extern float diffuseLight[];		// diffuse light

extern float spotlightPosition[4];								// spotlight position
extern float spotlightDirection[3];							// spotlight direction
extern float spotlightPositionBack[4];							// spotlight position
extern float spotlightDirectionBack[3];						// spotlight direction

// material variables
extern float matAmbient[];			// ambient material
extern float matDiff[];				// diffuse material
extern float matSpecular[];			// specular material

extern HDC g_HDC;

// Sound externs
extern LPDIRECTSOUNDBUFFER lpDSBSounds[];
extern LPDIRECTSOUND	   lpDS;			// Include for cleanup on program termination.

// DirectInput externs
extern LPDIRECTINPUTDEVICE pKeyboard;
extern char                keyboardBuffer[];

// powerUpImageData and powerUpBitmapInfoHeader Exclusive to Post Apocalyptic Garden of Eden.
extern const int NUMEFFECTS = 5;
extern unsigned int powerUpTextureGeneration[NUMEFFECTS];
extern unsigned char *powerUpImageData[NUMEFFECTS];
extern unsigned char *powerUpTexture[NUMEFFECTS];
extern BITMAPINFOHEADER powerUpBitmapInfoHeader[NUMEFFECTS];
extern BITMAPINFOHEADER powerUpBitmapInfo[NUMEFFECTS];
extern bool  bLineUpOnLongitudeUp, bLineUpOnLongitudeDown, bLineUpOnLatitudeR, bLineUpOnLatitudeL;

// Used for scoreboard font.
extern UINT g_FontListID;
extern const float FONT_EXTRUDE = 0.4f;

GLUquadricObj *powerUpSphere[4];			// Four power ups for PAGoE and other environments.

// local constants
/*! glFog effect settings. */
const float fogParams[] = {0.5f, 0.5f, 0.5f, 1.0f};
/*! Two times Pi. */
const float TWOPI = 2.0f * PI;
/*! How quickly the characters move across the screen. */
const float MOVE_RATE		= 50.0f;

// Universal limits of movement across the environment.
/*! The furthest point to the right of the camera that the characters can move. */
const float X_MOTION_LIMIT_UPPER = 1350.0f;
/*! The furthest point to the left of the camera that the characters can move. */
const float X_MOTION_LIMIT_LOWER = 200.0f;
/*! The furthest point above the camera that the characters can move. */
const float Y_MOTION_LIMIT_UPPER = 170.0f;
/*! The furthest point below the camera that the characters can move, although still above the mid-point of the y-axis. */
const float Y_MOTION_LIMIT_LOWER = 170.0f;
/*! The furthest point away from the camera that the characters can move. */
const float Z_MOTION_LIMIT_UPPER = -2000.0f;
/*! The nearest point to the camera that the characters can move. */
const float Z_MOTION_LIMIT_LOWER = -300.0f;

// File scope variables
/*! Direction that the player's character moves in: 0 = No motion, 1 = Left, 2 = Right, 3 = Up, 4 = Down. */
int  keyPressed = 0;
/*! Used to determine if a sound is currently playing. */
bool  soundPlayed = false;
/*! Used to select windowed or full screen for game. */
bool  windowed = true;
/*! Check if effects have already been initialised. */
int   iFeatureIsInitialised = 0;
/*! Tracks the rotation agle for all spherical in the game. */
float rotationAngle = 0.0f;

// Menu handling varables
/*! Tracks menu to be displayed. */
int currentMenu = 0;
/*! Global coordinates of mouse click. */
int g_x_region, g_y_region;
/*! Menu option currently selected. */
MENU_OPTION currentMenuOption;
/*! Global wParam. */
WPARAM g_wParam;
/*! Global lParam. */
LPARAM g_lParam;

// Function prototypes...
LRESULT CALLBACK WindowFunc(HWND, UINT, WPARAM, LPARAM);
void SetupPixelFormat(HDC hDC);
void FreeChainMemory();	
void dik_BuildStandardSphereChain();
void dik_StandardSphereChain();
void dik_DeconstructStandardSphereChain();
void dik_BuildSphereChainWithPowerUp(GLfloat factorR, GLfloat factorG, GLfloat factorB, char* outerTexture);
void dik_DeconstructSphereChainWithPowerUp();
void dik_SphereChainWithSupremeBlast();
void dik_SphereChainWithRebirth();
void dik_SphereChainWithInvisibility();
void dik_SphereChainWithSupremeBlastAndRebirth();
void dik_BuildPowerUp(char *bitmapName);
void dik_DeconstructPowerUp();
void dik_SupremeBlastPowerUp();
void dik_RebirthPowerUp();
void dik_InvisibilityPowerUp();
void dik_InitialisePostApocalypticGarderOfEden();
void dik_PostApocalypticGardenOfEden();
void dik_DeconstructApocalypticGarderOfEden();
void dik_InitialiseWaterWave();
void DisablePriorOpenGLCommands();
void GameLoopBody(HWND wnd);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR     lpCmdLine,
                   int       nCmdShow = SW_SHOW)
{
	HWND hwnd;
	MSG msg;
	WNDCLASSEX wcl;
	RECT windowRect;
	DWORD dwExStyle;		
	DWORD dwStyle;
	int mbResponse;

// To remind myself if I'm using the debug build!
#ifdef _DEBUG
	MessageBox(NULL, "This is the DEBUG build!", "DEBUG!", MB_OK);
#endif

	mbResponse = MessageBox(NULL, "Would you like to play in windowed mode?", "AntiVirus 2 Game Demo Display Mode", MB_ICONQUESTION | MB_YESNOCANCEL | MB_DEFBUTTON1);

	switch(mbResponse)
	{
		case IDYES:
			windowed = true;
			break;
		case IDNO:
			windowed = false;
			break;
		case IDCANCEL:
			exit(0);
			break;
		default:
			break;
	}

	// Define a window class. 
	wcl.cbSize = sizeof(WNDCLASSEX); 
	wcl.hInstance = hInstance;     
	wcl.lpszClassName = szWinName; 
	wcl.lpfnWndProc = WindowFunc;  
	wcl.style = CS_HREDRAW | CS_VREDRAW;
	wcl.hCursor = (HCURSOR) LoadImage(hInstance, MAKEINTRESOURCE(IDC_CURSOR1), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);
	wcl.hIcon = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
	wcl.lpszMenuName = NULL; 
	wcl.cbClsExtra = 0; 
	wcl.cbWndExtra = 0; 
	wcl.hbrBackground = reinterpret_cast <HBRUSH> (GetStockObject(BLACK_BRUSH)); 

	// Adjust size in pixels for performance reasons.
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = 800;
	windowRect.bottom = 600;

	// Register the window class. 
	if(!RegisterClassEx(&wcl)) 
		return 0;
	
	if(windowed)
	{
		SwitchToWindowed(hwnd, hInstance);
		ShowWindow(hwnd, SW_SHOW);
	}
	else
	{
		// Switch to full screen.
		if(!SwitchToFullScreen(hwnd, hInstance, windowRect, dwExStyle, dwStyle, windowRect.right, windowRect.bottom, 32))
			MessageBox(NULL, "Switch to full screen failed.", "Full Screen Error...", MB_OK);
		ShowWindow(hwnd, SW_MAXIMIZE);

	}

	ghwnd = hwnd;

	UpdateWindow(hwnd);

	// Start off and initialise game here!

	InitialiseAGUIMenu();
	iFeatureIsInitialised = 0;			// Do not invoke any gameplay modes.

	// Display the splashscreen!
	ShowCursor(false);
	if(windowed)
		SplashScreenInWindow();
	else
		SplashScreen();
	Sleep(4000);
	ShowCursor(true);

	// Initialise DirectSound and DiectInput components of DirectX
	if(!(DirectSoundInitialisation() && DirectInputDevicesInitialisation(hInstance)))
	{
		MessageBox(hwnd, "Program shutting down.", "DirectX Unrecoverable Error", MB_OK | MB_ICONINFORMATION);
		DeleteDirectSoundResources();
		DeleteDirectInputResources();
		exit(0);
	}

	// Fire up the menu beat!
	PlayMenuMusic();

	// Initialise player position in the environment.
	SphereObjectCoords[0].xcoord = 780.0f;
	SphereObjectCoords[0].ycoord = 270.0f;
	SphereObjectCoords[0].zcoord = -500.0f;

	FetchCurrentTime();

	ZeroMemory(&msg, sizeof(MSG));

	while(msg.message != WM_DESTROY)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);  
		}
		else
			if(CheckTimeDelta())
				GameLoopBody(hwnd);
	}

	ChangeDisplaySettings(NULL, 0);		// Restore original screen resolution.

	return (int) msg.wParam;
}

LRESULT CALLBACK WindowFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HGLRC hRC;
	HDC hDC = NULL;
	int width, height;
	int x_screenLocation, y_screenLocation;
	static bool isMenu = true;

	const char RESHEADER[] = "Resolution";
	const char RESSAVEDHEADER[] = "Resolution Save...";
	const char RESWRTFAIL[] = "Game needs to be run from hard drive or other writeable media.";

	switch(message) {
		case WM_CREATE:
			hDC = GetDC(hwnd);			
			g_HDC = hDC;
			SetupPixelFormat(hDC);		
			hRC = wglCreateContext(hDC);
			wglMakeCurrent(hDC, hRC);

			// Initialise sphere chain to avoid crashes.
			for(counter = 0; counter < CHAINMAXLENGTH; counter++)
				ASphere[counter] = NULL;
			break;
		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_ESCAPE:
					switch(iFeatureIsInitialised)
					{
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							currentMenu = 2;
							PostMessage(hwnd, ID_GAME_MENU, wParam, lParam);
							break;
						case 6:
						case 7:
						case 8:
							currentMenu = 3;
							PostMessage(hwnd, ID_GAME_MENU, wParam, lParam);
							break;
						case 9:
							currentMenu = 0;
							PostMessage(hwnd, ID_GAME_MENU, wParam, lParam);
							break;
						default:
							break;
					}
					break;
				case VK_F1:
					if(!isMenu)
					{
						if(WriteResFile(640, 480))
							MessageBox(NULL, "Game will now play at 640 x 480 resolution.", RESHEADER, MB_OK);
						else
							MessageBox(NULL, RESWRTFAIL, RESSAVEDHEADER, MB_OK);
						if(!CheckCurrentResolution(640, 480))
							ChangeScreenResolution(640, 480);
					}
					break;
				case VK_F2:
					if(!isMenu)
					{
						if(WriteResFile(800, 600))
							MessageBox(NULL, "Game will now play at 800 x 600 resolution.", RESHEADER, MB_OK);
						else
							MessageBox(NULL, RESWRTFAIL, RESSAVEDHEADER, MB_OK);
						if(!CheckCurrentResolution(800, 600))
							ChangeScreenResolution(800, 600);
					}
					break;
				case VK_F3:
					if(!isMenu)
					{
						if(WriteResFile(1024, 768))
							MessageBox(NULL, "Game will now play at 1024 x 768 resolution.", RESHEADER, MB_OK);
						else
							MessageBox(NULL, RESWRTFAIL, RESSAVEDHEADER, MB_OK);
						if(!CheckCurrentResolution(1024, 768))
							ChangeScreenResolution(1024, 768);
					}
					break;
				case VK_F4:
					if(!isMenu)
					{
						if(WriteResFile(1280, 1024))
							MessageBox(NULL, "Game will now play at 1280 x 1024 resolution.", RESHEADER, MB_OK);
						else
							MessageBox(NULL, RESWRTFAIL, RESSAVEDHEADER, MB_OK);
						if(!CheckCurrentResolution(1280, 1024))
							ChangeScreenResolution(1280, 1024);
					}
					break;
				default:
					break;
			}
			break;
		case WM_LBUTTONDOWN:
			Sleep(400);			// Retains mouse coords for 2/5 of a second to prevent filtering through to last menu.
			x_screenLocation = LOWORD(lParam); y_screenLocation = HIWORD(lParam);
			g_x_region = x_screenLocation; g_y_region = y_screenLocation;
			g_wParam = wParam; g_lParam = lParam;
			break;
		case ID_GAME_MENU:
			if(!CheckCurrentResolution(800, 600) && !windowed)
				ChangeScreenResolution(800, 600);
			DisablePriorOpenGLCommands();
			PlaySound(NULL, NULL, SND_NODEFAULT); // Stop previous sound!
			InitialiseAGUIMenu();
			PlayMenuMusic();
			iFeatureIsInitialised = 0;
			ShowCursor(true);
			isMenu = true;
			break;
		case SPHR_STANDARD:
			if(!windowed)
				FetchResolution();
			DisablePriorOpenGLCommands();
			dik_BuildStandardSphereChain();
			PlayStandardMusic();
			iFeatureIsInitialised = 1;
			ShowCursor(false);
			isMenu = false;
			break;
		case SPHR_SUPBLAST:
			if(!windowed)
				FetchResolution();
			DisablePriorOpenGLCommands();
			dik_BuildSphereChainWithPowerUp(0.0f, 0.0f, 0.7f, "supblast.bmp");
			PlaySupremeBlastMusic();
			iFeatureIsInitialised = 2;
			ShowCursor(false);
			isMenu = false;
			break;
		case SPHR_REBIRTH:
			if(!windowed)
				FetchResolution();
			DisablePriorOpenGLCommands();
			dik_BuildSphereChainWithPowerUp(0.7f, 0.0f, 0.0f, "rebirth.bmp");
			PlayRebirthMusic();
			iFeatureIsInitialised = 3;
			ShowCursor(false);
			isMenu = false;
			break;
		case SPHR_SBREBTH:
			if(!windowed)
				FetchResolution();
			DisablePriorOpenGLCommands();
			dik_BuildSphereChainWithPowerUp(0.7f, 0.0f, 0.7f, "sbrebirth.bmp");
			PlaySupaFlyMusic();
			iFeatureIsInitialised = 4;
			ShowCursor(false);
			isMenu = false;
			break;
		case SPHR_INVISIBL:
			if(!windowed)
				FetchResolution();
			DisablePriorOpenGLCommands();
			dik_BuildSphereChainWithPowerUp(0.7f, 0.0f, 0.7f, "invisibility.bmp");
			PlayInvisibilityMusic();
			iFeatureIsInitialised = 5;
			ShowCursor(false);
			isMenu = false;
			break;
		case PU_SUPBLAST:
			if(!windowed)
				FetchResolution();
			DisablePriorOpenGLCommands();
			dik_BuildPowerUp("supblast.bmp");
			PlaySupremeBlastMusic();
			iFeatureIsInitialised = 6;
			ShowCursor(false);
			isMenu = false;
			break;
		case PU_REBIRTH:
			if(!windowed)
				FetchResolution();
			DisablePriorOpenGLCommands();
			dik_BuildPowerUp("rebirth.bmp");
			PlayRebirthMusic();
			iFeatureIsInitialised = 7;
			ShowCursor(false);
			isMenu = false;
			break;
		case PU_INVISIBL:
			if(!windowed)
				FetchResolution();
			DisablePriorOpenGLCommands();
			dik_BuildPowerUp("invisibility.bmp");
			PlayInvisibilityMusic();
			iFeatureIsInitialised = 8;
			ShowCursor(false);
			isMenu = false;
			break;
		case ENV_PAGOE:
			if(!windowed)
				FetchResolution();
			DisablePriorOpenGLCommands();
			dik_InitialisePostApocalypticGarderOfEden();
			PlayPAGoEMusic();
			iFeatureIsInitialised = 9;
			ShowCursor(false);
			isMenu = false;
			break;
		case WM_SIZE:
			height = HIWORD(lParam);
			width = LOWORD(lParam);

			if(height == 0)				
				height = 1;
			
			glViewport(0, 0, width, height);
			glMatrixMode(GL_PROJECTION);	
			glLoadIdentity();			
			
			gluPerspective(54.0f, (GLfloat) width/(GLfloat) height, 1.0f, 3000.0f);

			glMatrixMode(GL_MODELVIEW);	
			glLoadIdentity();			

			return 0;
			break;
		case WM_DESTROY:
			CleanUpLoadTextures();
			FreeChainMemory();
			DeleteDirectSoundResources();
			DeleteDirectInputResources();
			DisablePriorOpenGLCommands();
			DestroyFont();
			wglMakeCurrent(hDC, NULL);
			wglDeleteContext(hRC);
			PostQuitMessage(0);
			break;
		case WM_QUIT:			
			DestroyWindow(hwnd);	
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
			break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void SetupPixelFormat(HDC hDC)
{
	int nPixelFormat;					

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	
		1,								
		PFD_DRAW_TO_WINDOW |			
		PFD_SUPPORT_OPENGL |			
		PFD_DOUBLEBUFFER,				
		PFD_TYPE_RGBA,					
		32,								
		0, 0, 0, 0, 0, 0,				
		0,								
		0,								
		0,								
		0, 0, 0, 0,						
		16,								
		0,								
		0,								
		PFD_MAIN_PLANE,					
		0,								
		0, 0, 0};						

	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	SetPixelFormat(hDC, nPixelFormat, &pfd);
}

void FreeChainMemory()
{
	// Free up memory of quadric structures
	if(chainUsed)
		for(counter = 0; counter < CHAINMAXLENGTH; counter++) 
			gluDeleteQuadric(ASphere[counter]);
}

void dik_BuildStandardSphereChain()
{
	// The standard sphere chain.
	accumulator = 0.0f;
	SetEnvironmentSpotlight();

	factorR = 0.0f;			
	factorG = INITIALTRANSFORMATIONCOLOURS;
	factorB = 0.0f;

	for(counter = 0; counter < CHAINMAXLENGTH; counter++)
		if(ASphere[counter] == NULL)
		{
			ASphere[counter] = gluNewQuadric();

			if(ASphere[counter] == NULL)	// If it's still not allocated.
			{
				MessageBox(NULL, "Memory allocation failed for chain.", "Error: Chain", MB_OK);
				break;
			}
		}

	glEnable(GL_TEXTURE_2D);

	sphereTexture = LoadBitmapFile("standard.bmp", &textureInfo[0]);
	BindTexturesAndSetMipmaps();

	FixBackdropTexture();
}

void dik_StandardSphereChain()
{
	float x_motion;
	float y_motion;

	InitialiseTransparents();

	sineAngle += 0.07f;
	angle += 1.5f;
	if(angle >= 360.0f)
		angle = 0.0f;
	if(sineAngle >= 360.0f)
		sineAngle = 0.0f;

	glBindTexture(GL_TEXTURE_2D, backdropTexture);

	for(counter = 0; counter < CHAINMAXLENGTH; counter++)
	{
		x_motion = static_cast <float> (-counter);
		y_motion = chainBounce[counter];

		x_motion += XTRAVELSTART;

		RenderSphere(x_motion, y_motion, -10.0f, ASphere[counter], &textureGeneration, 1.0f, RenderBackdrop);
	}

/*	Preceding for loop follows this pattern...
	RenderSphere(xTravel, bounce[0], -7.0f, ASphere[0]);
	RenderSphere(-1.0f+xTravel, bounce[1], -10.0f, ASphere[1]);
	RenderSphere(-2.0f+xTravel, bounce[2], -10.0f, ASphere[2]); */

	ConstructModelChainMotion();
	glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

	glFlush();
	SwapBuffers(g_HDC);
}

void dik_DeconstructStandardSphereChain()
{
	DisableSphereOGLFunctions();

	accumulator = 0.0f;
	sineAngle = 0.0f;
	animationCounter = ANIMATIONSTART;
	chainUsed = true;
}

void dik_BuildSphereChainWithPowerUp(GLfloat factorR, GLfloat factorG, GLfloat factorB, char* outerTexture)
{
	// Supreme Blast sphere.
	accumulator = 0.0f;
	SetEnvironmentSpotlight();

	for(counter = 0; counter < CHAINMAXLENGTH; counter++)
		if(ASphere[counter] == NULL)
		{
			ASphere[counter] = gluNewQuadric();

			if(ASphere[counter] == NULL)	// If it's still not allocated.
			{
				MessageBox(NULL, "Memory allocation failed for chain.", "Error: Chain", MB_OK);
				break;
			}
		}

	glEnable(GL_TEXTURE_2D);

	sphereTexture = LoadBitmapFile("standard.bmp", &textureInfo[0]);
	BindTexturesAndSetMipmaps();

	quadSphereTexture = LoadBitmapFile(outerTexture, &textureInfo[1]);
	BindTexturesAndSetMipmapsForPowerUp();

	FixBackdropTexture();
}

void dik_DeconstructSphereChainWithPowerUp()
{
	DisableSphereOGLFunctions();

	accumulator = 0.0f;
	sineAngle = 0.0f;
	animationCounter = ANIMATIONSTART;
	soundPlayed = false;
	chainUsed = true;
}

void dik_SphereChainWithSupremeBlast()
{
	float x_motion;
	float y_motion;

	InitialiseTransparents();

	sineAngle += 0.07f;
	angle += 1.5f;
	if(angle >= 360.0f)
		angle = 0.0f;
	if(sineAngle >= 360.0f)
		sineAngle = 0.0f;

	for(counter = 0; counter < CHAINMAXLENGTH; counter++)
	{
		x_motion = static_cast <float> (-counter);
		y_motion = chainBounce[counter];

		x_motion += XTRAVELSTART;		// static_cast <float> (-counter) + XTRAVELSTART

		if(animationCounter >= 1.0f)
			RenderSphere(x_motion, y_motion, -10.0f, ASphere[counter], &textureGeneration, 1.0f, RenderBackdrop);
		else
		{
			if(!soundPlayed)
			{
				factorR = -0.5f;		// Change colours and play sound once.
				factorG = -0.5f;
				factorB = 1.0f;
				lpDSBSounds[0]->GetStatus(&dwStatus);
				if(dwStatus & DSBSTATUS_BUFFERLOST)
					RestoreDSBuffers();
				else
				{
					lpDSBSounds[0]->SetFrequency(37500);
					lpDSBSounds[0]->SetVolume(DSBVOLUME_MAX);
					if (!(dwStatus & DSBSTATUS_PLAYING))
						lpDSBSounds[0]->Play(0, 0, 0);
					else
					if(dwStatus & DSBSTATUS_PLAYING)
						lpDSBSounds[0]->Stop();
					soundPlayed = true;
				}
			}
			RenderSphereWithPowerUp(static_cast <float> (-counter) + XTRAVELSTART, chainBounce[counter], -10.0f, ASphere[counter], &textureGeneration, &quadTextureGeneration, SCALING * animationCounter, RenderBackdrop);
		}
	}
	
	ConstructModelChainMotion();

	glFlush();
	SwapBuffers(g_HDC);
}

void dik_SphereChainWithRebirth()
{
	float x_motion;
	float y_motion;

	InitialiseTransparents();

	sineAngle += 0.07f;
	angle += 1.5f;
	if(angle >= 360.0f)
		angle = 0.0f;
	if(sineAngle >= 360.0f)
		sineAngle = 0.0f;

	for(counter = 0; counter < CHAINMAXLENGTH; counter++)
	{
		x_motion = static_cast <float> (-counter);
		y_motion = chainBounce[counter];

		x_motion += XTRAVELSTART;

		if(animationCounter >= 1.0f)
			RenderSphere(x_motion, y_motion, -10.0f, ASphere[counter], &textureGeneration, 1.0f, RenderBackdrop);
		else
		{
			if(!soundPlayed)
			{
				factorR = 1.0f;
				factorG = -0.5f;
				factorB = -0.5f;
				lpDSBSounds[1]->GetStatus(&dwStatus);
				if(dwStatus & DSBSTATUS_BUFFERLOST)
					RestoreDSBuffers();
				else
				{
					lpDSBSounds[1]->SetFrequency(37500);
					lpDSBSounds[1]->SetVolume(DSBVOLUME_MAX);
					if (!(dwStatus & DSBSTATUS_PLAYING))
						lpDSBSounds[1]->Play(0, 0, 0);
					else
					if(dwStatus & DSBSTATUS_PLAYING)
						lpDSBSounds[1]->Stop();
					soundPlayed = true;
				}
			}
			RenderSphereWithPowerUp(x_motion, y_motion, -10.0f, ASphere[counter], &textureGeneration, &quadTextureGeneration, SCALING * animationCounter, RenderBackdrop);
		}
	}

	ConstructModelChainMotion();

	glFlush();
	SwapBuffers(g_HDC);
}

void dik_SphereChainWithInvisibility()
{
	float x_motion;
	float y_motion;

	InitialiseTransparents();

	sineAngle += 0.07f;
	angle += 1.5f;
	if(angle >= 360.0f)
		angle = 0.0f;
	if(sineAngle >= 360.0f)
		sineAngle = 0.0f;

	for(counter = 0; counter < CHAINMAXLENGTH; counter++)
	{
		x_motion = static_cast <float> (-counter);
		y_motion = chainBounce[counter];

		x_motion += XTRAVELSTART;

		if(animationCounter >= 1.0f)
			RenderSphere(x_motion, y_motion, -10.0f, ASphere[counter], &textureGeneration, 1.0f, RenderBackdrop);
		else
		{
			if(!soundPlayed)
			{
				factorR = 1.0f;
				factorG = 1.0f;
				factorB = 1.0f;
				lpDSBSounds[2]->GetStatus(&dwStatus);
				if(dwStatus & DSBSTATUS_BUFFERLOST)
					RestoreDSBuffers();
				else
				{
					lpDSBSounds[2]->SetFrequency(37500);
					lpDSBSounds[2]->SetVolume(DSBVOLUME_MAX);
					if (!(dwStatus & DSBSTATUS_PLAYING))
						lpDSBSounds[2]->Play(0, 0, 0);
					else
					if(dwStatus & DSBSTATUS_PLAYING)
						lpDSBSounds[2]->Stop();
					soundPlayed = true;
				}
			}
			RenderSphere(x_motion, y_motion, -10.0f, ASphere[counter], &quadTextureGeneration, animationCounter/25.0f, RenderBackdrop);
		}
	}

	ConstructModelChainMotion();

	glFlush();
	SwapBuffers(g_HDC);

}

void dik_SphereChainWithSupremeBlastAndRebirth()
{
	float x_motion;
	float y_motion;

	InitialiseTransparents();

	sineAngle += 0.07f;
	angle += 1.5f;
	if(angle >= 360.0f)
		angle = 0.0f;
	if(sineAngle >= 360.0f)
		sineAngle = 0.0f;

	for(counter = 0; counter < CHAINMAXLENGTH; counter++)
	{
		x_motion = static_cast <float> (-counter);
		y_motion = chainBounce[counter];

		x_motion += XTRAVELSTART;

		if(animationCounter >= 1.0f)
			RenderSphere(x_motion, y_motion, -10.0f, ASphere[counter], &textureGeneration, 1.0f, RenderBackdrop);
		else
		{
			if(!soundPlayed)
			{
				factorR = 1.0f;
				factorG = 0.5f;
				factorB = 0.5f;
				lpDSBSounds[2]->GetStatus(&dwStatus);
				if(dwStatus & DSBSTATUS_BUFFERLOST)
					RestoreDSBuffers();
				else
				{
					lpDSBSounds[3]->SetFrequency(37500);
					lpDSBSounds[3]->SetVolume(DSBVOLUME_MAX);
					if (!(dwStatus & DSBSTATUS_PLAYING))
						lpDSBSounds[3]->Play(0, 0, 0);
					else
					if(dwStatus & DSBSTATUS_PLAYING)
						lpDSBSounds[3]->Stop();

					soundPlayed = true;
				}
			}
			RenderSphereWithPowerUp(x_motion, y_motion, -10.0f, ASphere[counter], &textureGeneration, &quadTextureGeneration, SCALING * animationCounter, RenderBackdrop);
		}
	}

	ConstructModelChainMotion();

	glFlush();
	SwapBuffers(g_HDC);
}

void dik_BuildPowerUp(char *bitmapName)
{
	SetSphereSpotlight();

	texturedSphere = gluNewQuadric();				// Create new quadric.
	gluQuadricTexture(texturedSphere, GL_TRUE);		// Allow sphere to be texture mapped.
	glEnable(GL_TEXTURE_2D);
	sphereTexture = LoadBitmapFile(bitmapName, &textureInfo[0]);
	BindTexturesAndSetMipmaps();

	FixBackdropTexture();
}

void dik_DeconstructPowerUp()
{
	DisableTransparentsOGLFunctions();
	gluDeleteQuadric(texturedSphere);
}

void dik_SupremeBlastPowerUp()
{
	// Supreme Blast power-up.
	InitialiseTransparents();
	RenderPowerUp(Bouncer, 0.0f, 0.0f, 1.0f, texturedSphere, RenderBackdrop);

	glFlush();
	SwapBuffers(g_HDC);
}

void dik_RebirthPowerUp()
{
	// Rebirth power-up.
	InitialiseTransparents();
	RenderPowerUp(Bouncer, 1.0f, 0.0f, 0.0f, texturedSphere, RenderBackdrop);

	glFlush();
	SwapBuffers(g_HDC);
}

void dik_InvisibilityPowerUp()
{
	// Invisibility power-up.
	InitialiseTransparents();
	RenderPowerUp(Bouncer, 0.0f, 0.0f, 0.0f, texturedSphere, RenderBackdrop);

	glFlush();
	SwapBuffers(g_HDC);
}

void dik_InitialisePostApocalypticGarderOfEden()
{
	int i;			// Indexer for 'for loops' in this function.

	SetEnvironmentSpotlight();

	InitialiseTransparents();

	// Initialise scoreboard font.
	AllocateHeapForScoreboard();
	g_FontListID = CreateOpenGL3DFont("Impact", FONT_EXTRUDE);

	// Load graphics files.
	powerUpImageData[0] = LoadBitmapFile("standard.bmp", &powerUpBitmapInfoHeader[0]);
	powerUpTexture[0] = LoadBitmapFile("standard.bmp", &powerUpBitmapInfo[0]);
	powerUpImageData[1] = LoadBitmapFile("supblast.bmp", &powerUpBitmapInfoHeader[1]);
	powerUpTexture[1] = LoadBitmapFile("supblast.bmp", &powerUpBitmapInfo[1]);
	powerUpImageData[2] = LoadBitmapFile("rebirth.bmp", &powerUpBitmapInfoHeader[2]);
	powerUpTexture[2] = LoadBitmapFile("rebirth.bmp", &powerUpBitmapInfo[2]);
	powerUpImageData[3] = LoadBitmapFile("sbrebirth.bmp", &powerUpBitmapInfoHeader[3]);
	powerUpTexture[3] = LoadBitmapFile("sbrebirth.bmp", &powerUpBitmapInfo[3]);
	powerUpImageData[4] = LoadBitmapFile("invisibility.bmp", &powerUpBitmapInfoHeader[4]);
	powerUpTexture[4] = LoadBitmapFile("invisibility.bmp", &powerUpBitmapInfo[4]);

	BindTexturesAndSetMipmapsForPowerUpsInEnvironments();

	glEnable(GL_TEXTURE_2D);

	// Allocate power up objects and allow them to be texture mapped.
	for(i = 0; i < NUMEFFECTS; i++)
	{
		powerUpSphere[i] = gluNewQuadric();
		if(powerUpSphere[i] == NULL)
			MessageBox(ghwnd, "Failed to allocate power up object for PAGoE!", "PAGoE Population Error...", MB_OK | MB_ICONEXCLAMATION); // If out of memory.
		else
			gluQuadricTexture(powerUpSphere[i], GL_TRUE);
	}

	// Claim all memory required to render all players.
	for(i = 0; i < MAXPLAYERCHAINLENGTH; i++)
	{
		player1Chain[i] = gluNewQuadric();
		player2Chain[i] = gluNewQuadric();
		player3Chain[i] = gluNewQuadric();

		if((player1Chain[i] == NULL) || (player2Chain[i] == NULL) || (player3Chain[i] == NULL))
			MessageBox(ghwnd, "Failed to allocate all player models for PAGoE!", "PAGoE Population Error...", MB_OK | MB_ICONEXCLAMATION); // If out of memory.
		else
		{
			gluQuadricTexture(player1Chain[i], GL_TRUE);
			gluQuadricTexture(player2Chain[i], GL_TRUE);
			gluQuadricTexture(player3Chain[i], GL_TRUE);
		}
	}

	// Initialise marker spere for player
	markerSphere = gluNewQuadric();
	if(markerSphere == NULL)
		MessageBox(ghwnd, "Failed to allocate marker for player.", "PAGoE Population Error...", MB_OK | MB_ICONEXCLAMATION);

	// Return to processing for environment.
	imageData = LoadBitmapFile("KBGrass.bmp", &bitmapInfoHeader);
	landTexture = LoadBitmapFile("KBGrass.bmp", &landInfo);

	InitialiseTerrain();
	LoadTextures();

	if(!LoadTextures())
		MessageBox(ghwnd, "Texture(s) could not be loaded.", "Post Apocalyptic Garden of Eden", MB_ICONEXCLAMATION);
}

void dik_PostApocalypticGardenOfEden()
{

	RenderPostApocalypticGardenOfEden(land, NULL, Bouncer, markerSphere, player1Chain, player2Chain, player3Chain, SphereObjectCoords[0],
										bLineUpOnLongitudeUp, bLineUpOnLongitudeDown, bLineUpOnLatitudeL, bLineUpOnLatitudeR, ComputeNextSpherePosition, 1.0f);
}

void dik_DeconstructApocalypticGarderOfEden()
// Free up memory and reset OpenGL environment.
{
	int i;

	// Free memory consumed by power up objects assigned locations.
	for(i = 0; i < NUMEFFECTS; i++)
		if(powerUpSphere[i] != NULL)
			gluDeleteQuadric(powerUpSphere[i]);

	// Free memory taken by player characters.
	for(i = 0; i < MAXPLAYERCHAINLENGTH; i++)
	{
		if(player1Chain[i] != NULL)
			gluDeleteQuadric(player1Chain[i]);
		if(player2Chain[i] != NULL)
			gluDeleteQuadric(player2Chain[i]);
		if(player3Chain[i] != NULL)
			gluDeleteQuadric(player3Chain[i]);
	}

	DisableTransparentsOGLFunctions();
	DestroyFont();
}

void DisablePriorOpenGLCommands()
// Clean up previous OpenGL setup.
{
	factorR = 0.0f;			
	factorG = INITIALTRANSFORMATIONCOLOURS;
	factorB = 0.0f;

	switch(iFeatureIsInitialised)
	{
		case 0:
			MenuCleanUp();
		case 1:
			dik_DeconstructStandardSphereChain();
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			dik_DeconstructSphereChainWithPowerUp();
			break;
		case 6:
		case 7:
		case 8:
			dik_DeconstructPowerUp();
			break;
		case 9:
			dik_DeconstructApocalypticGarderOfEden();
			break;
		default:
			break;
	}

	iFeatureIsInitialised = 0;	// Reinitialise feature ID.
}

inline void GameLoopBody(HWND wnd)
{
		if(pKeyboard->GetDeviceState(KEYBUFCOUNT * sizeof(keyboardBuffer[0]), (LPVOID) &keyboardBuffer) == DIERR_INPUTLOST)
		{
			if(pKeyboard->Acquire() == DI_OK)
				pKeyboard->GetDeviceState(KEYBUFCOUNT * sizeof(keyboardBuffer[0]), (LPVOID) &keyboardBuffer);
		}

		if(rotationAngle <= 0.0f)
			rotationAngle = 360.0f;
		else
			rotationAngle -= 0.1f;

		// Get keyboard input
		if(KEYDOWN(keyboardBuffer, DIK_LEFT) || KEYDOWN(keyboardBuffer, DIK_A))
			keyPressed = 1;
		if(KEYDOWN(keyboardBuffer, DIK_RIGHT) || KEYDOWN(keyboardBuffer, DIK_D))
			keyPressed = 2;
		if(KEYDOWN(keyboardBuffer, DIK_UP) || KEYDOWN(keyboardBuffer, DIK_W))
			keyPressed = 3;
		if(KEYDOWN(keyboardBuffer, DIK_DOWN) || KEYDOWN(keyboardBuffer, DIK_S))
			keyPressed = 4;

		if(keyPressed == 1)
		{
			if(SphereObjectCoords[0].xcoord <= X_MOTION_LIMIT_LOWER)
				SphereObjectCoords[0].xcoord = X_MOTION_LIMIT_LOWER;
			else
				SphereObjectCoords[0].xcoord -= MOVE_RATE;

			bLineUpOnLongitudeUp = false;
			bLineUpOnLongitudeDown = false;
			bLineUpOnLatitudeR = false;
			bLineUpOnLatitudeL = true;					
		}


		if(keyPressed == 2)
		{	
			if(SphereObjectCoords[0].xcoord >= X_MOTION_LIMIT_UPPER)
				SphereObjectCoords[0].xcoord = X_MOTION_LIMIT_UPPER;
			else
				SphereObjectCoords[0].xcoord += MOVE_RATE;

			bLineUpOnLongitudeUp = false;
			bLineUpOnLongitudeDown = false;
			bLineUpOnLatitudeL = false;
			bLineUpOnLatitudeR = true;
		}	

		if(keyPressed == 3)
		{	
			if(SphereObjectCoords[0].zcoord <= Z_MOTION_LIMIT_UPPER)
				SphereObjectCoords[0].zcoord = Z_MOTION_LIMIT_UPPER;
			else
				SphereObjectCoords[0].zcoord -= MOVE_RATE;
				
			bLineUpOnLongitudeUp = false;
			bLineUpOnLongitudeDown = true;
			bLineUpOnLatitudeR = false;
			bLineUpOnLatitudeL = false;
		}	

		if(keyPressed == 4)
		{
			if(SphereObjectCoords[0].zcoord >= Z_MOTION_LIMIT_LOWER)
				SphereObjectCoords[0].zcoord = Z_MOTION_LIMIT_LOWER;
			else
				SphereObjectCoords[0].zcoord += MOVE_RATE;
				
			bLineUpOnLongitudeUp = true;
			bLineUpOnLongitudeDown = false;
			bLineUpOnLatitudeR = false;
			bLineUpOnLatitudeL = false;
		}

	// Render game/model view.
	switch(iFeatureIsInitialised)
	{
		case 0:
			DisplayMenu(currentMenu, windowed);
			currentMenuOption = FindMenuRegionClicked(currentMenu, g_x_region, g_y_region, windowed);
			ZeroMouseCoords(&g_x_region, &g_y_region);
			SendMenuOptionToMessageQueue(wnd, g_wParam, g_lParam, (int &) currentMenu, currentMenuOption);
			break;
		case 1:
			dik_StandardSphereChain();
			break;
		case 2:
			dik_SphereChainWithSupremeBlast();
			break;
		case 3:
			dik_SphereChainWithRebirth();
			break;
		case 4:
			dik_SphereChainWithSupremeBlastAndRebirth();
			break;
		case 5:
			dik_SphereChainWithInvisibility();
			break;
		case 6:
			dik_SupremeBlastPowerUp();
			break;
		case 7:
			dik_RebirthPowerUp();
			break;
		case 8:
			dik_InvisibilityPowerUp();
			break;
		case 9:
			dik_PostApocalypticGardenOfEden();
			break; 
		default:
			break;
	} 
				
}
