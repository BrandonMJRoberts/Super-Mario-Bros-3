#ifndef _BASE_OBJECT_SMB3_H_
#define _BASE_OBJECT_SMB3_H_

#include <SDL.h>
#include "Game_Maths.h"

class Texture2D;

class BaseObject_SMB3
{
public:
	BaseObject_SMB3() = delete;
	BaseObject_SMB3(const SDL_Renderer* renderer);
	~BaseObject_SMB3();

	virtual bool Update(const float deltaTime);
	virtual void Render();

protected:
	Texture2D*          mSpriteSheet;
	const SDL_Renderer* mRenderer;

	Vector2D            mCurrentPosition;
	Vector2D	        mSpawnPosition;

	unsigned int        mCurrentAnimationIndex;
	unsigned int        mFirstAnimationIndex;
	unsigned int        mLastAnimationIndex;

	unsigned int        mAmountOfSpritesWidth;
	unsigned int        mAmountOfSpritesHeight;

	bool                mIsCurrentlySpawnedInLevel;
};

#endif // !_BASE_OBJECT_SMB3_H_
