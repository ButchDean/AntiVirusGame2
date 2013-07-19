/*!\file GUIMenu.cpp
 * \brief The GUI menu implementation and management.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "GUIMenu.h"
#include "ScreenResolutionManagement.h"
#include "GameMessages.h"

extern HDC g_HDC;

/*! Display list base. */
unsigned int listBase = 0;
/*! Holds orientation and placement info for display lists. */
GLYPHMETRICSFLOAT gmf[256];

void ZeroMouseCoords(int *x, int *y)
{
	*x = 0;
	*y = 0;
}

unsigned int CreateOutlineFont(char *fontName, int fontSize, float depth)
{
	HFONT hFont;         // windows font
	unsigned int base = 0;

	base = glGenLists(256);  

	if (stricmp(fontName, "symbol") == 0)
	{
	     hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, SYMBOL_CHARSET, 
							OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							FF_DONTCARE | DEFAULT_PITCH, fontName);
	}
	else
	{
		 hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
							OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							FF_DONTCARE | DEFAULT_PITCH, fontName);
	}

	if (!hFont)
		return 0;

	SelectObject(g_HDC, hFont);
	wglUseFontOutlines(g_HDC, 0, 255, base, 0.0f, depth, WGL_FONT_POLYGONS, gmf);

	return base;
}

void ClearFont(unsigned int base)
{
	if(base)
		glDeleteLists(base, sizeof base);
}

void PrintString(unsigned int base, char *str)
{
	float length = 0;
	unsigned int loop;

	if ((str == NULL))
		return;

	// center the text
	for (loop = 0;loop < (strlen(str)); loop++)	
	{
		length+=gmf[str[loop]].gmfCellIncX;		        // increase length by character's width
	}

	if(int(length) % 2 == 0)
		glTranslatef(-length/2,0.0f,0.0f);                  // translate to center text
	else
		glTranslatef(-(length+1)/2,0.0f,0.0f);

	// draw the text
	glPushAttrib(GL_LIST_BIT);
		glListBase(base);
		glCallLists((GLsizei)strlen(str), GL_UNSIGNED_BYTE, str);
	glPopAttrib();
}

void MenuCleanUp()
{
	if(listBase)
		ClearFont(listBase);
}

void InitialiseAGUIMenu()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// clear to black

	glShadeModel(GL_SMOOTH);					
	glEnable(GL_DEPTH_TEST);					
	glEnable(GL_LIGHT0);						
	glEnable(GL_LIGHTING);						
	glEnable(GL_COLOR_MATERIAL);

	if(!listBase)
		listBase = CreateOutlineFont("Arial", 10, 0.25f);  

}

// Render Menus...
void SplashScreen()
{
	// clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	glLoadIdentity();
		
	glTranslatef(0.5f, 3.0f, -10.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	PrintString(listBase, "AntiVirus Demo");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-2.8f, -1.9f, -5.0f);
	PrintString(listBase, "Dean Butcher 2004/5");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-4.3f, -2.0f, 2.0f);
	PrintString(listBase, "mathematix@gmail.com");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-5.5f, -1.9f, 2.0f);
	PrintString(listBase, "Version 2.1");

	glFlush();
	SwapBuffers(g_HDC);			// bring backbuffer to foreground
}

void SplashScreenInWindow()
{
	// clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	glLoadIdentity();
		
	glTranslatef(0.0f, 3.0f, -10.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	PrintString(listBase, "AntiVirus Demo");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.3f, -1.9f, -5.0f);
	PrintString(listBase, "Dean Butcher 2004/5");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-4.0f, -2.0f, 2.0f);
	PrintString(listBase, "mathematix@gmail.com");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-5.2f, -1.9f, 2.0f);
	PrintString(listBase, "Version 2.1");

	glFlush();
	SwapBuffers(g_HDC);			// bring backbuffer to foreground
}

void RenderRootMenu()
{
	glPopAttrib();

	// clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	glLoadIdentity();
		
	glTranslatef(0.0f, 3.0f, -10.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	PrintString(listBase, "Main Menu");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-2.3f, -1.9f, -5.0f);
	PrintString(listBase, "View Characters");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.3f, -1.9f, 2.0f);
	PrintString(listBase, "Play Game!");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-2.1f, -1.9f, 2.0f);
	PrintString(listBase, "Quit");

	glFlush();
	SwapBuffers(g_HDC);			// bring backbuffer to foreground

	glPushAttrib(GL_ALL_ATTRIB_BITS);
}

