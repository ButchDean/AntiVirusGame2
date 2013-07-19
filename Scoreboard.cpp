/*!\file Scoreboard.cpp
 * \brief The scoreboard seen in the main game is implemented here.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

#include <stdlib.h>
#include <string.h>
#include "Scoreboard.h"

/*! Total characters in character set. */
const int	MAX_CHARS =	256;
/*! For 3D font rendered in the game, this is how much they extrude along the z-axis of their local coordidates. */
const float FONT_EXTRUDE = 0.4f;

/*! This is the global handle for the window. */
HWND  g_hWnd;
/*! This is a global variable that holds the window dimensions. */
RECT  g_rRect;
extern HDC g_HDC;
/*! General OpenGL_DC - Our Rendering Context for OpenGL. */
HGLRC g_hRC;
/*! This holds our window hInstance. */
HINSTANCE g_hInstance;

/*! Global font list handle set to NULL. */
UINT g_FontListID = 0;
/*! Intermediate handle to a font. */
HFONT hOldFont;
/*! The array that will hold the 3D font info for each character. */
GLYPHMETRICSFLOAT g_GlyphInfo[MAX_CHARS];

// Used for scoreboard text displays.
/*! Character spaces to diaply score on scoreboard. */
const int CHARS = 6;
/*! the base used to represent numbers for the scoreboard. */
const int RADIX = 10;
/*! The number of players in the game, minus 1 and the AI are grouped as one character for the score. */
const int PLAYERS = 2;
/*! The number for the total characters for scoreboard - one per line. */
const int TOTAL_STRING_LENGTH = 50;
/*! Pointer to string holding the score. */
char *numString[PLAYERS];
/*! Pointer to the scoreboard string. */
char *stringBuffer[PLAYERS];


unsigned int CreateOpenGL3DFont(LPSTR strFontName, float extrude)	
{
	UINT	fontListID = 0;								
	HFONT	hFont;										

	fontListID = glGenLists(MAX_CHARS);					

	hFont = CreateFont(	0,								
						0,								
						0,								
						0,								
						FW_BOLD,						
						true,							
						false,							
						false,							
						ANSI_CHARSET,					
						OUT_TT_PRECIS,					
						CLIP_DEFAULT_PRECIS,			
						ANTIALIASED_QUALITY,			
						FF_DONTCARE|DEFAULT_PITCH,		
						strFontName);					

	hOldFont = (HFONT)SelectObject(g_HDC, hFont);

	wglUseFontOutlines(	g_HDC,							
						0,								
						MAX_CHARS - 1,					
						fontListID,						
						0,								
						FONT_EXTRUDE,					
						WGL_FONT_POLYGONS,				
						g_GlyphInfo);					

	return fontListID;									
}

void Draw3DText(const char *strString)
{
	char		strText[MAX_CHARS];						
	float		unitLength=0.0f;

	// First we need to check if there was even a string given
	if (strString == NULL)								
		return;									

	sprintf(strText, strString, strText);			

	// Below we find out the total length of the characters in 3D units, then center them.

	for (int i=0; i < (int)strlen(strText); i++)		
	{
		unitLength += g_GlyphInfo[strText[i]].gmfCellIncX;	
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);					

	glListBase(g_FontListID);							
	glCallLists((GLsizei) strlen(strText), GL_UNSIGNED_BYTE, strText);

	glPopAttrib();	
}

void AllocateHeapForScoreboard()
{
	int column;

	for(column = 0; column < PLAYERS; column++)
	{
		numString[column] = (char *) calloc(CHARS, sizeof(char));
		stringBuffer[column] = (char *) calloc(TOTAL_STRING_LENGTH, sizeof(char));

		if(numString[column] == NULL || stringBuffer[column] == NULL)
			MessageBox(NULL, "Unable to allocate scorboard!", "Scoreboared Error..", MB_OK);
	}
}

void DisplayScoreboard(GLfloat xPos, GLfloat yPos, GLfloat zPos, int p1score, int p2score, int p3score)
{	
	const int WINNING_SCORE = 5;
	const char title[] = "ANTI-VIRUS GAME";
	const char yourScore[] = "Anti-Virus... ";
	const char enemy1Score[] = "Viruses... ";
	static int twinScore;

	twinScore = p2score + p3score;

	// Notify winner/loser on achieving relevant score.
	if(p1score >= WINNING_SCORE)	// Display player as winner.
	{
		DisplayYouWinYouLose(xPos+5.0f, yPos, zPos, true);
		return;
	}
	else
		if(twinScore >= WINNING_SCORE)	// Display AI as winner.
		{
			DisplayYouWinYouLose(xPos+5.0f, yPos, zPos, false);
			return;
		}

	// Build the strings
	// For Player 1
	itoa(p1score, numString[0], RADIX);
	strcpy(stringBuffer[0], yourScore);
	strcat(stringBuffer[0], numString[0]);

	// For Player 2 (enemy Alpha)
	itoa(twinScore, numString[1], RADIX);
	strcpy(stringBuffer[1], enemy1Score);
	strcat(stringBuffer[1], numString[1]);

	glPushMatrix();
		glTranslatef(xPos, yPos, zPos);					// The position of the overall enviroment.
		glPushMatrix();
			glTranslatef(xPos + 9.5f, yPos + 2.4f, zPos);					
			glColor3f(1.0f, 1.0f, 0.0f);				
			Draw3DText(title);
		glPopMatrix();
		glPushMatrix();
			glColor3f(0.0f, 1.0f, 0.0f);				// player's score display
			glTranslatef(xPos+10.2f, yPos, zPos);
			Draw3DText(stringBuffer[0]);
		glPopMatrix();
		glPushMatrix();
			glColor3f(1.0f, 0.0f, 0.0f);				// Enemy score display
			glTranslatef(xPos+10.5f, yPos - 1.7f, zPos);
			Draw3DText(stringBuffer[1]);
		glPopMatrix();
	glPopMatrix();		
}

void DisplayYouWinYouLose(GLfloat xPos, GLfloat yPos, GLfloat zPos, bool YouWin)
{
	if(YouWin)
	{
		glPushMatrix();
			glTranslatef(xPos, yPos, zPos);					// The position of the overall enviroment.
			glColor3f(0.0f, 1.0f, 0.0f);				
			glTranslatef(xPos, yPos, zPos);
			Draw3DText("You WIN!!");
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
			glTranslatef(xPos, yPos, zPos);					// The position of the overall enviroment.
			glColor3f(1.0f, 0.0f, 0.0f);				
			glTranslatef(xPos, yPos, zPos);
			Draw3DText("You lose.");
		glPopMatrix();
	}
}

void DestroyFont()										
{
	glDeleteLists(g_FontListID, MAX_CHARS);				// Free the display list
	SelectObject(g_HDC, hOldFont);						// Select the old font back in so we don't have memory leaks
}
