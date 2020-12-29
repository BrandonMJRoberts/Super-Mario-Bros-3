#ifndef _BASE_CHARACTER_H
#define _BASE_CHARACTER_H

#include "Commons_SMB3.h"
#include "Texture2D.h"

class BaseCharacter abstract
{
public:
	BaseCharacter(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, Vector2D numberOfSpritesOnDimensions);
	virtual ~BaseCharacter();

	virtual void Render();
	virtual void Update(const float deltaTime, SDL_Event e);

	const Vector2D GetPosition() const { return mPosition; }

private:
	void HandleMovementInput(SDL_Event e);

	Vector2D   mPosition;
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