void RenderRootMenuInWindow()
{
	glPopAttrib();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	glLoadIdentity();
		
	glTranslatef(0.0f, 3.0f, -10.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	PrintString(listBase, "Main Menu");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-2.3f, -1.9f, -5.0f);
	PrintString(listBase, "View Characters");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.3f, -1.9f, 2.0f);
	PrintString(listBase, "Play Game!");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-2.1f, -1.9f, 2.0f);
	PrintString(listBase, "Quit");

	glFlush();
	SwapBuffers(g_HDC);	

	glPushAttrib(GL_ALL_ATTRIB_BITS);
}

void RenderSubMenu1()
{
	glPopAttrib();

	// clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	glLoadIdentity();
		
	glTranslatef(1.0f, 3.0f, -10.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	PrintString(listBase, "Chains and Power-ups!");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-5.1f, -1.9f, -5.0f);
	PrintString(listBase, "Sphere Chains");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.0f, -1.9f, 2.0f);
	PrintString(listBase, "Power-ups");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-2.4f, -1.9f, 2.0f);
	PrintString(listBase, "Back");

	glFlush();
	SwapBuffers(g_HDC);			// bring backbuffer to foreground

	glPushAttrib(GL_ALL_ATTRIB_BITS);
}

void RenderSubMenu1InWindow()
{
	glPopAttrib();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	glLoadIdentity();
		
	glTranslatef(0.5f, 3.0f, -10.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	PrintString(listBase, "Chains and Power ups!");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-5.4f, -1.9f, -5.0f);
	PrintString(listBase, "Sphere Chains");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.0f, -1.9f, 2.0f);
	PrintString(listBase, "Power ups");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-2.2f, -1.9f, 2.0f);
	PrintString(listBase, "Back");

	glFlush();
	SwapBuffers(g_HDC);	

	glPushAttrib(GL_ALL_ATTRIB_BITS);
}

void RenderSubMenu2_1()
{
	glPopAttrib();

	// clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	glLoadIdentity();
		
	glTranslatef(1.0f, 3.0f, -10.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	PrintString(listBase, "Character Chains");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.1f, -1.5f, -7.0f);
	PrintString(listBase, "Basic Chain");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-1.9f, -1.3f, 0.0f);
	PrintString(listBase, "Rebirth Chain");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-2.5f, -1.3f, 0.0f);
	PrintString(listBase, "Supreme Blast Chain");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.7f, -1.3f, 0.0f);
	PrintString(listBase, "Invisibility Chain");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.5f, -1.3f, 0.0f);
	PrintString(listBase, "SupaFly Chain");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-3.2f, -1.5f, 0.0f);
	PrintString(listBase, "Back");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-0.5f, -1.5f, 0.0f);
	PrintString(listBase, "Back to Main");

	glFlush();
	SwapBuffers(g_HDC);	

	glPushAttrib(GL_ALL_ATTRIB_BITS);
}

void RenderSubMenu2_1InWindow()
{
	glPopAttrib();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	glLoadIdentity();
		
	glTranslatef(0.5f, 3.0f, -10.0f);
	glColor3f(0.6f, 0.5f, 0.0f);
	PrintString(listBase, "Character Chains");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.5f, -0.5f, -7.0f);
	PrintString(listBase, "Basic Chain");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-2.2f, -1.3f, 0.0f);
	PrintString(listBase, "Rebirth Chain");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-2.3f, -1.3f, 0.0f);
	PrintString(listBase, "Supreme Blast Chain");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-4.0f, -1.3f, 0.0f);
	PrintString(listBase, "Invisibility Chain");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.5f, -1.3f, 0.0f);
	PrintString(listBase, "SupaFly Chain");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-3.2f, -2.0f, 0.0f);
	PrintString(listBase, "Back");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-0.5f, -1.5f, 0.0f);
	PrintString(listBase, "Back to Main");

	glFlush();
	SwapBuffers(g_HDC);			

	glPushAttrib(GL_ALL_ATTRIB_BITS);
}

void RenderSubMenu2_2()
{
	glPopAttrib();

	// clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	glLoadIdentity();
		
	glTranslatef(0.5f, 3.0f, -10.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	PrintString(listBase, "Power-Ups");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-1.5f, -1.5f, -7.0f);
	PrintString(listBase, "Rebirth Power-up");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.6f, -1.3f, 0.0f);
	PrintString(listBase, "Supreme Power-up");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-4.0f, -1.3f, 0.0f);
	PrintString(listBase, "Invisibility Power-up");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-4.5f, -1.5f, 0.0f);
	PrintString(listBase, "Back");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-0.5f, -1.3f, 0.0f);
	PrintString(listBase, "Back to Main");

	glFlush();
	SwapBuffers(g_HDC);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
}

