#include "PlayableCharacter.h"

#include "Constants_SMB3.h"

#include "InteractionLayer.h"
#include "ObjectLayer.h"

#include <math.h>

// ----------------------------------------------------- //

PlayableCharacter::PlayableCharacter(SDL_Renderer* renderer, const char* filePathToSpriteSheet, Vector2D spawnPoint, const Vector2D levelBounds, const float timePerFrame)
: Subject()
, mRealGridPosition(spawnPoint)
, mScreenGridPosition(0, 0)
, mRenderRefencePoint(spawnPoint)

, mVelocity(0, 0)
, mAcceleration(0, 0)

, mIsAlive(true)

, mSpriteSheet(nullptr)
, mSingleSpriteWidth(0)
, mSingleSpriteHeight(0)

, mCurrentFrame(0)
, mStartFrame(0)
, mEndFrame(0)
, mNumberOfSpritesOnWidth(0)
, mNumberOfSpritesOnHeight(0)
, mLevelBounds(levelBounds)

, mCollisionBox(1.0f, 1.0f)
, mCollisionBoxOffset(0.0f, 0.0f)

, kBaseMaxHorizontalSpeed(5.0f)
, mMaxHorizontalSpeed(kBaseMaxHorizontalSpeed)
, kMaxHorizontalSpeedOverall(7.5f)
, mPSpeedAccumulatorRate(2.0f)

, kFrictionMultiplier(9.0f)

, kJumpHeldAccelerationDepreciationRate(16.0f)
, mJumpInitialBoost(-14.0f)
, kJumpHeldInitialBoost(-17.0f)
, mJumpHeldCurrentBoost(kJumpHeldInitialBoost)

, mCurrentMovements(MovementBitField::NONE)

, mTimePerFrame(timePerFrame)
, mTimeTillNextFrame(timePerFrame)
, mPriorFrameMovements(0)

, mPowerUpState(POWER_UP_TYPE::NONE)
, mRenderer(renderer)
, mWasFacingRight(true)
, mGrounded(true)
, mHasControl(true)

, mJumpTimerLeway(0.0f)
{
	LoadInCorrectSpriteSheet();

	// Now calculate where the starting screen space position the player should be
	CalculateScreenBoundsPosition(spawnPoint);

	// Now calculate the starting render reference point
	CalculateInitialRenderReferencePoint();
}

// ----------------------------------------------------- //

PlayableCharacter::~PlayableCharacter()
{
	delete mSpriteSheet;
	mSpriteSheet = nullptr;
}

// ----------------------------------------------------- //

void PlayableCharacter::Render()
{
	if (mSpriteSheet)
	{
		// First calculate what we should render - which sprite
		SDL_Rect portionOfSpriteSheet {int((mCurrentFrame % mNumberOfSpritesOnWidth) * mSingleSpriteWidth),
									   int((mCurrentFrame / mNumberOfSpritesOnWidth)* mSingleSpriteHeight),
			                           int(mSingleSpriteWidth), 
			                           int(mSingleSpriteHeight) };

		// Now calculate where we should render it
		SDL_Rect destRect {int(mScreenGridPosition.x * RESOLUTION_OF_SPRITES), 
			               int(mScreenGridPosition.y * RESOLUTION_OF_SPRITES) - int(float(RESOLUTION_OF_SPRITES) * mCollisionBox.y) + 2, // Render in the position, moved up the height of the sprite + 1 to make it look like he is running on the ground
			               int(mSingleSpriteWidth), 
			               int(mSingleSpriteHeight) };


		// If going right
		if (mCurrentMovements & MovementBitField::MOVING_RIGHT)
		{
			// Render it facing Right
			mSpriteSheet->Render(portionOfSpriteSheet, destRect, SDL_FLIP_HORIZONTAL);
		}
		else if (mCurrentMovements & MovementBitField::MOVING_LEFT) // If going left
		{
			// Render it facing left
			mSpriteSheet->Render(portionOfSpriteSheet, destRect, SDL_FLIP_NONE);
		}
		else // Not pressing a direction so go off the prior direction
		{
			// If we were going right and are now not moving
			if (mWasFacingRight)
			{
				// Then render facing right
				mSpriteSheet->Render(portionOfSpriteSheet, destRect, SDL_FLIP_HORIZONTAL);
			}
			else
			{
				mSpriteSheet->Render(portionOfSpriteSheet, destRect, SDL_FLIP_NONE);
			}
		}
	}
	else
		std::cout << "Invalid sprite sheet trying to be rendered from: Base Character" << std::endl;
}

