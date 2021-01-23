#ifndef _COMMONS_H_
#define _COMMONS_H_

// ---------------------------------------------------------------------- //

struct Rect2D
{
	Rect2D(float startX, float startY, float startWidth, float startHeight)
	{
		x      = startX;
		y      = startY;
		width  = startWidth;
		height = startHeight;
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
	DOWN
};

// ---------------------------------------------------------------------- //

#endif