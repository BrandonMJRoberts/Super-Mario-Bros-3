#ifndef _COMMONS_H_
#define _COMMONS_H_

#include "Game_Maths.h"

// ---------------------------------------------------------------------- //

struct GameSelectReturnData
{
	GameSelectReturnData()                                                                  { gameSelected = false; gameSelectedIsMario3 = false; quitEverything = false; }
	GameSelectReturnData(bool gameIsSelected, bool isGameSelectedIsMario3, bool shouldQuit) { gameSelected = gameIsSelected; gameSelectedIsMario3 = isGameSelectedIsMario3; quitEverything = shouldQuit; }

	bool gameSelected;
	bool gameSelectedIsMario3;
	bool quitEverything;
};

// ---------------------------------------------------------------------- //

struct Rect2D
{
	Rect2D(float startX, float startY, float startWidth, float startHeight) 
		: x(startX)
		, y(startY)
		, width(startWidth)
		, height(startHeight)
	{	
	}

	Rect2D(Vector2D position, Vector2D collisionBox)
		: x(position.x)
		, y(position.y)
		, width(collisionBox.x)
		, height(collisionBox.y)
	{

	}

	double x;
	double y;
	double width;
	double height;
};

// ---------------------------------------------------------------------- //

enum class SCREENS
{
	INTRO = 0,
	MENU,
	LEVEL1,
	LEVEL2,
	GAMEOVER,
	HIGHSCORES
};

// ---------------------------------------------------------------------- //

// Using enum class instead of just an enum so that the values do not pollute the scope (for naming clashes) 
// And they can be forward declared
enum class FACING
{
	LEFT = 0,
	RIGHT,
	UP,
	DOWN,
	NONE
};

// ---------------------------------------------------------------------- //

#endif