void RenderSubMenu2_2InWindow()
{
	glPopAttrib();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	glLoadIdentity();
		
	glTranslatef(0.0f, 3.0f, -10.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	PrintString(listBase, "Power Ups");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-1.7f, -1.5f, -7.0f);
	PrintString(listBase, "Rebirth Power up");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-3.6f, -1.3f, 0.0f);
	PrintString(listBase, "Supreme Power up");

	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(-4.1f, -1.3f, 0.0f);
	PrintString(listBase, "Invisibility Power up");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-4.5f, -2.5f, 0.0f);
	PrintString(listBase, "Back");

	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(-0.5f, -1.5f, 0.0f);
	PrintString(listBase, "Back to Main");

	glFlush();
	SwapBuffers(g_HDC);		

	glPushAttrib(GL_ALL_ATTRIB_BITS);
}

void DisplayMenu(int menuNum, bool bWindowed)
{
	switch(menuNum)
	{
		case 0:
			if(bWindowed)
				RenderRootMenuInWindow();
			else
				RenderRootMenu();
			break;
		case 1:
			if(bWindowed)
				RenderSubMenu1InWindow();
			else
				RenderSubMenu1();
			break;
		case 2:
			if(bWindowed)
				RenderSubMenu2_1InWindow();
			else
				RenderSubMenu2_1();
			break;
		case 3:
			if(bWindowed)
				RenderSubMenu2_2InWindow();
			else
				RenderSubMenu2_2();
			break;
		default:
			break;
	}
}

MENU_OPTION DetermineRegionOfMouseClickMenu0(int x_region, int y_region)
{		
	if(((x_region >= 255) && (x_region <= 540)) && 
	   ((y_region >= 225) && (y_region <= 260)))
		// Main menu 'View characters' clicked!
		return ViewCharacters;

	if(((x_region >= 295) && (x_region <= 520)) && 
	   ((y_region >= 300) && (y_region <= 340)))
		// Main menu 'Play Game' clicked!
		return PlayGame;

	if(((x_region >= 345) && (x_region <= 455)) && 
	   ((y_region >= 405) && (y_region <= 445)))
		// Main menu 'Quit' clicked!
		return Quit;
	
	return Miss; // Some other region clicked that is not valid for this menu.
}

MENU_OPTION DetermineRegionOfMouseClickMenu0InWindow(int x_region, int y_region)
{		
	if(((x_region >= 260) && (x_region <= 525)) && 
	   ((y_region >= 215) && (y_region <= 245)))
		// Main menu 'View characters' clicked!
		return ViewCharacters;

	if(((x_region >= 295) && (x_region <= 510)) && 
	   ((y_region >= 290) && (y_region <= 320)))
		// Main menu 'Play Game' clicked!
		return PlayGame;

	if(((x_region >= 350) && (x_region <= 445)) && 
	   ((y_region >= 385) && (y_region <= 420)))
		// Main menu 'Quit' clicked!
		return Quit;
	
	return Miss; // Some other region clicked that is not valid for this menu.
}

MENU_OPTION DetermineRegionOfMouseClickMenu1(int x_region, int y_region)
{
	if(((x_region >= 280) && (x_region <= 545)) && 
	   ((y_region >= 220) && (y_region <= 265)))
		// Submenu 2 'Sphere Chains'
		return SphereChains;

	if(((x_region >= 315) && (x_region <= 530)) && 
	   ((y_region >= 300) && (y_region <= 340)))
		// Submenu 2 'Power Ups'
		return PowerUps;

	if(((x_region >= 337) && (x_region <= 472)) && 
	   ((y_region >= 402) && (y_region <= 449)))
		// Submenu 1 'Back' clicked.
		return Back;

	return Miss;
}

MENU_OPTION DetermineRegionOfMouseClickMenu1InWindow(int x_region, int y_region)
{
	if(((x_region >= 260) && (x_region <= 495)) && 
	   ((y_region >= 215) && (y_region <= 245)))
		// Submenu 2 'Sphere Chains'
		return SphereChains;

	if(((x_region >= 285) && (x_region <= 480)) && 
	   ((y_region >= 290) && (y_region <= 320)))
		// Submenu 2 'Power Ups'
		return PowerUps;

	if(((x_region >= 330) && (x_region <= 440)) && 
	   ((y_region >= 385) && (y_region <= 420)))
		// Submenu 1 'Back' clicked.
		return Back;

	return Miss;
}