// ----------------------------------------------------- //

void PlayableCharacter::Update(const float deltaTime, SDL_Event e, const Vector2D levelBounds, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// If the player has no control then just dont change anything
	if (!mHasControl)
		return;

	// Update the jumping leway
	if(mJumpTimerLeway > 0.0f)
		mJumpTimerLeway -= deltaTime;

	// First handle input to see if the player wants to move in a direction
	HandleMovementInput(e);

	// Now update the player's physics
	UpdatePhysics(deltaTime);

	// Check X first
	bool collisionOnX = HandleXCollisions(deltaTime, interactionLayer, objectLayer);
	bool collisionOnY = HandleYCollisions(deltaTime, interactionLayer, objectLayer);

	// If we are not prevented from moving in both directions then calculate where we are going to be next frame
	CalculateNewPosition(deltaTime, collisionOnX, collisionOnY);

	UpdateAnimations(deltaTime);
}

// ----------------------------------------------------- //

bool PlayableCharacter::HandleXCollisions(const float deltaTime, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Default to checking right - taking into consideration the position, the collision box width, collision box offset and the movement distance
	Vector2D footPos = Vector2D(mRealGridPosition.x + mCollisionBox.x + mCollisionBoxOffset.x + (mVelocity.x * deltaTime), mRealGridPosition.y - mCollisionBoxOffset.y);
	Vector2D headPos = Vector2D(mRealGridPosition.x + mCollisionBox.x + mCollisionBoxOffset.x + (mVelocity.x * deltaTime), mRealGridPosition.y - mCollisionBox.y - mCollisionBoxOffset.y);

	// Check if we are going left - if we are then check left
	if (mVelocity.x < 0.0f)
	{
		// Going left
		footPos = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x + (mVelocity.x * deltaTime), mRealGridPosition.y - mCollisionBoxOffset.y);
		headPos = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x + (mVelocity.x * deltaTime), mRealGridPosition.y - mCollisionBox.y - mCollisionBoxOffset.y);
	}

	return (CheckXCollision(footPos, headPos, interactionLayer, objectLayer));
}

// ----------------------------------------------------- //

bool PlayableCharacter::HandleYCollisions(const float deltaTime, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Default to going downwards
	Vector2D leftPos  = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x,                   mRealGridPosition.y - mCollisionBoxOffset.y + (mVelocity.y * deltaTime));
	Vector2D rightPos = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x + mCollisionBox.x, mRealGridPosition.y - mCollisionBoxOffset.y + (mVelocity.y * deltaTime));

	// Going downwards
	if (mVelocity.y >= 0.0f)
	{
		if (CheckYCollision(leftPos, rightPos, interactionLayer, objectLayer))
		{
			mCurrentMovements &= ~(MovementBitField::JUMPING);
			mCurrentMovements &= ~(MovementBitField::HOLDING_JUMP);

			if (!(mCurrentMovements & MovementBitField::JUMPING))
			{
				mCurrentMovements &= ~(MovementBitField::JUMPING);
				mCurrentMovements &= ~(MovementBitField::HOLDING_JUMP);
			}

			mGrounded = true;
			return true;
		}
		else
			mGrounded = false;
	}
	else // Going upwards
	{
		leftPos  = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x,                   mRealGridPosition.y - mCollisionBox.y - mCollisionBoxOffset.y + (mVelocity.y * deltaTime));
		rightPos = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x + mCollisionBox.x, mRealGridPosition.y - mCollisionBox.y - mCollisionBoxOffset.y + (mVelocity.y * deltaTime));

		mGrounded = false;

		return (CheckYCollision(leftPos, rightPos, interactionLayer, objectLayer));
	}

	return false;
}

// ----------------------------------------------------- //

