#include "RenderObject.h"

#include "Texture2D.h"

#include <SDL.h>

#include "LevelMap.h"

// --------------------------------------------------------------- //

RenderObject::RenderObject()
	: mCurrentSpriteID(0)
	, mEndSpriteID(0)
	, mStartFrameID(0)

	, kTimePerFrame(0.0f)
	, mTimeRemainingPerFrame(kTimePerFrame)

	, mPosition(0.0f, 0.0f)
	, mVelocity(0.0f, 0.0f)

	, mSourceRect(nullptr)
	, mDestRect(nullptr)

	, kSpritesOnWidth(0)
	, kSpritesOnHeight(0)

	, mFacingLeft(true)

    , mCollisionBox(1.0f, 1.0f)

	, mSingleSpriteHeight(0)
	, mSingleSpriteWidth(0)
{

}

// --------------------------------------------------------------- //

RenderObject::RenderObject(unsigned int start, unsigned int end, unsigned int current, const float timePerFrame, Vector2D startPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, Vector2D collisionBox)
	: mCurrentSpriteID(current)
	, mEndSpriteID(end)
	, mStartFrameID(start)

	, kTimePerFrame(timePerFrame)
	, mTimeRemainingPerFrame(timePerFrame)

	, mPosition(startPosition)

	, mSourceRect(nullptr)
	, mDestRect(nullptr)

	, kSpritesOnWidth(spritesOnWidth)
	, kSpritesOnHeight(spritesOnHeight)

	, mVelocity(0.0f, 0.0f)

	, mFacingLeft(true)

	, mCollisionBox(collisionBox)

	, mSingleSpriteHeight(0)
	, mSingleSpriteWidth(0)
{

}

// --------------------------------------------------------------- //

RenderObject::~RenderObject()
{
	delete mSourceRect;
	mSourceRect = nullptr;

	delete mDestRect;
	mDestRect = nullptr;
}

// --------------------------------------------------------------- //

void RenderObject::Render()
{
	// Basic render function
	Texture2D* texture = GetSpriteSheet();

	if (!texture || !mSourceRect || !mDestRect)
		return;

	// Update the render position
	mDestRect->x = mPosition.x * SPRITE_RES;
	mDestRect->y = (mPosition.y - mCollisionBox.y) * SPRITE_RES;

	if(mFacingLeft)
		texture->Render(*mSourceRect, *mDestRect, SDL_FLIP_NONE, 0.0f);
	else
		texture->Render(*mSourceRect, *mDestRect, SDL_FLIP_HORIZONTAL, 0.0f);
}

// --------------------------------------------------------------- //

void RenderObject::Update(const float deltaTime, LevelMap* levelMap)
{
	// Take the time off 
	mTimeRemainingPerFrame -= deltaTime;

	// Check for the time boundary
	if (mTimeRemainingPerFrame < 0.0f)
	{
		// Reset the time
		mTimeRemainingPerFrame = kTimePerFrame;

		// Increment the sprite ID
		mCurrentSpriteID++;

		// Check for looping boundaries
		if (mCurrentSpriteID > mEndSpriteID)
		{
			mCurrentSpriteID = 0;
		}

		if (mSourceRect)
		{
			// Update the sprite sheet
			mSourceRect->x = int(mSingleSpriteWidth * (mCurrentSpriteID % kSpritesOnWidth));
			mSourceRect->y = int(mSingleSpriteHeight * int(mCurrentSpriteID / kSpritesOnWidth));
		}
	}

	// Now update the physics of this object - this should be overriden by the child class
	UpdatePhysics(deltaTime, levelMap);
}

// --------------------------------------------------------------- //

void RenderObject::SetupRenderRects()
{
	// First get the texture
	Texture2D* texture = GetSpriteSheet();

	if (!texture || kSpritesOnHeight == 0 || kSpritesOnWidth == 0)
		return;

	mSingleSpriteWidth  = texture->GetWidth()  / kSpritesOnWidth;
	mSingleSpriteHeight = texture->GetHeight() / kSpritesOnHeight;

	mSourceRect        = new SDL_Rect{int(mSingleSpriteWidth *    (mCurrentSpriteID % kSpritesOnWidth)),
		                              int(mSingleSpriteHeight * int(mCurrentSpriteID / kSpritesOnWidth)),
									  (int)mSingleSpriteWidth,
									  (int)mSingleSpriteHeight };

	mDestRect          = new SDL_Rect{   int(mPosition.x         * SPRITE_RES), 
									  (int)((mPosition.y - 1.0f) * SPRITE_RES),
									   (int)mSingleSpriteWidth,
									   (int)mSingleSpriteHeight };
}

// --------------------------------------------------------------- //
