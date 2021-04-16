#include "BaseAnimation.h"

#include "Constants_SMB3.h"
#include "Texture2D.h"

#include <SDL.h>

// -------------------------------------------------------------- //

BaseAnimation::BaseAnimation(Vector2D startPos, unsigned int startFrame, unsigned int endFrame, const float timePerFrame, Vector2D startVelocity, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight) 
	: kSpritesOnWidth(spritesOnWidth),
	  kSpritesOnHeight(spritesOnHeight),

	  mPosition(startPos),

	  mCurrentFrameID(startFrame),
	  mStartFrameID(startFrame),
	  mEndFrameID(endFrame),

	  mSingleSpriteWidth(0),
	  mSingleSpriteHeight(0),

	  mCompletedAnimation(false),

	  mTimePerFrame(timePerFrame),
	  mTimeRemainingForFrame(timePerFrame),
	  mVelocity(startVelocity)
{

}

// -------------------------------------------------------------- //

BaseAnimation::~BaseAnimation()
{

}

// -------------------------------------------------------------- //

void BaseAnimation::Render(Vector2D offset)
{
	Texture2D* spriteSheet = GetSpriteSheet();

	if (spriteSheet)
	{
		Vector2D renderPos = Vector2D(mPosition.x - offset.x, mPosition.y - offset.y);

		SDL_Rect portionOfSpriteSheet{ (int)(mCurrentFrameID % kSpritesOnWidth) * (int)mSingleSpriteWidth,
									   (int)(mCurrentFrameID / kSpritesOnWidth) * (int)mSingleSpriteHeight,
									   (int)mSingleSpriteWidth,
									   (int)mSingleSpriteHeight };

		SDL_Rect destRect{            int(renderPos.x * RESOLUTION_OF_SPRITES),
									  int(renderPos.y * RESOLUTION_OF_SPRITES) - (int)mSingleSpriteHeight + 2,
									 (int)mSingleSpriteWidth,
									 (int)mSingleSpriteHeight };

		spriteSheet->Render(portionOfSpriteSheet, destRect, SDL_FLIP_NONE);
	}
}

// -------------------------------------------------------------- //

void BaseAnimation::UpdateAnimations(const float deltaTime)
{
	mTimeRemainingForFrame -= deltaTime;
	if (mTimeRemainingForFrame < 0.0f)
	{
		mTimeRemainingForFrame = mTimePerFrame;

		mCurrentFrameID++;

		if (mCurrentFrameID > mEndFrameID && !mCompletedAnimation)
			mCurrentFrameID = mStartFrameID;
	}
}

// -------------------------------------------------------------- //