bool PlayableCharacter::CheckXCollision(const Vector2D positionToCheck1, const Vector2D positionToCheck2, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Check to see if we have hit the terrain on the X movement, and the objects in the level, and the screen constraints
	if (HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck1)                || HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck2)                ||
		HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck1).StopXMovement || HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck2).StopXMovement ||
		positionToCheck1.x < 0.0f ||
		positionToCheck2.x < 0.0f ||
		positionToCheck1.x > mLevelBounds.x ||
		positionToCheck2.x > mLevelBounds.x)
	{
		// Return that there was a collision
		return true;
	}

	// Return that there was not a collision
	return false;
}

// ----------------------------------------------------- //

bool PlayableCharacter::CheckYCollision(const Vector2D positionToCheck1, const Vector2D positionToCheck2, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Check terrain collision
	if (HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck1) || HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck2) ||
		positionToCheck1.y < 0.0f ||
		positionToCheck2.y < 0.0f ||
		positionToCheck1.y + mCollisionBox.y > mLevelBounds.y + 2 ||
		positionToCheck2.y + mCollisionBox.y > mLevelBounds.y + 2)
	{
		// Return that there was a collision
		return true;
	}

	MovementPrevention returnData1 = HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck1), returnData2 = HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck2);

	if (returnData1.StopYMovement || returnData2.StopYMovement)
	{
		if(returnData1.givesJump || returnData2.givesJump)
			mJumpTimerLeway = 0.1f;

		// Return that there was a collision
		return true;
	}

	// Return that there was not a collision
	return false;
}

// ----------------------------------------------------- //

