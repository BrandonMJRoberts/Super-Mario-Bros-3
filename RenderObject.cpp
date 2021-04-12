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

	, mRotation(0.0f)
	, mDoingDeathAnimation(false)
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

	, mRotation(0.0f)
	, mDoingDeathAnimation(false)
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
		texture->Render(*mSourceRect, *mDestRect, SDL_FLIP_NONE, mRotation);
	else
		texture->Render(*mSourceRect, *mDestRect, SDL_FLIP_HORIZONTAL, mRotation);
}

// --------------------------------------------------------------- //

bool RenderObject::Update(const float deltaTime, LevelMap* levelMap)
{
	if (mDoingDeathAnimation)
	{
		mVelocity.y += CHARACTER_GRAVITY * deltaTime;
		mPosition.y += mVelocity.y * deltaTime;

		if(mVelocity.x > 0.0f)
			mRotation   += 60.0f * deltaTime;
		else
			mRotation   -= 60.0f * deltaTime;

		// Check if it has gone off the bottom of the screen
		if (mPosition.y - mCollisionBox.y > SCREEN_HEIGHT_GRID)
		{
			return true;
		}
	}
	else
	{
		if (mGrounded || mIsFlipped)
		{
			UpdateAnimations(deltaTime);
		}

		// Now update the physics of this object - this should be overriden by the child class
		UpdatePhysics(deltaTime, levelMap);

		if (ClassSpecificUpdate(deltaTime))
			return true;

		if (CheckForLooping(levelMap))
			return true;
	}

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
	bool yCollision = true;
	float checkingDistance = 0.1f;

	// first check y axis
	if(     mVelocity.y >= 0.0f 
		&& (levelMap->GetCollisionTileAt(int(mPosition.y + checkingDistance), int(mPosition.x)) // Bottom left 
		||	levelMap->GetCollisionTileAt(int(mPosition.y + checkingDistance), int(mPosition.x + mCollisionBox.x)))) // Bottom right
	{
		// Foot collision
		mPosition.y = int(mPosition.y + checkingDistance) - 0.001;

		mGrounded = true;

		mVelocity.y = 0.0f;
	}
	else if   (  mVelocity.y < 0.0f  // Head collision
		   && (  levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y - checkingDistance),  int(mPosition.x)) // Top left 
			  || levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y - checkingDistance), int(mPosition.x + mCollisionBox.x)))) // Top right
	{
		mVelocity.y = 2.0f;

		mGrounded = false;
	}
	else
	{
		mGrounded  = false;
		yCollision = false;

		mVelocity.y += CHARACTER_GRAVITY * deltaTime;
		std::cout << "GRAVITY!" << std::endl;
	}


	// X axis
	if(    mVelocity.x < 0.0f
		&& (levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y), int(mPosition.x - checkingDistance)) // Top left
		||  levelMap->GetCollisionTileAt(int(mPosition.y),                   int(mPosition.x - checkingDistance)))) // bottom left
	{
		mPosition.x = int(mPosition.x - checkingDistance) + 1.01;

		mFacingLeft = false;
		mVelocity.x = mMovementSpeed;
	}
	else if ( mVelocity.x > 0.0f
		   && ( levelMap->GetCollisionTileAt(int(mPosition.y - mCollisionBox.y), int(mPosition.x + mCollisionBox.x + checkingDistance))   // Top right
		   ||   levelMap->GetCollisionTileAt(int(mPosition.y),                   int(mPosition.x + mCollisionBox.x + checkingDistance)))) // bottom right
	{
		mPosition.x = int(mPosition.x + checkingDistance) - 0.01;

		mFacingLeft = true;
		mVelocity.x = -mMovementSpeed;
	}
	else
	{
		mPosition.x += (mVelocity * deltaTime).x;
	}

	if (!yCollision)
	{
		mPosition.y += (mVelocity * deltaTime).y;
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