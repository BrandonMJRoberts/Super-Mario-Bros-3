#ifndef _PLAYABLE_CHARACTER_H
#define _PLAYABLE_CHARACTER_H

#include "Commons_SMB3.h"
#include "Texture2D.h"

class PlayableCharacter
{
public:
	PlayableCharacter(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, Vector2D numberOfSpritesOnDimensions);
	virtual ~PlayableCharacter();

	virtual void Render();
	virtual void Update(const float deltaTime, SDL_Event e);

	const Vector2D GetRealGridPosition()     const { return mRealGridPosition; }
	const Vector2D GetScreenGridPosition()   const { return mScreenGridPosition; }

private:
	void HandleMovementInput(SDL_Event e);

	Vector2D   mRealGridPosition;
	Vector2D   mScreenGridPosition;

	Vector2D   mVelocity;
	Vector2D   mAcceleration;

	unsigned int mNumberOfSpritesOnWidth;;

	Texture2D* mSpriteSheet;

	unsigned int mSingleSpriteWidth;
	unsigned int mSingleSpriteHeight;

	unsigned int mCurrentFrame;
	unsigned int mStartFrame;
	unsigned int mEndFrame;

	bool       mIsAlive;
};


#endif