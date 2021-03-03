#include "Koopa_Trooper_SMB3.h"

#include "Animation_Speeds.h"

#include <sstream>

unsigned int KoopaTrooper::mCurrentSpriteID(0);
unsigned int KoopaTrooper::mStartSpriteID(0);
unsigned int KoopaTrooper::mEndSpriteID(1);

float        KoopaTrooper::mTimeRemainingTillNextFrame(0.0f);
bool		 KoopaTrooper::mUpdatedStaticVariables(false);


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

, mColourIndexOfKoopa(colourIndexOfKoopa)
, mTimePerFrame(timePerFrame)
, mHitsRemaining(1)
, kTimeOnFloor(5.0f)
, mTimeTillConversion(0.0f)
{
	if(mFacingLeft)
		mVelocity.x = -MOVEMENT_SPEED;
	else
		mVelocity.x = MOVEMENT_SPEED;

	switch (colourIndexOfKoopa)
	{
	default:
	break;
	}
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
		}
	}

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

ObjectCollisionHandleData KoopaTrooper::SetIsCollidedWith(TwoDimensionalCollision collisionData, const unsigned int playerMovements)
{
	if (mHitsRemaining == 0)
	{
		// Stop the shell from moving if it is jumped on whilst moving
		if (mVelocity.x != 0.0f && collisionData.playerPriorPosition.y < mCurrentPosition.y - mCollisionBox.y && collisionData.collisionDataPrimary == MOVEMENT_DIRECTION::DOWN)
		{
			mVelocity.x      = 0.0f;

			mCurrentSpriteID = 2;
			mEndSpriteID     = 2;
			mStartSpriteID   = 2;

			mTimeTillConversion = kTimeOnFloor;

			return ObjectCollisionHandleData(false, false, true, false, true);
		}

		// Determine which direction the shell should be kicked
		if (collisionData.collisionDataSecondary == MOVEMENT_DIRECTION::LEFT)
		{
			mVelocity.x = -4.0f;
		}
		else if(collisionData.collisionDataSecondary == MOVEMENT_DIRECTION::RIGHT)
		{
			mVelocity.x = 4.0f;
		}

		mCurrentSpriteID = 3;
		mEndSpriteID     = 5;
		mStartSpriteID   = 3;

		mTimeTillConversion = kTimeOnFloor;
		mTimePerFrame       = KOOPA_SHELL_ANIMATION_SPEED;

		return ObjectCollisionHandleData(false, true, false, false, false);
	}

	if (collisionData.playerPriorPosition.y < mCurrentPosition.y - mCollisionBox.y && collisionData.collisionDataPrimary == MOVEMENT_DIRECTION::DOWN)
	{
		// Stop the koopa
		mVelocity.x      = 0.0f;

		mCurrentSpriteID = 2;
		mEndSpriteID     = 2;
		mStartSpriteID   = 2;

		if (mHitsRemaining > 0)
		{
			mHitsRemaining--;
		}

		mTimeTillConversion = kTimeOnFloor;

		Notify(SUBJECT_NOTIFICATION_TYPES::JUMPED_OFF_ENEMY, "");

		return ObjectCollisionHandleData(false, false, true, false, true);
	}

	if (collisionData.collisionDataSecondary == MOVEMENT_DIRECTION::RIGHT || collisionData.collisionDataSecondary == MOVEMENT_DIRECTION::LEFT)
		return ObjectCollisionHandleData(false, true, false, false, false);

	return ObjectCollisionHandleData();
}

// ------------------------------------------------------------- //