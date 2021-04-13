#include "Goomba_SMB3.h"

#include <sstream>

bool		 Goomba::mUpdatedStaticVariables(false);

// ------------------------------------------------------------- //

Goomba::Goomba(const Vector2D      spawnPosition,
	           const bool          startSpawnedInLevel,
	           SDL_Renderer* renderer,
	           const std::string   filePathToSpriteSheet,
			   const unsigned int  spritesOnWidth,
			   const unsigned int  spritesOnHeight,
			   const double        collisionBoxWidth,
			   const double        collisionBoxHeight,
		       const float		    timePerFrame,
			   const bool          canMove,
		       const bool          canJump,
			   const bool          startFacingLeft)

: EnemyObject(spawnPosition
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
, mTimerTillDespawn(1.0f)
, mHitsRemaining(1)
, mCurrentSpriteID(0)
, mStartSpriteID(0)
, mEndSpriteID(1)
, mTimeRemainingTillNextFrame(0.0f)
{
	mVelocity.x = -MOVEMENT_SPEED;
}

// ------------------------------------------------------------- //

Goomba::~Goomba()
{

}

// ------------------------------------------------------------- //

BaseObject* Goomba::Clone(std::string data)
{
	// First extra the data from the line passed in
	std::stringstream dataLine(data);

	Vector2D newPos;
	char     facingDirection;

	dataLine >> newPos.x >> newPos.y >> facingDirection;

	bool startFacingLeft = facingDirection == 'L' ? true : false;

	if (mThisSpriteSheet)
		return new Goomba(newPos, false, mRenderer, mThisSpriteSheet->GetFilePath(), mSpritesOnWidth, mSpritesOnHeight, mCollisionBox.x, mCollisionBox.y, mTimePerFrame, mCanMove, mCanJump, startFacingLeft);
	else
		return nullptr;
}

// ------------------------------------------------------------- //

bool Goomba::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Animation updates
	if (!mUpdatedStaticVariables)
		UpdateStaticVariables(deltaTime);

	// Handleing despawning and animations
	if (mHitsRemaining == 0)
	{
		mTimerTillDespawn -= deltaTime;

		if (mTimerTillDespawn <= 0.0f)
			return true;
	}

	ApplyGravity(deltaTime);

	if (mCanMove)
		HandleMovement(deltaTime, interactionLayer, objectLayer);
	else
		mVelocity.x = 0.0f;

	return false;
}

// ------------------------------------------------------------- //

void Goomba::Move()
{
	// Now move the goomba in its current facing direction
	if (mFacingLeft)
		mVelocity.x = -MOVEMENT_SPEED;
	else
		mVelocity.x = MOVEMENT_SPEED;
}

// ------------------------------------------------------------- //

void Goomba::Jump()
{

}

// ------------------------------------------------------------- //

void Goomba::Attack()
{

}

// ------------------------------------------------------------- //

void Goomba::UpdateStaticVariables(const float deltaTime)
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

void Goomba::Render(const Vector2D renderReferencePoint)
{
	if(mHitsRemaining == 0)
		RenderSprite(renderReferencePoint, 2);
	else
		RenderSprite(renderReferencePoint, mCurrentSpriteID);

}

// ------------------------------------------------------------- //

ObjectCollisionHandleData Goomba::SetIsCollidedWith(TwoDimensionalCollision collisionData, const unsigned int playerMovements, const bool isPlayer)
{
	if (!isPlayer)
		return ObjectCollisionHandleData();

	if (mHitsRemaining == 0)
	{
		return ObjectCollisionHandleData(false, false, false, false, false, false);
		mCanMove = false;
	}

	if (collisionData.playerPriorPosition.y < mCurrentPosition.y - mCollisionBox.y && collisionData.collisionDataPrimary == MOVEMENT_DIRECTION::DOWN)
	{
		// Stop the goomba
		mCanMove  = false;

		if (mHitsRemaining > 0)
		{
			mHitsRemaining--;
		}

		if(mHitsRemaining == 0)
			mCanMove = false;

		Notify(SUBJECT_NOTIFICATION_TYPES::JUMPED_OFF_ENEMY, "");

		return ObjectCollisionHandleData(false, false, true, false, true, false);
	}

	if( collisionData.collisionDataPrimary   == MOVEMENT_DIRECTION::NONE  || 
		collisionData.collisionDataSecondary == MOVEMENT_DIRECTION::NONE  || 
		collisionData.collisionDataSecondary == MOVEMENT_DIRECTION::RIGHT || 
		collisionData.collisionDataSecondary == MOVEMENT_DIRECTION::LEFT)
	{
		return ObjectCollisionHandleData(false, true, false, false, false, true);
	}

	return ObjectCollisionHandleData();
}

// ------------------------------------------------------------- //