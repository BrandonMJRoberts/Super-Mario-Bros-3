#ifndef _POW_SMB1_H_
#define _POW_SMB1_H_

#include "Game_Maths.h"

class  Texture2D;
struct SDL_Renderer;
struct SDL_Rect;

class POW
{
public:
	POW(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D bottomLeftStartPos);
	~POW();

	void     Render();
	void     Update(const float deltaTime);

	Vector2D GetCollisionBox() const { return mCollisionBox; }
	Vector2D GetPosition()     const { return mPosition;     }

	bool     SetHasBeenHit();

private:
	Texture2D*   mSpriteSheet;

	Vector2D     mPosition;
	Vector2D     mStartPosition;

	Vector2D	 mCollisionBox;

	SDL_Rect*    mSourceRect;
	SDL_Rect*	 mDestRect;

	unsigned int mCurrentSpriteID;

	bool	     mDoingBounce;
	bool		 mGoingUp;
};

#endif