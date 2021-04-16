#include "Koopa_Trooper_SMB3.h"

#include "Animation_Speeds.h"

#include <sstream>

// ------------------------------------------------------------------------------ //

KoopaTrooper::KoopaTrooper(const Vector2D      spawnPosition,
	const bool          startSpawnedInLevel,
	SDL_Renderer*       renderer,
	const std::string   filePathToSpriteSheet,
	const unsigned int  spritesOnWidth,
	const unsigned int  spritesOnHeight,
	const double        collisionBoxWidth,
	const double        collisionBoxHeight,
	const float			timePerFrame,
	const bool          canMove,
	const bool          canJump,
	const bool          startFacingLeft,
	const char          colourIndexOfKoopa)

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

, mColourStartID(0)
, mColourIndexOfKoopa(colourIndexOfKoopa)
, mTimePerFrame(timePerFrame)
, mHitsRemaining(1)
, kTimeOnFloor(5.0f)
, mTimeTillConversion(0.0f)

, mStartSpriteID(0)
, mEndSpriteID(0)
, mCurrentSpriteID(0)

, mUpdatedStaticVariables(false)

, mTimeRemainingTillNextFrame(0.0f)
, mDelayTillNextHit(0.0f)

, kMaxTimePerFrame(timePerFrame)
{
	if(mFacingLeft)
		mVelocity.x = -MOVEMENT_SPEED;
	else
		mVelocity.x = MOVEMENT_SPEED;

	switch (colourIndexOfKoopa)
	{
	default:
	case 'R':
		mColourStartID = 7;
	break;
	
	case 'G':
		mColourStartID = 0;
	break;

	case 'B':
		mColourStartID = 28;
	break;
	}

	mStartSpriteID   = mColourStartID;
	mEndSpriteID     = mStartSpriteID + 1;
	mCurrentSpriteID = mStartSpriteID;
}

// ------------------------------------------------------------------------------ //

KoopaTrooper::~KoopaTrooper()
{

}

// ------------------------------------------------------------------------------ //

BaseObject* KoopaTrooper::Clone(std::string dataLine)
{
	std::stringstream data(dataLine);

	Vector2D newPos;
	char     charPlaceHolder;

	data >> newPos.x >> newPos.y >> charPlaceHolder;

	bool startFacingLeft = true;

	if (charPlaceHolder == 'R')
		startFacingLeft = false;

	data >> charPlaceHolder;

	if (mThisSpriteSheet)
	{
		return new KoopaTrooper(newPos, false, mRenderer, mThisSpriteSheet->GetFilePath(), mSpritesOnWidth, mSpritesOnHeight, mCollisionBox.x, mCollisionBox.y, mTimePerFrame, true, mCanJump, startFacingLeft, charPlaceHolder);
	}
	else 
		return nullptr;
}

// ------------------------------------------------------------------------------ //

bool KoopaTrooper::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Animation updates
	if (!mUpdatedStaticVariables)
		UpdateStaticVariables(deltaTime);

	ApplyGravity(deltaTime);

	HandleMovement(deltaTime, interactionLayer, objectLayer);

	if (mHitsRemaining == 0)
	{
		mTimeTillConversion -= deltaTime;

		if (mTimeTillConversion <= 0.0f)
		{
			mHitsRemaining = 1;

			mTimePerFrame = KOOPA_WALK_ANIMATION_SPEED;

			mStartSpriteID   = mColourStartID;
			mEndSpriteID     = mStartSpriteID + 1;
			mCurrentSpriteID = mStartSpriteID;

			mCanMove         = true;

			mFacingLeft      = true;
			mVelocity.x      = -MOVEMENT_SPEED;

			mTimePerFrame    = kMaxTimePerFrame;
		}
	}

	if(mDelayTillNextHit > 0.0f)
		mDelayTillNextHit -= deltaTime;

	return false;
}

// ------------------------------------------------------------------------------ //

void KoopaTrooper::UpdateStaticVariables(const float deltaTime)
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

void KoopaTrooper::Render(const Vector2D renderReferencePoint)
{
	RenderSprite(renderReferencePoint, mCurrentSpriteID);
}

// ------------------------------------------------------------- //

ObjectCollisionHandleData KoopaTrooper::SetIsCollidedWith(TwoDimensionalCollision collisionData, const unsigned int playerMovements, const bool isPlayer)
{
	if (!isPlayer || mDelayTillNextHit > 0.0f)
		return ObjectCollisionHandleData();

	// If koopa is up and walking
	if (mHitsRemaining == 1)
	{
		// If the player is hitting the shell from above
		if (collisionData.playerPriorPosition.y < mCurrentPosition.y - mCollisionBox.y && 
			collisionData.collisionDataPrimary == MOVEMENT_DIRECTION::DOWN)
		{
			// Stop the koopa
			mVelocity.x      = 0.0f;

			mCurrentSpriteID = mColourStartID + 2;
			mEndSpriteID     = mColourStartID + 2;
			mStartSpriteID   = mColourStartID + 2;

			mHitsRemaining--;

			mTimeTillConversion = kTimeOnFloor;

			Notify(SUBJECT_NOTIFICATION_TYPES::JUMPED_OFF_ENEMY, "");
			mDelayTillNextHit = 0.05f;

			return ObjectCollisionHandleData(false, false, true, false, true, false);
		}
		else
		{
			return ObjectCollisionHandleData(false, true, false, false, false, true);
		}
	}
	else  // Koopa is in shell mode
	{
		// If the player is hitting the shell from above
		if (collisionData.playerPriorPosition.y < mCurrentPosition.y - mCollisionBox.y &&
			collisionData.collisionDataPrimary == MOVEMENT_DIRECTION::DOWN)
		{
			mVelocity.x = 0.0f;

			mCurrentSpriteID = mColourStartID + 2;
			mEndSpriteID     = mColourStartID + 2;
			mStartSpriteID   = mColourStartID + 2;

			mTimeTillConversion = kTimeOnFloor;

			Notify(SUBJECT_NOTIFICATION_TYPES::JUMPED_OFF_ENEMY, "");

			mDelayTillNextHit = 0.05f;

			return ObjectCollisionHandleData(false, false, true, false, true, false);
		}
		else
		{
			// If the shell is stopped
			if (mVelocity.x == 0.0f)
			{
				if (collisionData.playerPriorPosition.x >= mCurrentPosition.x + (mCollisionBox.x / 2.0f))
				{
					mVelocity.x = -6.0f;
				}
				else
				{
					mVelocity.x = 6.0f;
				}

				mTimePerFrame     = kMaxTimePerFrame / 2.0f;

				mCurrentSpriteID  = mColourStartID + 2;
				mEndSpriteID      = mColourStartID + 5;
				mStartSpriteID    = mColourStartID + 2;

				mDelayTillNextHit   = 0.05f;
				mTimeTillConversion = 10000.0f; // Make the koopa essentially never get up

				return ObjectCollisionHandleData(false, true, false, false, false, false);
			}
			else
			{
				// Player has been hit so just return that
				return ObjectCollisionHandleData(false, false, false, false, false, true);
			}
		}
	}

	return ObjectCollisionHandleData();
}

// ------------------------------------------------------------- //