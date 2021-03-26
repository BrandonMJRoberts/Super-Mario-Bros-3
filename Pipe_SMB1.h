#ifndef _PIPE_SMB1_H_
#define _PIPE_SMB1_H_

#include "Game_Maths.h"

class  Texture2D;
struct SDL_Renderer;
struct SDL_Rect;

// -------------------------------------------------------------------------------- //

enum class PIPE_FACING_DIRECTION_SMB1
{
	RIGHT = 0,
	LEFT,
};

// -------------------------------------------------------------------------------- //

class PIPE_SMB1
{
public:
	PIPE_SMB1(SDL_Renderer* renderer, const char* filePathToSpriteSheet, PIPE_FACING_DIRECTION_SMB1 facingDirection, Vector2D bottomLeftPosition);
	~PIPE_SMB1();

	void Render();
	void Update(const float deltaTime);

	void SetReleasingEnemy();

private:
	Vector2D     mPosition;

	Texture2D*   mSpriteSheet;

	PIPE_FACING_DIRECTION_SMB1 mPipeFacingDirection;

	SDL_Rect*    mSourceRect;
	SDL_Rect*    mDestRect;
};

// -------------------------------------------------------------------------------- //

#endif