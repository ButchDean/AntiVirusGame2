/*!\file Scoreboard.h
 * \brief Function prototypes.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

#ifndef _SCOREBOARD_
#define _SCOREBOARD_

#include <windows.h>
#include <stdio.h>
#include <gl/glu.h>

unsigned int CreateOpenGL3DFont(LPSTR strFontName, float extrude);
void Draw3DText(const char *strString);
void AllocateHeapForScoreboard();
void DisplayScoreboard(GLfloat xPos, GLfloat yPos, GLfloat zPos, int p1score, int p2score, int p3score);
void DisplayYouWinYouLose(GLfloat xPos, GLfloat yPos, GLfloat zPos, bool YouWin);
void DestroyFont();

#endif