void PlayableCharacter::CalculateScreenBoundsPosition(const Vector2D spawnPoint)
{
	// If the player is near a boundary then they need specific positioning - starting with the Y axis
	if (spawnPoint.y + (PLAYABLE_SCREEN_AREA_HEIGHT / 2) > mLevelBounds.y)
	{
		mScreenGridPosition.y = (BACKGROUND_SPRITE_RENDER_HEIGHT - 1.0f) - (mLevelBounds.y - spawnPoint.y);
	}
	else if (spawnPoint.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2) < 0.0f)
	{
		mScreenGridPosition.y = spawnPoint.y;
	}
	else
	{
		mScreenGridPosition.y = LEVEL_BOUNDING_AREA_HEIGHT / 2;
	}

	// Now for the X axis
	if (spawnPoint.x + (PLAYABLE_SCREEN_AREA_WIDTH / 2) > mLevelBounds.x)
	{
		mScreenGridPosition.x = mLevelBounds.x - spawnPoint.x;
	}
	else if (spawnPoint.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2) < 0.0f)
	{
		mScreenGridPosition.x = spawnPoint.x;
	}
	else
	{
		mScreenGridPosition.x = LEVEL_BOUNDING_AREA_WIDTH / 2;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::CalculateNewPosition(const float deltaTime, bool xCollisionOccured, bool yCollisionOccured)
{
	// Calculate the new potential positions
	Vector2D movementDistance = (mVelocity * deltaTime);

	// Calculate the new potential positions
	Vector2D newRealGridPos   = mRealGridPosition   + movementDistance;
	Vector2D newScreenGridPos = mScreenGridPosition + movementDistance;

	// First cap the new positions to their relative bounds - starting with the y axis
	if (newScreenGridPos.y <= 0.0f)
		newScreenGridPos.y = 0.0f;
	else if (newScreenGridPos.y >= PLAYABLE_SCREEN_AREA_HEIGHT + 4.0f)
		newScreenGridPos.y = PLAYABLE_SCREEN_AREA_HEIGHT + 4.0f;

	if (newRealGridPos.y <= 0.0f)
		newRealGridPos.y = 0.0f;
	else if (newRealGridPos.y >= mLevelBounds.y + 2)
		newRealGridPos.y = mLevelBounds.y + 2;

	// Now for the x axis
	if (newScreenGridPos.x <= 0.0f)
		newScreenGridPos.x = 0.0f;
	else if (newScreenGridPos.x >= (PLAYABLE_SCREEN_AREA_WIDTH - 1.0f))
		newScreenGridPos.x = (PLAYABLE_SCREEN_AREA_HEIGHT - 1.0f);

	if (newRealGridPos.x <= -1.0f)
		newRealGridPos.x = -1.0f;
	else if (newRealGridPos.x >= mLevelBounds.x + 1)
		newRealGridPos.x = mLevelBounds.x + 1;

	// ----------------------------------------------------------------------------------------------------------------------------------- //

	// Update the players position is the collision has not occured
	if (!xCollisionOccured)
	{
		mRealGridPosition.x = newRealGridPos.x;

		// If we want to scroll the screen then scroll - X axis
		if (newScreenGridPos.x > (PLAYABLE_SCREEN_AREA_WIDTH - (PLAYABLE_SCREEN_AREA_WIDTH / 2.0f)) + 1 && movementDistance.x > 0.0f)
		{
			// If moving in the correct direction then scroll the screen
			mRenderRefencePoint.x += movementDistance.x;
		}
		else if (newScreenGridPos.x < ((PLAYABLE_SCREEN_AREA_WIDTH / 2.0f) - 1) && movementDistance.x < 0.0f)
		{
			mRenderRefencePoint.x += movementDistance.x;
		}
		else // If we are within this leway area then don't scroll the screen
		{
			// Allow the player to move freely
			mScreenGridPosition.x = newScreenGridPos.x;
		}

		if (mRealGridPosition.x + (PLAYABLE_SCREEN_AREA_WIDTH / 2) - 1 > mLevelBounds.x ||
			mRealGridPosition.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2) + 1 < 0.0f)
		{
			mScreenGridPosition.x = newScreenGridPos.x;
		}
	}
	else
		mVelocity.x = 0.0f;

	if (!yCollisionOccured)
	{
		mRealGridPosition.y = newRealGridPos.y;

		// If we want to scroll the screen then scroll - Y axis
		if (newScreenGridPos.y > PLAYABLE_SCREEN_AREA_HEIGHT - (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) + 2 && movementDistance.y > 0.0f)
		{
			mRenderRefencePoint.y += movementDistance.y;
		}
		else if (newScreenGridPos.y < (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) && movementDistance.y < 0.0f)
		{
			mRenderRefencePoint.y += movementDistance.y;
		}
		else
		{
			// Allow the player to move freely
			mScreenGridPosition.y = newScreenGridPos.y;
		}

		if (mRealGridPosition.y + (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) > mLevelBounds.y ||
			mRealGridPosition.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) < 0.0f)
		{
			mScreenGridPosition.y = newScreenGridPos.y;
		}
	}
	else
		mVelocity.y = 0.0f;

	// ----------------------------------------------------------------------------------------------------------------------------------- //

	// Cap the render reference point
	if (mRenderRefencePoint.x < 0.0f)
	{
		mRenderRefencePoint.x = 0.0f;
	}
	else if (mRenderRefencePoint.x > mLevelBounds.x - BACKGROUND_SPRITE_RENDER_WIDTH)
	{
		mRenderRefencePoint.x = mLevelBounds.x - BACKGROUND_SPRITE_RENDER_WIDTH;
	}

	// ----------------------------------------------------------------------------------------------------------------------------------- //

	if (mRenderRefencePoint.y < 0.0f)
	{
		mRenderRefencePoint.y = 0.0f;
	}
	else if (mRenderRefencePoint.y > mLevelBounds.y - BACKGROUND_SPRITE_RENDER_HEIGHT + 1)
	{
		mRenderRefencePoint.y = mLevelBounds.y - BACKGROUND_SPRITE_RENDER_HEIGHT + 1;
	}

	// ----------------------------------------------------------------------------------------------------------------------------------- //
}

// ----------------------------------------------------- //

