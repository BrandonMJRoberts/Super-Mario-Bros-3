#include "Para_Goomba_SMB3.h"

#include <sstream>

bool		 ParaGoomba::mUpdatedStaticVariables(false);

// ------------------------------------------------------------------------------------------------ //

ParaGoomba::ParaGoomba(const Vector2D      spawnPosition,
	const bool          startSpawnedInLevel,
	SDL_Renderer*       renderer,
	const std::string   filePathToSpriteSheet,
	const unsigned int  spritesOnWidth,
	const unsigned int  spritesOnHeight,
	const double        collisionBoxWidth,
	const double        collisionBoxHeight,
	const float		    timePerFrame,
	const bool          canMove,
	const bool          canJump,
	const bool          startFacingLeft)

: Goomba(spawnPosition
, startSpawnedInLevel
, renderer
, filePathToSpriteSheet
, spritesOnWidth
, spritesOnHeight
, collisionBoxWidth
, collisionBoxHeight
, timePerFrame
, canMove
, canJump
, startFacingLeft)

, mTimePerFrame(timePerFrame)
, mJumpForce(4.0f)
, mJumpsCompleted(0)
, mJumpTimer(0.15f)
, mCurrentSpriteID(3)
, mStartSpriteID(3)
, mEndSpriteID(4)
, mTimeRemainingTillNextFrame(mTimePerFrame)
, kTimePerJump(0.25f)
, kTimePerJumpLoop(2.5f)
, mHitDelay(0.0f)
{
	mHitsRemaining              = 2;
	mTimeRemainingTillNextFrame = mTimePerFrame;
}

// ------------------------------------------------------------------------------------------------ //

ParaGoomba::~ParaGoomba()
{

}

// ------------------------------------------------------------------------------------------------ //

BaseObject* ParaGoomba::Clone(std::string data)
{
	std::stringstream streamLine(data);

	Vector2D newPos;
	char     facingDirectionChar;

	streamLine >> newPos.x >> newPos.y >> facingDirectionChar;

	bool startMovingLeft = true;
	if (facingDirectionChar == 'R')
		startMovingLeft = false;

	if (mThisSpriteSheet)
		return new ParaGoomba(newPos, false, mRenderer, mThisSpriteSheet->GetFilePath(), mSpritesOnWidth, mSpritesOnHeight, mCollisionBox.x, mCollisionBox.y, mTimePerFrame, mCanJump, mCanJump, startMovingLeft);
	else 
		return nullptr;
}

// ------------------------------------------------------------------------------------------------ //

bool ParaGoomba::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Animation updates
	if (!mUpdatedStaticVariables)
		UpdateStaticVariables(deltaTime);

	// Handleing despawning and animations
	if (mHitsRemaining < 0)
	{
		mTimerTillDespawn -= deltaTime;

		if (mTimerTillDespawn <= 0.0f)
			return true;
	}

	// Gravity
	ApplyGravity(deltaTime);

	if (mCanMove)
		HandleMovement(deltaTime, interactionLayer, objectLayer);
	else
		mVelocity.x = 0.0f;

	mJumpTimer -= deltaTime;

	// Check for jumping
	if (mCanJump  && 
		mGrounded && 
		mJumpTimer <= 0.0f)
	{
		Jump();
	}

	return false;
}

// ------------------------------------------------------------------------------------------------ //

void ParaGoomba::Move()
{

}

// ------------------------------------------------------------------------------------------------ //

void ParaGoomba::Jump()
{
	mGrounded = false;

	// Increase counter
	mJumpsCompleted += 1;

	if (mJumpsCompleted < 5)
	{
		// Reset the timer
		mJumpTimer       = kTimePerJump;

		// Set the jump force
		mJumpForce = 4.0f;

		if(mJumpsCompleted == 3)
			mJumpForce = 12.0f;

		// Jump
		mVelocity.y -= mJumpForce;
	}
	else
	{
		mJumpTimer      = kTimePerJumpLoop;
		mJumpsCompleted = 0;
	}
}

// ------------------------------------------------------------------------------------------------ //

void ParaGoomba::Attack()
{

}

// ------------------------------------------------------------------------------------------------ //

void ParaGoomba::UpdateStaticVariables(const float deltaTime)
{
	mTimeRemainingTillNextFrame -= deltaTime;

	if (mTimeRemainingTillNextFrame <= 0.0f)
	{
		mTimeRemainingTillNextFrame = mTimePerFrame;

		mCurrentSpriteID++;

		if (mCurrentSpriteID > mEndSpriteID)
		{
			mCurrentSpriteID = mStartSpriteID;
		}
	}

	mUpdatedStaticVariables = true;
}

// ------------------------------------------------------------- //

void ParaGoomba::Render(const Vector2D renderReferencePoint)
{
	RenderSprite(renderReferencePoint, mCurrentSpriteID);
}

// ------------------------------------------------------------- //

ObjectCollisionHandleData ParaGoomba::SetIsCollidedWith(TwoDimensionalCollision collisionData, const unsigned int playerMovements, const bool isPlayer)
{
	if (!isPlayer)
		return ObjectCollisionHandleData();

	if (collisionData.playerPriorPosition.y <= mCurrentPosition.y - mCollisionBox.y &&
		collisionData.collisionDataPrimary == MOVEMENT_DIRECTION::DOWN)
	{
		mHitsRemaining--;

		if (mHitsRemaining > 0)
		{
			mCanJump        = false;

			return ObjectCollisionHandleData(false, false, true, false, true, false);
		}
		else
		{
			mCanMove         = false;

			mCurrentSpriteID = 5;
			mEndSpriteID     = 5;
			mStartSpriteID   = 5;

			return ObjectCollisionHandleData(false, false, false, false, true, false);
		}
	}

	if(mCanMove)
		return ObjectCollisionHandleData(false, false, true, false, true, true);
	else
		return ObjectCollisionHandleData(false, false, false, false, false, false);
}

// ------------------------------------------------------------- //