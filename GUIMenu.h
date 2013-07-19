/*!\file GUIMenu.h
 * \brief Function prototypes.
 * \author Dean N. Butcher
 * \version 2.1
 * \date August 2004
 */

#ifndef _MOUSE_TRACKING_
#define _MOUSE_TRACKING_

enum MENU_OPTION {ViewCharacters, PlayGame, MainMenu, Back, BackToMain, Quit, SphereChains, PowerUps, Miss,
				  BasicChain, RebirthChain, SupremeBlastChain, InvisibilityChain,
				  SupaFlyChain, RebirthPowerUp, SupremePowerUp, InvisibilityPowerUp};

void ZeroMouseCoords(int *x, int *y);
unsigned int CreateOutlineFont(char *fontName, int fontSize, float depth);
void ClearFont(unsigned int base);
void PrintString(unsigned int base, char *str);
void MenuCleanUp();
void InitialiseAGUIMenu();
void SplashScreen();
void SplashScreenInWindow();
void RenderRootMenu();
void RenderRootMenuInWindow();
void RenderSubMenu1();
void RenderSubMenu1InWindow();
void RenderSubMenu2_1();
void RenderSubMenu2_1InWindow();
void RenderSubMenu2_2();
void RenderSubMenu2_2InWindow();
void DisplayMenu(int menuNum, bool bWindowed);
MENU_OPTION DetermineRegionOfMouseClickMenu0(int x_region, int y_region);
MENU_OPTION DetermineRegionOfMouseClickMenu0InWindow(int x_region, int y_region);
MENU_OPTION DetermineRegionOfMouseClickMenu1(int x_region, int y_region);
MENU_OPTION DetermineRegionOfMouseClickMenu1InWindow(int x_region, int y_region);
MENU_OPTION DetermineRegionOfMouseClickMenu2_1(int x_region, int y_region);
MENU_OPTION DetermineRegionOfMouseClickMenu2_1InWindow(int x_region, int y_region);
MENU_OPTION DetermineRegionOfMouseClickMenu2_2(int x_region, int y_region);
MENU_OPTION DetermineRegionOfMouseClickMenu2_2InWindow(int x_region, int y_region);
MENU_OPTION FindMenuRegionClicked(int menuNum, int x_region, int y_region, bool bWindowed);
void SendMenuOptionToMessageQueue(HWND hwnd, WPARAM &wparam, LPARAM &lparam, int &menuNum, MENU_OPTION menuOption);

#endif