MENU_OPTION DetermineRegionOfMouseClickMenu2_1(int x_region, int y_region)
{
	if(((x_region >= 330) && (x_region <= 515)) && 
	   ((y_region >= 220) && (y_region <= 250)))
	   // View Basic Chain
	   return BasicChain;

	if(((x_region >= 320) && (x_region <= 530)) && 
	   ((y_region >= 265) && (y_region <= 295)))
	   // View Rebirth Chain
	   return RebirthChain;

	if(((x_region >= 265) && (x_region <= 580)) && 
	   ((y_region >= 310) && (y_region <= 340)))
	   // View Supreme Blast Chain
	   return SupremeBlastChain;

	if(((x_region >= 305) && (x_region <= 560)) && 
	   ((y_region >= 355) && (y_region <= 385)))
	   // View Invisibility Chain
	   return InvisibilityChain;

	if(((x_region >= 325) && (x_region <= 545)) && 
	   ((y_region >= 400) && (y_region <= 430)))
	   // View SupaFly Chain
	   return SupaFlyChain;

	if(((x_region >= 390) && (x_region <= 475)) && 
	   ((y_region >= 450) && (y_region <= 480)))
	   // Back to previous menu
	   return Back;

	if(((x_region >= 335) && (x_region <= 535)) && 
	   ((y_region >= 500) && (y_region <= 535)))
	   // Back to main menu
	   return BackToMain;

	return Miss;
}

MENU_OPTION DetermineRegionOfMouseClickMenu2_1InWindow(int x_region, int y_region)
{
	if(((x_region >= 300) && (x_region <= 470)) && 
	   ((y_region >= 180) && (y_region <= 200)))
	   // View Basic Chain
	   return BasicChain;

	if(((x_region >= 285) && (x_region <= 480)) && 
	   ((y_region >= 220) && (y_region <= 245)))
	   // View Rebirth Chain
	   return RebirthChain;

	if(((x_region >= 240) && (x_region <= 535)) && 
	   ((y_region >= 265) && (y_region <= 285)))
	   // View Supreme Blast Chain
	   return SupremeBlastChain;

	if(((x_region >= 270) && (x_region <= 505)) && 
	   ((y_region >= 305) && (y_region <= 330)))
	   // View Invisibility Chain
	   return InvisibilityChain;

	if(((x_region >= 285) && (x_region <= 490)) && 
	   ((y_region >= 350) && (y_region <= 375)))
	   // View SupaFly Chain
	   return SupaFlyChain;

	if(((x_region >= 350) && (x_region <= 420)) && 
	   ((y_region >= 415) && (y_region <= 440)))
	   // Back to previous menu
	   return Back;

	if(((x_region >= 295) && (x_region <= 480)) && 
	   ((y_region >= 465) && (y_region <= 485)))
	   // Back to main menu
	   return BackToMain;

	return Miss;
}

MENU_OPTION DetermineRegionOfMouseClickMenu2_2(int x_region, int y_region)
{
	if(((x_region >= 295) && (x_region <= 560)) && 
	   ((y_region >= 220) && (y_region <= 255)))
	   // View Rebirth PowerUp
	   return RebirthPowerUp;

	if(((x_region >= 290) && (x_region <= 575)) && 
	   ((y_region >= 265) && (y_region <= 300)))
	   // View Supreme Blast PowerUp
	   return SupremePowerUp;

	if(((x_region >= 280) && (x_region <= 585)) && 
	   ((y_region >= 310) && (y_region <= 345)))
	   // View Invisibility PowerUp
	   return InvisibilityPowerUp;

	if(((x_region >= 390) && (x_region <= 470)) && 
	   ((y_region >= 360) && (y_region <= 395)))
	   // Back to previous menu
	   return Back;

	if(((x_region >= 330) && (x_region <= 530)) && 
	   ((y_region >= 410) && (y_region <= 440)))
	   // Back to main menu
	   return BackToMain;

	return Miss;
}