void PlayableCharacter::HandleMovementInput(SDL_Event e)
{
	double speed      = 15.0f;
	double multiplier = 1.0f;

	// If the player is currently running then set the speed to be correct
	if (mCurrentMovements & MovementBitField::RUNNING)
	{
		multiplier = 1.1f;
	}

	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_w:
			// If grounded then set that the player is jumping
			if (mGrounded || !(mCurrentMovements & MovementBitField::JUMPING) || mJumpTimerLeway > 0.0f)
			{
				// Set that the player is jumping
				mCurrentMovements |= MovementBitField::JUMPING;

				mJumpHeldCurrentBoost = kJumpHeldInitialBoost * std::max(float(abs(mVelocity.x) / kMaxHorizontalSpeedOverall), 0.1f);

				mGrounded = false;

				// Give the minimum jump height of boost upwards
				mVelocity.y = mJumpInitialBoost;

				Notify(SUBJECT_NOTIFICATION_TYPES::PLAYER_JUMPED, "");
			}

			// Otherwise check if you are jumping and if so state that you are holding down jump
			if (mCurrentMovements & MovementBitField::JUMPING)
			{
				mCurrentMovements |= MovementBitField::HOLDING_JUMP;
			}
		break;

		case SDLK_RSHIFT:
			// If not running
			if (!(mCurrentMovements & MovementBitField::RUNNING))
			{
				// Store that the player is now running
				mCurrentMovements |= MovementBitField::RUNNING;
			}
		break;

		case SDLK_d:
			mWasFacingRight = false;

			mCurrentMovements |= MovementBitField::MOVING_RIGHT;

			mAcceleration.x = multiplier * speed;
		break;

		case SDLK_a:
			mCurrentMovements |= MovementBitField::MOVING_LEFT;

			mAcceleration.x = -multiplier * speed;
		break;

		case SDLK_s:
			mCurrentMovements |= MovementBitField::CROUCHING;

			mAcceleration.y = multiplier * speed;
		break;
		}
	break;

	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_RSHIFT:
			// Set the player to not be running
			mCurrentMovements &= ~(MovementBitField::RUNNING);

			mMaxHorizontalSpeed = kBaseMaxHorizontalSpeed;
		break;

		case SDLK_d:
			mWasFacingRight = true;

			mCurrentMovements &= ~(MovementBitField::MOVING_RIGHT);

			mAcceleration.x = 0.0f;
		break;

		case SDLK_a:
			mWasFacingRight = false;

			mCurrentMovements &= ~(MovementBitField::MOVING_LEFT);

			mAcceleration.x = 0.0f;
		break;

		case SDLK_s:
			mCurrentMovements &= ~(MovementBitField::CROUCHING);
		break;

		case SDLK_w:
			//mCurrentMovements &= ~(MovementBitField::JUMPING);

			// Not holding jump anymore
			mCurrentMovements &= ~(MovementBitField::HOLDING_JUMP);

			// If you release jump whilst jumping you cannot continue the extra boost after the release
			mJumpHeldCurrentBoost = 0.0f;

			mAcceleration.y = 0.0f;
		break;
		}
	break;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::CalculateInitialRenderReferencePoint()
{
	// Starting with the Y axis
	if (mRealGridPosition.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2) <= 0.0f)
	{
		// Set the correct reference point
		mRenderRefencePoint.y = 0.0f;
	}
	else if (mRealGridPosition.y + ((PLAYABLE_SCREEN_AREA_HEIGHT / 2) + 3) >= mLevelBounds.y)
	{
		// Set the correct reference point
		mRenderRefencePoint.y = mLevelBounds.y - (BACKGROUND_SPRITE_RENDER_HEIGHT - 1);
	}

	// Now for the X-axis
	if (mRealGridPosition.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2) < 0.0f)
	{
		mRenderRefencePoint.x = 0.0f;
	}
	else if (mRealGridPosition.x + (PLAYABLE_SCREEN_AREA_WIDTH / 2) >= mLevelBounds.x)
	{
		mRenderRefencePoint.x = mLevelBounds.x - PLAYABLE_SCREEN_AREA_WIDTH;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::SpawnIntoNewArea(const Vector2D newPos, const Vector2D newLevelBounds)
{
	mRealGridPosition = newPos; 

	mLevelBounds      = newLevelBounds;

	CalculateScreenBoundsPosition(newPos);

	// Now calculate the starting render reference point
	CalculateInitialRenderReferencePoint();
}

// ----------------------------------------------------- //

bool PlayableCharacter::HandleCollisionsWithInteractionLayer(InteractableLayer* interactionLayer, const Vector2D newPos)
{
	return (interactionLayer->GetIsCollision((unsigned int)newPos.y, (unsigned int)newPos.x));
}

// ----------------------------------------------------- //

MovementPrevention PlayableCharacter::HandleCollisionsWithInteractionObjectLayer(ObjectLayer* objectLayer, const Vector2D newPos)
{
	return (objectLayer->CheckCollision(newPos, mVelocity, mRealGridPosition));
}

// ----------------------------------------------------- //

void PlayableCharacter::UpdatePhysics(const float deltaTime)
{
	// Apply friction to the player's movement 
	float frictionReduction = (kFrictionMultiplier * deltaTime);

	// Apply friction to the player's X movement
	if (abs(mVelocity.x) - frictionReduction < 0.0f)
		mVelocity.x = 0.0f;
	else
	{
		if (mVelocity.x > 0.0f)
			mVelocity.x -= frictionReduction;
		else if (mVelocity.x < 0.0f)
			mVelocity.x += frictionReduction;
	}

	// Jumping calculations - you only get the extra jump height if you are holding run, otherwise it is just the regular jump
	if (mCurrentMovements & MovementBitField::HOLDING_JUMP && mCurrentMovements & MovementBitField::RUNNING)
	{
		// If holding down jump then keep mario going up until a point
		mAcceleration.y = mJumpHeldCurrentBoost;

		// Reduce the current extra being added onto the jump
		mJumpHeldCurrentBoost += (kJumpHeldAccelerationDepreciationRate * deltaTime);

		// Bounds check
		if (mJumpHeldCurrentBoost >= 0.0f)
			mJumpHeldCurrentBoost = 0.0f;
	}

	// Apply the acceleration of the player 
	mVelocity += Vector2D(mAcceleration.x * deltaTime, (mAcceleration.y + GRAVITY) * deltaTime);

	// If walking
	if (!(mCurrentMovements & MovementBitField::RUNNING))
	{
		// Cap the velocity to the max speed if it exceeds it
		if (mVelocity.x > kBaseMaxHorizontalSpeed)
			mVelocity.x = kBaseMaxHorizontalSpeed;
		else if(mVelocity.x < -kBaseMaxHorizontalSpeed)
			mVelocity.x = -kBaseMaxHorizontalSpeed;
	}
	else
	{
		// Cap the velocity to the max speed if it exceeds it
		if (abs(mVelocity.x) > mMaxHorizontalSpeed)
		{
			mMaxHorizontalSpeed += mPSpeedAccumulatorRate * deltaTime;

			if (mVelocity.x > 0.0f)
				mVelocity.x = mMaxHorizontalSpeed;
			else
				mVelocity.x = -mMaxHorizontalSpeed;
		}

		if (mMaxHorizontalSpeed > kMaxHorizontalSpeedOverall)
			mMaxHorizontalSpeed = kMaxHorizontalSpeedOverall;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimations(const float deltaTime)
{
	mTimeTillNextFrame -= deltaTime;

	// Check to see if we should move the frame on
	if (mTimeTillNextFrame <= 0.0f)
	{
		mCurrentFrame++;
		mTimeTillNextFrame = mTimePerFrame;

		if (mCurrentFrame > mEndFrame)
			mCurrentFrame = mStartFrame;
	}

	// Now check to see if we should change animation
	if (mPriorFrameMovements != mCurrentMovements)
	{
		switch (mPowerUpState)
		{
		default:
		case POWER_UP_TYPE::NONE:
			UpdateAnimationsSmallMario();
		break;

		case POWER_UP_TYPE::MUSHROOM:
			UpdateAnimationsLargeMario();
		break;

		case POWER_UP_TYPE::FIRE_FLOWER:
			UpdateAnimationsFireMario();
		break;

		case POWER_UP_TYPE::TANOOKI_SUIT:
			UpdateAnimationsTanookiMario();
		break;

		case POWER_UP_TYPE::FROG_SUIT:
			UpdateAnimationsFrogMario();
		break;

		case POWER_UP_TYPE::HAMMER_SUIT:
			UpdateAnimationsHammerMario();
		break;

		case POWER_UP_TYPE::STAR:
			UpdateAnimationsStarMario();
		break;

		case POWER_UP_TYPE::SUPER_LEAF:
			UpdateAnimationsLeafMario();
		break;
		}
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsSmallMario()
{
	// Check to see if the player has stopped moving
	if (mCurrentMovements == 0)
	{
		mStartFrame   = 0;
		mEndFrame     = 0;
		mCurrentFrame = 0;

		mPriorFrameMovements = mCurrentMovements;

		mCollisionBox.x       = 0.83f;
		mCollisionBoxOffset.x = 0.083f;

		return;
	}

	// Check to see if they are jumping
	if (mCurrentMovements & MovementBitField::JUMPING)
	{
		mCollisionBox.x = 0.83f;
		mCollisionBoxOffset.x = 0.083f;

		// See if this jump is a full speed jump
		if (mCurrentMovements & MovementBitField::RUNNING && abs(mVelocity.x) >= kMaxHorizontalSpeedOverall)
		{
			// Full sprint jump
			mStartFrame   = 5;
			mEndFrame     = 5;
			mCurrentFrame = mStartFrame;
		}
		else
		{
			// Regular jump
			mStartFrame   = 2;
			mEndFrame     = 2;
			mCurrentFrame = mStartFrame;
		}

		mPriorFrameMovements = mCurrentMovements;

		return;
	}

	// If turning around sharply
	if ((mCurrentMovements & MovementBitField::MOVING_RIGHT && mPriorFrameMovements & MovementBitField::MOVING_LEFT) ||
		(mCurrentMovements & MovementBitField::MOVING_LEFT  && mPriorFrameMovements & MovementBitField::MOVING_RIGHT))
	{
		mCollisionBox.x = 0.83f;
		mCollisionBoxOffset.x = 0.083f;

		mStartFrame   = 6;
		mEndFrame     = 6;
		mCurrentFrame = mStartFrame;

		mPriorFrameMovements = mCurrentMovements;

		return;
	}

	// Check for sprinting - must be going at least a certain speed in order to get this sprite 
	if (mCurrentMovements & MovementBitField::RUNNING)
	{
		mCollisionBox.x = 0.83f;
		mCollisionBoxOffset.x = 0.083f;

		if (abs(mVelocity.x) >= kBaseMaxHorizontalSpeed)
		{
			mStartFrame   = 3;
			mEndFrame     = 4;
			mCurrentFrame = mStartFrame;
		}
		else if(mVelocity.x != 0.0f)
		{
			mStartFrame   = 0;
			mEndFrame     = 1;
			mCurrentFrame = mStartFrame;
		}
		else
		{
			mStartFrame   = 0;
			mEndFrame     = 0;
			mCurrentFrame = mStartFrame;
		}

		mPriorFrameMovements = mCurrentMovements;

		return;
	}

	// Check to see if we need to start walking
	if (mCurrentMovements & MovementBitField::MOVING_RIGHT || mCurrentMovements & MovementBitField::MOVING_LEFT)
	{
		mCollisionBox.x = 0.83f;
		mCollisionBoxOffset.x = 0.083f;

		mStartFrame   = 0;
		mEndFrame     = 1;
		mCurrentFrame = mStartFrame;

		mPriorFrameMovements = mCurrentMovements;

		return;
	}

	// If going down/up a pipe
	if (mCurrentMovements & MovementBitField::ENTERING_PIPE_VERTICALLY)
	{
		mCollisionBox.x = 0.83f;
		mCollisionBoxOffset.x = 0.083f;

		mStartFrame   = 7;
		mEndFrame     = 7;
		mCurrentFrame = mStartFrame;

		mPriorFrameMovements = mCurrentMovements;

		return;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsLargeMario()
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsFrogMario()
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsHammerMario()
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsFireMario()
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsTanookiMario()
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsLeafMario()
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsStarMario()
{

}

// ----------------------------------------------------- //

void PlayableCharacter::LoadInCorrectSpriteSheet()
{
	if (mSpriteSheet)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	// Create the new sprite sheet
	mSpriteSheet = new Texture2D(mRenderer);

	switch (mPowerUpState)
	{
	default:
	case POWER_UP_TYPE::NONE:
		mSpriteSheet->LoadFromFile("SDL_Mario_Project/Characters/Mario/In Game Mario/SmallMarioSpriteSheet.png");

		mNumberOfSpritesOnWidth  = 4;
		mNumberOfSpritesOnHeight = 4;

		mCollisionBox.x = 1.0f;   // 48 pixels wide
		mCollisionBox.y = 1.0f;   // 48 pixels tall
	break;

	case POWER_UP_TYPE::MUSHROOM:
		mSpriteSheet->LoadFromFile("SDL_Mario_Project/Characters/Mario/In Game Mario/SuperMarioSpriteSheet.png");

		mNumberOfSpritesOnWidth  = 6;
		mNumberOfSpritesOnHeight = 6;

		mCollisionBox.x = 1.0f;  // 48 pixels wide
		mCollisionBox.y = 1.17f; // 56 pixels tall
	break;

	case POWER_UP_TYPE::FIRE_FLOWER:
		mSpriteSheet->LoadFromFile("SDL_Mario_Project/Characters/Mario/In Game Mario/FireMarioSpriteSheet.png");

		mNumberOfSpritesOnWidth  = 1;
		mNumberOfSpritesOnHeight = 1;

		mCollisionBox.x = 1.0f;
		mCollisionBox.y = 2.0f;
	break;

	case POWER_UP_TYPE::TANOOKI_SUIT:
		mSpriteSheet->LoadFromFile("SDL_Mario_Project/Characters/Mario/In Game Mario/TanookiMarioLuigiSpriteSheet.png");

		mNumberOfSpritesOnWidth = 1;
		mNumberOfSpritesOnHeight = 1;

		mCollisionBox.x = 1.0f;
		mCollisionBox.y = 1.25f; // 60 pixels tall
	break;

	case POWER_UP_TYPE::FROG_SUIT:
		mSpriteSheet->LoadFromFile("SDL_Mario_Project/Characters/Mario/In Game Mario/FrogMarioLuigiSpriteSheet.png");

		mNumberOfSpritesOnWidth =1 ;
		mNumberOfSpritesOnHeight = 1;

		mCollisionBox.x = 1.0f;
		mCollisionBox.y = 2.0f;
	break;

	case POWER_UP_TYPE::HAMMER_SUIT:
		mSpriteSheet->LoadFromFile("SDL_Mario_Project/Characters/Mario/In Game Mario/HammerMarioLuigiSpriteSheet.png");

		mNumberOfSpritesOnWidth = 1;
		mNumberOfSpritesOnHeight = 1;

		mCollisionBox.x = 1.0f;
		mCollisionBox.y = 2.0f;
	break;

	case POWER_UP_TYPE::STAR:
		mSpriteSheet->LoadFromFile("SDL_Mario_Project/Characters/Mario/In Game Mario/0.png");

		mNumberOfSpritesOnWidth  = 1 ;
		mNumberOfSpritesOnHeight =1 ;

		mCollisionBox.x = 1.0f;
		mCollisionBox.y = 2.0f;
	break;

	case POWER_UP_TYPE::SUPER_LEAF: // Racoon mario
		mSpriteSheet->LoadFromFile("SDL_Mario_Project/Characters/Mario/In Game Mario/RacoonMarioSpriteSheet.png");

		mNumberOfSpritesOnWidth  = 6;
		mNumberOfSpritesOnHeight = 6;

		mCollisionBox.x = 1.0f;
		mCollisionBox.y = 1.875f; // 90 pixels tall
	break;
	}

	if (!mSpriteSheet)
	{
		std::cout << "Failed to load in the sprite sheet for the playable character!" << std::endl;
		return;
	}

	mSingleSpriteWidth  = mSpriteSheet->GetWidth()  / mNumberOfSpritesOnWidth;
	mSingleSpriteHeight = mSpriteSheet->GetHeight() / mNumberOfSpritesOnHeight;
}

// ----------------------------------------------------- //
