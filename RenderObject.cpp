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

	, kMovementSpeed(3.0f)
{

}

// --------------------------------------------------------------- //

RenderObject::RenderObject(unsigned int start, unsigned int end, unsigned int current, const float timePerFrame, Vector2D startPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, Vector2D collisionBox, const float movementSpeed)
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

	, kMovementSpeed(movementSpeed)
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
	mDestRect->x = int(mPosition.x * SPRITE_RES);
	mDestRect->y = int((mPosition.y - mCollisionBox.y) * SPRITE_RES);

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
			mCurrentSpriteID = mStartFrameID;
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

	CheckForLooping(levelMap);
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

void RenderObject::UpdatePhysics(const float deltaTime, LevelMap* levelMap)
{
	Vector2D movementDistance = mVelocity * deltaTime;

	// Check for collisions
	if (   levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x))                   == 1 // left check
		|| levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x + mCollisionBox.x)) == 1) // right check
	{
		mVelocity.y = 0.0f;
	}
	else if (levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x))                   == 1 ||  // left check
		     levelMap->GetCollisionTileAt(int(mPosition.y + movementDistance.y), int(mPosition.x + mCollisionBox.x)) == 1) // right check
	{
		// Up collision bounce back down
		mVelocity.y = 1.0f;
	}
	else
	{
		mVelocity.y += CHARACTER_GRAVITY * deltaTime;

		// No y collisions
		mPosition.y += movementDistance.y;
	}

	// Now for x checks
	if (   levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y), int(mPosition.x)) == 1  // top left check
		|| levelMap->GetCollisionTileAt(int(mPosition.y),                   int(mPosition.x)) == 1) // bottom left check
	{
		mVelocity.x = 0.0f;
	}
	else if (levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y), int(mPosition.x + mCollisionBox.x)) == 1 ||  // top right check
		     levelMap->GetCollisionTileAt(int(mPosition.y),                   int(mPosition.x + mCollisionBox.x)) == 1) // bottom right check
	{
		// Up collision bounce back down
		mVelocity.x = 0.0f;
	}
	else
	{
		if (mFacingLeft)
			mVelocity.x = -kMovementSpeed;
		else
			mVelocity.x = kMovementSpeed;

		// No y collisions
		mPosition.x += movementDistance.x;
	}
}

// --------------------------------------------------------------- //

void RenderObject::CheckForLooping(LevelMap* levelMap)
{
	if (!levelMap)
		return;

	if (mPosition.x > levelMap->GetLevelWidth() && mVelocity.x > 0.0f)
	{
		mPosition.x = -(mCollisionBox.x) + 0.01;
	}
	else if (mPosition.x + mCollisionBox.x < 0.0f && mVelocity.x < 0.0f)
	{
		mPosition.x = levelMap->GetLevelWidth() - 0.01;
	}
}

// --------------------------------------------------------------- //