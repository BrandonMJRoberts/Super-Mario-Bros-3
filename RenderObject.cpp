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

	, mMovementSpeed(3.0f)

	, mGrounded(false)
	, mHittingWall(false)

	, mIsFlipped(false)
	, mIsAlive(true)
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

	, mMovementSpeed(movementSpeed)

	, mGrounded(false)
	, mHittingWall(false)

	, mIsFlipped(false)
	, mIsAlive(true)
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

bool RenderObject::Update(const float deltaTime, LevelMap* levelMap)
{
	if (mGrounded)
	{
		UpdateAnimations(deltaTime);
	}

	// Now update the physics of this object - this should be overriden by the child class
	UpdatePhysics(deltaTime, levelMap);

	if (ClassSpecificUpdate(deltaTime))
		return true;

	if (CheckForLooping(levelMap))
		return true;

	return false;
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
									  (int)((mPosition.y - mCollisionBox.y) * SPRITE_RES),
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

		mGrounded = true;
	}
	//else if (levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y + movementDistance.y), int(mPosition.x))                   == 1 ||  // left check
	//	     levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y + movementDistance.y), int(mPosition.x + mCollisionBox.x)) == 1) // right check
	//{
		// Up collision bounce back down
	//	mVelocity.y      = 1.0f;

	//	mGrounded        = false;
	//}
	else
	{
		mVelocity.y += CHARACTER_GRAVITY * deltaTime;

		// No y collisions
		mPosition.y += movementDistance.y;

		mGrounded = false;
	}

	// Now for x checks
	if (   levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y), int(mPosition.x)) == 1  // top left check
		|| levelMap->GetCollisionTileAt(int(mPosition.y),                   int(mPosition.x)) == 1) // bottom left check
	{
		mVelocity.x = 0.0f;

		// Flip direction
		if (!mHittingWall)
		{
			mFacingLeft = !mFacingLeft;
			mHittingWall = true;
		}
	}
	else if (levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y), int(mPosition.x + mCollisionBox.x)) == 1 ||  // top right check
		     levelMap->GetCollisionTileAt(int(mPosition.y),                   int(mPosition.x + mCollisionBox.x)) == 1) // bottom right check
	{
		// Up collision bounce back down
		mVelocity.x = 0.0f;

		// Flip direction
		if (!mHittingWall)
		{
			mFacingLeft  = !mFacingLeft;
			mHittingWall = true;
		}
	}
	else
	{
		// Set X velocity
		if (!mIsFlipped)
		{
			if (mFacingLeft)
				mVelocity.x = -mMovementSpeed;
			else
				mVelocity.x = mMovementSpeed;
		}

		mHittingWall = false;

		// No y collisions
		mPosition.x += movementDistance.x;
	}
}

// --------------------------------------------------------------- //

bool RenderObject::CheckForLooping(LevelMap* levelMap)
{
	if (!levelMap)
		return false;

	if (mPosition.x > levelMap->GetLevelWidth() && mVelocity.x > 0.0f)
	{
		// Now check to see if we are at the bottom of the screen
		if (int(mPosition.y + 0.05) == levelMap->GetLevelHeight() - 1)
		{
			return true;
		}
		else
		{
			mPosition.x = -(mCollisionBox.x) + 0.01;
		}
	}
	else if (mPosition.x + mCollisionBox.x < 0.0f && mVelocity.x < 0.0f)
	{
		// Now check to see if we are at the bottom of the screen
		if (int(mPosition.y + 0.05) == levelMap->GetLevelHeight() - 1)
		{
			return true;
		}
		else
		{
			mPosition.x = levelMap->GetLevelWidth() - 0.01;
		}
	}

	return false;
}

// --------------------------------------------------------------- //

void RenderObject::UpdateAnimations(const float deltaTime)
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
}

// --------------------------------------------------------------- //

void RenderObject::SetPOWHit()
{
	AddVelocity(Vector2D(0.0f, -2.0f));
}

// --------------------------------------------------------------- //