MENU_OPTION DetermineRegionOfMouseClickMenu2_2InWindow(int x_region, int y_region)
{
	if(((x_region >= 275) && (x_region <= 520)) && 
	   ((y_region >= 210) && (y_region <= 240)))
	   // View Rebirth PowerUp
	   return RebirthPowerUp;

	if(((x_region >= 270) && (x_region <= 535)) && 
	   ((y_region >= 255) && (y_region <= 280)))
	   // View Supreme Blast PowerUp
	   return SupremePowerUp;

	if(((x_region >= 260) && (x_region <= 545)) && 
	   ((y_region >= 295) && (y_region <= 325)))
	   // View Invisibility PowerUp
	   return InvisibilityPowerUp;

	if(((x_region >= 360) && (x_region <= 435)) && 
	   ((y_region >= 375) && (y_region <= 400)))
	   // Back to previous menu
	   return Back;

	if(((x_region >= 305) && (x_region <= 490)) && 
	   ((y_region >= 325) && (y_region <= 450)))
	   // Back to main menu
	   return BackToMain;

	return Miss;
}

MENU_OPTION FindMenuRegionClicked(int menuNum, int x_region, int y_region, bool bWindowed)
{
	switch(menuNum)
	{
		case 0:
			if(bWindowed)
				return DetermineRegionOfMouseClickMenu0InWindow(x_region, y_region);
			else
				return DetermineRegionOfMouseClickMenu0(x_region, y_region);
			break;
		case 1:
			if(bWindowed)
				return DetermineRegionOfMouseClickMenu1InWindow(x_region, y_region);
			else
				return DetermineRegionOfMouseClickMenu1(x_region, y_region);
			break;
		case 2:
			if(bWindowed)
				return DetermineRegionOfMouseClickMenu2_1InWindow(x_region, y_region);
			else
				return DetermineRegionOfMouseClickMenu2_1(x_region, y_region);
			break;
		case 3:
			if(bWindowed)
				return DetermineRegionOfMouseClickMenu2_2InWindow(x_region, y_region);
			else
				return DetermineRegionOfMouseClickMenu2_2(x_region, y_region);
			break;
		default:
			break;
	}
	return Miss;
}

void SendMenuOptionToMessageQueue(HWND hwnd, WPARAM &wparam, LPARAM &lparam, int &menuNum, MENU_OPTION menuOption)
{
	const char PMQUIT[] = "Unable to process menu options - Quitting game."; 
	const char PMTITLE[] = "Menu Processing error";
	UINT currentMsg;
	bool postMsg = false;

	switch(menuNum)
	{
		case 0:			// The root menu options.
			switch(menuOption)
			{
				case ViewCharacters:
					menuNum = 1;
					break;
				case PlayGame:
					currentMsg = ENV_PAGOE;
					postMsg = true;
					break;
				case Quit:
					postMsg = true;
					currentMsg = WM_DESTROY;
					break;
				default:
					break;
			}
			break;
		case 1:			// Choose to view chains or power ups.
			switch(menuOption)
			{
				case SphereChains:
					menuNum = 2;
					break;
				case PowerUps:
					menuNum = 3;
					break;
				case Back:
					menuNum = 0;
					break;
				default:
					break;
			}
			break;
		case 2:			// View all chain models.
			switch(menuOption)
			{
				case BasicChain:
					currentMsg = SPHR_STANDARD;
					postMsg = true;
					break;
				case RebirthChain:
					currentMsg = SPHR_REBIRTH;
					postMsg = true;
					break;
				case SupremeBlastChain:
					currentMsg = SPHR_SUPBLAST;
					postMsg = true;
					break;
				case InvisibilityChain:
					currentMsg = SPHR_INVISIBL;
					postMsg = true;
					break;
				case SupaFlyChain:
					currentMsg = SPHR_SBREBTH;
					postMsg = true;
					break;
				case Back:
					menuNum = 1;
					break;
				case BackToMain:
					menuNum = 0;
					break;
				default:
					break;
			}
			break;
		case 3:			// View all power ups.
			switch(menuOption)
			{
				case RebirthPowerUp:
					currentMsg = PU_REBIRTH;
					postMsg = true;
					break;
				case SupremePowerUp:
					currentMsg = PU_SUPBLAST;
					postMsg = true;
					break;
				case InvisibilityPowerUp:
					currentMsg = PU_INVISIBL;
					postMsg = true;
					break;
				case Back:
					menuNum = 1;
					break;
				case BackToMain:
					menuNum = 0;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	if(postMsg)
		if(!PostMessage(hwnd, currentMsg, wparam, lparam))
		{
			MessageBox(hwnd, PMQUIT, PMTITLE, MB_OK);
			PostQuitMessage(0);
		}
}
