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
, mAnimationCurrentState(MovementBitField::NONE)

, kMaxYVelocity(60.0f)

, mPowerUpState(POWER_UP_TYPE::NONE)
, mRenderer(renderer)
, mWasFacingRight(true)
, mGrounded(false)
, mHasControl(true)

, mJumpTimerLeway(0.0f)
{
	LoadInCorrectSpriteSheet();

	// Now calculate where the starting screen space position the player should be
	CalculateScreenBoundsPosition(spawnPoint);

	// Now calculate the starting render reference point
	CalculateInitialRenderReferencePoint();

	// Set the collision box
	mCollisionBox.x       = 0.83f;
	mCollisionBoxOffset.x = 0.083f;

	mCollisionBox.y       = 0.83f;
	mCollisionBoxOffset.y = 0.083f;
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
									   int((mCurrentFrame / mNumberOfSpritesOnWidth) * mSingleSpriteHeight),
			                           int(mSingleSpriteWidth), 
			                           int(mSingleSpriteHeight) };

		// Now calculate where we should render it
		SDL_Rect destRect {int(mScreenGridPosition.x * RESOLUTION_OF_SPRITES), 
			               int(mScreenGridPosition.y * RESOLUTION_OF_SPRITES) - int(float(RESOLUTION_OF_SPRITES) * mCollisionBox.y), // Render in the position, moved up the height of the sprite + 2 to make it look like he is running on the ground
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

	// Check to see if there were any collisions in the new position to move to
	CollisionPositionalData collisionOnX = HandleXCollisions(deltaTime, interactionLayer, objectLayer);
	CollisionPositionalData collisionOnY = HandleYCollisions(deltaTime, interactionLayer, objectLayer);

	// Pass through the collision info into the positional calculations
	CalculateNewPosition(deltaTime, collisionOnX, collisionOnY);

	// Update the player's animations
	UpdateAnimations(deltaTime);
}

// ----------------------------------------------------- //

CollisionPositionalData PlayableCharacter::HandleXCollisions(const float deltaTime, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Default to checking right - taking into consideration the position, the collision box width, collision box offset and the movement distance
	Vector2D footPos = Vector2D(mRealGridPosition.x + mCollisionBox.x + mCollisionBoxOffset.x + (mVelocity.x * deltaTime), mRealGridPosition.y + mCollisionBoxOffset.y);
	Vector2D headPos = Vector2D(mRealGridPosition.x + mCollisionBox.x + mCollisionBoxOffset.x + (mVelocity.x * deltaTime), mRealGridPosition.y - mCollisionBox.y + mCollisionBoxOffset.y);

	// Check if we are going left - if we are then check left
	if (mVelocity.x < 0.0f)
	{
		// Going left
		footPos = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x + (mVelocity.x * deltaTime), mRealGridPosition.y + mCollisionBoxOffset.y);
		headPos = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x + (mVelocity.x * deltaTime), mRealGridPosition.y - mCollisionBox.y - mCollisionBoxOffset.y);
	}

	CollisionPositionalData returnData            = CheckXCollision(footPos, headPos, interactionLayer, objectLayer);
	returnData.collisionRealPositionLeftOrTop     = headPos;
	returnData.collisionRealPositionRightOrBottom = footPos;

	return returnData;
}

// ----------------------------------------------------- //

CollisionPositionalData PlayableCharacter::HandleYCollisions(const float deltaTime, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Default to going downwards
	Vector2D leftPos  = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x,                   mRealGridPosition.y + mCollisionBoxOffset.y + (mVelocity.y * deltaTime));
	Vector2D rightPos = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x + mCollisionBox.x, mRealGridPosition.y + mCollisionBoxOffset.y + (mVelocity.y * deltaTime));

	// Going downwards
	if (mVelocity.y >= 0.0f)
	{
		CollisionPositionalData returnData = CheckYCollision(leftPos, rightPos, interactionLayer, objectLayer);

		if (returnData.collisionOccured)
		{
			mCurrentMovements &= ~(MovementBitField::HOLDING_JUMP);

			if (!(mCurrentMovements & MovementBitField::JUMPING))
			{
				mCurrentMovements &= ~(MovementBitField::JUMPING);
				mCurrentMovements &= ~(MovementBitField::HOLDING_JUMP);
			}

			mGrounded       = true;
			return CollisionPositionalData(returnData.collisionOccured, leftPos, rightPos, returnData.collisionWithInteractionLayer, returnData.collisionWithObjectLayer);
		}
		else
			mGrounded = false;
	}
	else // Going upwards
	{
		leftPos  = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x,                   mRealGridPosition.y - mCollisionBox.y + mCollisionBoxOffset.y + (mVelocity.y * deltaTime));
		rightPos = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x + mCollisionBox.x, mRealGridPosition.y - mCollisionBox.y + mCollisionBoxOffset.y + (mVelocity.y * deltaTime));

		mGrounded = false;

		CollisionPositionalData returnData = CheckYCollision(leftPos, rightPos, interactionLayer, objectLayer);

		return (CollisionPositionalData(returnData.collisionOccured, leftPos, rightPos, returnData.collisionWithInteractionLayer, returnData.collisionWithObjectLayer));
	}

	return CollisionPositionalData();
}

// ----------------------------------------------------- //

CollisionPositionalData PlayableCharacter::CheckXCollision(const Vector2D positionToCheck1, const Vector2D positionToCheck2, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Check to see if we have hit the terrain on the X movement, and the objects in the level, and the screen constraints
	if (HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck1)                || HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck2) ||
		positionToCheck1.x < 0.0f ||
		positionToCheck2.x < 0.0f ||
		positionToCheck1.x > mLevelBounds.x ||
		positionToCheck2.x > mLevelBounds.x)
	{
		// Return that there was a collision
		return CollisionPositionalData(true, Vector2D(), Vector2D(), true, false);
	}

	if (HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck1).StopXMovement || HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck2).StopXMovement)
	{
		return CollisionPositionalData(true, Vector2D(), Vector2D(), false, true);
	}

	// Return that there was not a collision
	return CollisionPositionalData();
}

// ----------------------------------------------------- //

CollisionPositionalData PlayableCharacter::CheckYCollision(const Vector2D positionToCheck1, const Vector2D positionToCheck2, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// Check terrain collision
	if (HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck1) || HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck2) ||
		positionToCheck1.y < 0.0f ||
		positionToCheck2.y < 0.0f ||
		positionToCheck1.y + mCollisionBox.y > mLevelBounds.y + 2 ||
		positionToCheck2.y + mCollisionBox.y > mLevelBounds.y + 2)
	{
		if (mVelocity.y >= 0.0f)
		{
			mGrounded = true;
		}

		// Return that there was a collision
		return CollisionPositionalData(true, Vector2D(), Vector2D(), true, false);
	}

	MovementPrevention returnData1 = HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck1), returnData2 = HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck2);

	if (returnData1.StopYMovement || returnData2.StopYMovement)
	{
		if (returnData1.givesJump || returnData2.givesJump)
		{
			mJumpTimerLeway = 0.1f;
			mGrounded       = true;
		}

		// Return that there was a collision
		return CollisionPositionalData(true, Vector2D(), Vector2D(), false, true);
	}

	// Return that there was not a collision
	return CollisionPositionalData();
}

// ----------------------------------------------------- //

void PlayableCharacter::CalculateScreenBoundsPosition(const Vector2D currentPos)
{
	// If the player is near a boundary then they need specific positioning - starting with the Y axis
	if (currentPos.y + (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) > mLevelBounds.y)
	{
		mScreenGridPosition.y = (BACKGROUND_SPRITE_RENDER_HEIGHT - 1.0) - (mLevelBounds.y - currentPos.y);
	}
	else if (currentPos.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) < 0.0f)
	{
		mScreenGridPosition.y = currentPos.y;
	}
	else
	{
		mScreenGridPosition.y = LEVEL_BOUNDING_AREA_HEIGHT / 2;
	}

	// Now for the X axis
	if (currentPos.x + (PLAYABLE_SCREEN_AREA_WIDTH / 2.0f) > mLevelBounds.x)
	{
		mScreenGridPosition.x = mLevelBounds.x - currentPos.x;
	}
	else if (currentPos.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2.0f) < 0.0f)
	{
		mScreenGridPosition.x = currentPos.x;
	}
	else
	{
		mScreenGridPosition.x = LEVEL_BOUNDING_AREA_WIDTH / 2.0f;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::CalculateNewPosition(const float deltaTime, CollisionPositionalData xCollision, CollisionPositionalData yCollision)
{
	Vector2D movementDistance = (mVelocity * deltaTime);                // Distance potentially moved

	// First calculate the new position in the world based on collisions
	Vector2D newRealGridPos   = mRealGridPosition   + movementDistance; // Real position in the world according to this movement

	// Check for bounds
	if (newRealGridPos.y <= 0.0f)
		newRealGridPos.y = 0.0f;
	else if (newRealGridPos.y >= mLevelBounds.y + 2)
		newRealGridPos.y = mLevelBounds.y + 2;

	if (newRealGridPos.x <= -1.0f)
		newRealGridPos.x = -1.0f;
	else if (newRealGridPos.x >= mLevelBounds.x + 1)
		newRealGridPos.x = mLevelBounds.x + 1;

	if(!xCollision.collisionOccured)
	{
		// No collision so move the player in that direction
		mRealGridPosition.x = newRealGridPos.x;
	}
	else
	{
		if (xCollision.collisionWithInteractionLayer)
		{
			// We have the guarentee that this collision will obey grid lines
			if (mVelocity.x > 0.0f)
			{
				mRealGridPosition.x = int(mRealGridPosition.x) + mCollisionBoxOffset.x;
			}
			else if(mVelocity.x < 0.0f)
			{ 
				mRealGridPosition.x = int(mRealGridPosition.x + movementDistance.x) + 1.0;
			}
		}

		if(!yCollision.collisionOccured)
			mVelocity.x = 0.0f;
	}

	if(!yCollision.collisionOccured)
	{
		// No collision so move freely to the new position
		mRealGridPosition.y = newRealGridPos.y;
	}
	else
	{
		// Check what the collision was with
		if (yCollision.collisionWithInteractionLayer)
		{
			if (mVelocity.y >= 0.0f)
			{
				mRealGridPosition.y = int(mRealGridPosition.y) + 0.9;
			}

			mVelocity.y = 0.0f;
		}
		else if (yCollision.collisionWithObjectLayer)
		{
			mVelocity.y = 0.0f;
		}
	}

	std::cout << "Real X: " << mRealGridPosition.x << "\tReal Y :" << mRealGridPosition.y << std::endl;

	// ----------------------------------------------------------------------------------------------------------------------------------- //

	// Now calculate the new screen position and render reference point
	CalculateNewScreenPosAndRenderPos(movementDistance, xCollision, yCollision);
}

// ----------------------------------------------------------------------------------------------------------------------------------- //

void PlayableCharacter::CalculateNewScreenPosAndRenderPos(Vector2D movementDistance, const CollisionPositionalData xCollision, const CollisionPositionalData yCollision)
{		
	// Calculate the new potential screen position
	Vector2D newScreenGridPos = mScreenGridPosition + movementDistance;

	// First cap the new positions to their relative bounds - starting with the y axis
	if (newScreenGridPos.y <= 0.0f)
		newScreenGridPos.y = 0.0f;
	else if (newScreenGridPos.y >= PLAYABLE_SCREEN_AREA_HEIGHT + 4.0f)
		newScreenGridPos.y = PLAYABLE_SCREEN_AREA_HEIGHT + 4.0f;

	// Now for the x axis
	if (newScreenGridPos.x <= 0.0f)
		newScreenGridPos.x = 0.0f;
	else if (newScreenGridPos.x >= (PLAYABLE_SCREEN_AREA_WIDTH - 1.0f))
		newScreenGridPos.x = (PLAYABLE_SCREEN_AREA_HEIGHT - 1.0f);

	// ------------------------------------------------------------------------------------------- //

	// Check for a collision in the X axis
	if (xCollision.collisionOccured)
	{
		mScreenGridPosition.x = ConvertRealPositionIntoScreenPos(mRealGridPosition).x;
	}
	else // No collision occured
	{
		// If the screen position is on the right hand side of the screen then scroll the world, don't move the player
		if ((newScreenGridPos.x > (PLAYABLE_SCREEN_AREA_WIDTH - (PLAYABLE_SCREEN_AREA_WIDTH / 2.0)) + 1 && movementDistance.x > 0.0f) || 
			(newScreenGridPos.x < ((PLAYABLE_SCREEN_AREA_WIDTH / 2.0f) - 1.0f)                          && movementDistance.x < 0.0f))
		{
			// If moving in the correct direction then scroll the screen
			mRenderRefencePoint.x += movementDistance.x;
		}
		else // If we are within this leway area then don't scroll the screen
		{
			mScreenGridPosition.x = newScreenGridPos.x;
		}

		if (mRealGridPosition.x + (PLAYABLE_SCREEN_AREA_WIDTH / 2) - 1 > mLevelBounds.x ||
			mRealGridPosition.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2) + 1 < 0.0f)
		{
			mScreenGridPosition.x = newScreenGridPos.x;
		}
	}

	// ------------------------------------------------------------------------------------------- //

	// Check for Y collision
	if (yCollision.collisionOccured)
	{
		mScreenGridPosition.y = ConvertRealPositionIntoScreenPos(mRealGridPosition).y;
	}
	else
	{
		// If we want to scroll the screen then scroll - Y axis
		if ((newScreenGridPos.y > PLAYABLE_SCREEN_AREA_HEIGHT - (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) + 2 && movementDistance.y > 0.0f) || 
			(newScreenGridPos.y < (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f)                                   && movementDistance.y < 0.0f))
		{
			mRenderRefencePoint.y += movementDistance.y;
		}
		else
		{
			// We are within the two space leway in the centre of the screen then allow movement
			mScreenGridPosition.y = newScreenGridPos.y;
		}

		// Now check to see if we are at the top or the bottom portions of the level
		if ((mRealGridPosition.y + (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) > mLevelBounds.y) ||
			mRealGridPosition.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) < 0.0f)
		{
			mScreenGridPosition.y = newScreenGridPos.y;
		}

		mGrounded = false;
	}

	// ------------------------------------------------------------------------------------------- //

	// Cap the render reference point to the correct bounds

	// X checks
	if (   mRenderRefencePoint.x < 0.0f
		&& mRenderRefencePoint.x > mLevelBounds.x - BACKGROUND_SPRITE_RENDER_WIDTH)
	{
		mRenderRefencePoint.x = (mLevelBounds.x / 2.0f) - (PLAYABLE_SCREEN_AREA_WIDTH / 2.0f);
	}
	else if (mRenderRefencePoint.x < 0.0f)
	{
		mRenderRefencePoint.x = 0.0f;
	}
	else if (mRenderRefencePoint.x > mLevelBounds.x - BACKGROUND_SPRITE_RENDER_WIDTH)
	{
		mRenderRefencePoint.x = mLevelBounds.x - BACKGROUND_SPRITE_RENDER_WIDTH;
	}

	// Y checks
	if (mRenderRefencePoint.y < 0.0f
		&& mRenderRefencePoint.y > mLevelBounds.y - BACKGROUND_SPRITE_RENDER_HEIGHT + 1)
	{
		mRenderRefencePoint.y = (mLevelBounds.y / 2.0f) - (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f);
	}
	else if (mRenderRefencePoint.y < 0.0f)
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

Vector2D PlayableCharacter::ConvertRealPositionIntoScreenPos(Vector2D realPos)
{
	// This function returns the screen position of a position relative to the screen position in the world
	return (realPos - mRenderRefencePoint);
}

// ----------------------------------------------------- //

void PlayableCharacter::HandleMovementInput(SDL_Event e)
{
	double speed      = 15.0f;
	double multiplier = 1.0f;

	// If the player is currently running then set the speed to be correct
	if (mCurrentMovements & MovementBitField::RUNNING)
		multiplier = 1.1f;

	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_w:
			if (!(mCurrentMovements & MovementBitField::JUMPING))
			{
				HandleChangeInAnimations(MovementBitField::JUMPING, true);
				mCurrentMovements |= MovementBitField::JUMPING;
			}

			if (!(mCurrentMovements & MovementBitField::HOLDING_JUMP))
			{
				// Otherwise check if you are jumping and if so state that you are holding down jump
				if (mCurrentMovements & MovementBitField::JUMPING)
					mCurrentMovements |= MovementBitField::HOLDING_JUMP;
			}
		break;

		case SDLK_RSHIFT:
			// Store that the player is now running
			if (!(mCurrentMovements & MovementBitField::RUNNING))
			{
				HandleChangeInAnimations(MovementBitField::RUNNING, true);
				mCurrentMovements |= MovementBitField::RUNNING;
			}
		break;

		case SDLK_d:
			if (!(mCurrentMovements & MovementBitField::MOVING_RIGHT))
			{
				mWasFacingRight = false;
				HandleChangeInAnimations(MovementBitField::MOVING_RIGHT, true);
				mCurrentMovements |= MovementBitField::MOVING_RIGHT;
				mAcceleration.x = multiplier * speed;
			}
		break;

		case SDLK_a:

			if (!(mCurrentMovements & MovementBitField::MOVING_LEFT))
			{
				HandleChangeInAnimations(MovementBitField::MOVING_LEFT, true);
				mCurrentMovements |= MovementBitField::MOVING_LEFT;
				mAcceleration.x = -multiplier * speed;
			}
		break;

		case SDLK_s:
			if (!(mCurrentMovements & MovementBitField::CROUCHING))
			{
				HandleChangeInAnimations(MovementBitField::CROUCHING, true);
				mCurrentMovements |= MovementBitField::CROUCHING;
			}

			//mAcceleration.y = multiplier * speed;
		break;
		}
	break;

	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_RSHIFT:

			if (mCurrentMovements & MovementBitField::RUNNING)
			{
				// Set the player to not be running
				HandleChangeInAnimations(MovementBitField::RUNNING, false);
				mCurrentMovements &= ~(MovementBitField::RUNNING);

				mMaxHorizontalSpeed = kBaseMaxHorizontalSpeed;
			}
		break;

		case SDLK_d:
			if (mCurrentMovements & MovementBitField::MOVING_RIGHT)
			{
				mWasFacingRight = true;

				HandleChangeInAnimations(MovementBitField::MOVING_RIGHT, false);
				mCurrentMovements &= ~(MovementBitField::MOVING_RIGHT);

				mAcceleration.x = 0.0f;
			}
		break;

		case SDLK_a:
			if (mCurrentMovements & MovementBitField::MOVING_LEFT)
			{
				mWasFacingRight = false;

				HandleChangeInAnimations(MovementBitField::MOVING_LEFT, false);
				mCurrentMovements &= ~(MovementBitField::MOVING_LEFT);

				mAcceleration.x = 0.0f;
			}
		break;

		case SDLK_s:
			if (mCurrentMovements & MovementBitField::CROUCHING)
			{
				HandleChangeInAnimations(MovementBitField::CROUCHING, false);
				mCurrentMovements &= ~(MovementBitField::CROUCHING);
			}
		break;

		case SDLK_w:

			// Not holding jump anymore
			if (mCurrentMovements & MovementBitField::JUMPING)
			{
				mCurrentMovements &= ~(MovementBitField::JUMPING);
				mCurrentMovements &= ~(MovementBitField::HOLDING_JUMP);

				// If you release jump whilst jumping you cannot continue the extra boost after the release
				mJumpHeldCurrentBoost = 0.0f;

				mAcceleration.y = 0.0f;
			}
		break;
		}
	break;
	}

	// If grounded then set that the player is jumping
	if (mCurrentMovements & MovementBitField::JUMPING && (mGrounded || mJumpTimerLeway > 0.0f))
	{
		mJumpHeldCurrentBoost = kJumpHeldInitialBoost * std::max(float(abs(mVelocity.x) / kMaxHorizontalSpeedOverall), 0.1f);

		mGrounded = false;

		// Give the minimum jump height of boost upwards
		mVelocity.y = mJumpInitialBoost;

		mJumpTimerLeway = 0.0f;

		Notify(SUBJECT_NOTIFICATION_TYPES::PLAYER_JUMPED, "");
	}

	if (mGrounded 
		&& !(mCurrentMovements & MovementBitField::JUMPING) 
		&& (mAnimationCurrentState == MovementBitField::JUMPING))
	{
		HandleChangeInAnimations(MovementBitField::JUMPING, false);
	}

	if (mCurrentMovements == 0 
		&& mAnimationCurrentState != MovementBitField::JUMPING)
	{
		HandleChangeInAnimations(MovementBitField::NONE, true);
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::HandleChangeInAnimations(MovementBitField newMovement, bool goingInto)
{
	// Now check to see if we should change animation
	switch (mPowerUpState)
	{
	default:
	case POWER_UP_TYPE::NONE:
		UpdateAnimationsSmallMario(newMovement, goingInto);
	break;

	case POWER_UP_TYPE::MUSHROOM:
		UpdateAnimationsLargeMario(newMovement, goingInto);
	break;

	case POWER_UP_TYPE::FIRE_FLOWER:
		UpdateAnimationsFireMario(newMovement, goingInto);
	break;

	case POWER_UP_TYPE::TANOOKI_SUIT:
		UpdateAnimationsTanookiMario(newMovement, goingInto);
	break;

	case POWER_UP_TYPE::FROG_SUIT:
		UpdateAnimationsFrogMario(newMovement, goingInto);
	break;

	case POWER_UP_TYPE::HAMMER_SUIT:
		UpdateAnimationsHammerMario(newMovement, goingInto);
	break;

	case POWER_UP_TYPE::STAR:
		UpdateAnimationsStarMario(newMovement, goingInto);
	break;

	case POWER_UP_TYPE::SUPER_LEAF:
		UpdateAnimationsLeafMario(newMovement, goingInto);
	break;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::CalculateInitialRenderReferencePoint()
{
	// Starting with the Y axis
	if (mRealGridPosition.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) <= 0.0f
		&& mRealGridPosition.y + ((PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) + 3) >= mLevelBounds.y)
	{
		mRenderRefencePoint.y = mLevelBounds.y / 2.0f;
	}
	else if (mRealGridPosition.y - (PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) <= 0.0f)
	{
		// Set the correct reference point
		mRenderRefencePoint.y = 0.0f;
	}
	else if (mRealGridPosition.y + ((PLAYABLE_SCREEN_AREA_HEIGHT / 2.0f) + 3) >= mLevelBounds.y)
	{
		// Set the correct reference point
		mRenderRefencePoint.y = mLevelBounds.y - (BACKGROUND_SPRITE_RENDER_HEIGHT - 1);
	}

	// Now for the X-axis
	if (   mRealGridPosition.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2.0f) < 0.0f 
		&& mRealGridPosition.x + (PLAYABLE_SCREEN_AREA_WIDTH / 2.0f) >= mLevelBounds.x)
	{
		mRenderRefencePoint.x = mLevelBounds.x / 2.0f;
	}
	else if (mRealGridPosition.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2.0f) < 0.0f)
	{
		mRenderRefencePoint.x = 0.0f;
	}
	else if (mRealGridPosition.x + (PLAYABLE_SCREEN_AREA_WIDTH / 2.0f) >= mLevelBounds.x)
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

	if (mVelocity.y > kMaxYVelocity)
		mVelocity.y = kMaxYVelocity;
	else if (mVelocity.y < -kMaxYVelocity)
		mVelocity.y = -kMaxYVelocity;
}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimations(const float deltaTime)
{
	mTimeTillNextFrame -= deltaTime;

	// Check to see if we should move the frame on
	if (mTimeTillNextFrame <= 0.0f)
	{
		mCurrentFrame++;

		if (mCurrentFrame > mEndFrame)
		{
			mCurrentFrame = mStartFrame;
		}

		mTimeTillNextFrame = mTimePerFrame;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsSmallMario(MovementBitField newMovement, bool goingInto)
{
	// Switch through the states to see what we are going into
	switch (newMovement)
	{
	case MovementBitField::HOLDING_JUMP:
	default:
		
	break;

	case MovementBitField::JUMPING:
		if (goingInto)
		{
			// if the player is running then go into the sprint jump animation
			if (mCurrentMovements & MovementBitField::RUNNING)
			{
				mStartFrame = 5;
				mEndFrame   = 5;
				mCurrentFrame = mStartFrame;

				mAnimationCurrentState = MovementBitField::JUMPING;
			}
			else
			{
				// As jumping overrides everything else animation wise just go into the jumping state
				mStartFrame = 2;
				mEndFrame   = 2;
				mCurrentFrame = mStartFrame;

				mAnimationCurrentState = MovementBitField::JUMPING;
			}
		}
		else // Coming out of the jumping state, so check to see what state we should go into
		{
			if (mGrounded)
			{
				if (mCurrentMovements & MovementBitField::RUNNING)
				{
					if(mAnimationCurrentState == MovementBitField::JUMPING)
						UpdateAnimationsSmallMario(MovementBitField::RUNNING, true);
				}
				else if (mCurrentMovements & MovementBitField::MOVING_LEFT)
				{
					mAnimationCurrentState = MovementBitField::NONE;
					UpdateAnimationsSmallMario(MovementBitField::MOVING_LEFT, true);
				}
				else if (mCurrentMovements & MovementBitField::MOVING_RIGHT)
				{
					mAnimationCurrentState = MovementBitField::NONE;
					UpdateAnimationsSmallMario(MovementBitField::MOVING_RIGHT, true);
				}
				else
					UpdateAnimationsSmallMario(MovementBitField::NONE, true);
			}
		}
	break;

	case MovementBitField::MOVING_RIGHT:
		if (goingInto)
		{
			// Go into this state if not jumping 
			if (!(mCurrentMovements & MovementBitField::JUMPING) && mAnimationCurrentState != MovementBitField::JUMPING)
			{
				mAnimationCurrentState = MovementBitField::MOVING_RIGHT;
				mStartFrame = 0;
				mEndFrame = 1;
				mCurrentFrame = mStartFrame;
			}
		}
		else
		{
			if (mCurrentMovements & MovementBitField::JUMPING)
			{
				UpdateAnimationsSmallMario(MovementBitField::JUMPING, true);
			}
			else
			{
				UpdateAnimationsSmallMario(MovementBitField::NONE, true);
			}
		}
	break;

	case MovementBitField::MOVING_LEFT:
		if (goingInto)
		{
			// Go into this state if not jumping 
			if (!(mCurrentMovements & MovementBitField::JUMPING) && mAnimationCurrentState != MovementBitField::JUMPING)
			{
				mAnimationCurrentState = MovementBitField::MOVING_LEFT;

				mStartFrame   = 0;
				mEndFrame     = 1;
				mCurrentFrame = mStartFrame;
			}
		}
		else
		{
			if (mCurrentMovements & MovementBitField::JUMPING)
			{
				UpdateAnimationsSmallMario(MovementBitField::JUMPING, true);
			}
			else
			{
				UpdateAnimationsSmallMario(MovementBitField::NONE, true);
			}
		}
	break;

	case MovementBitField::ENTERING_PIPE_VERTICALLY:
	case MovementBitField::CROUCHING:
	break;

	case MovementBitField::NONE:
		// If doing absolutly nothing then stop movements
		if (mCurrentMovements == 0)
		{
			mAnimationCurrentState = MovementBitField::NONE;

			mEndFrame     = 0;
			mStartFrame   = 0;
			mCurrentFrame = mStartFrame;
		}
	break;

	case MovementBitField::RUNNING:
		if (goingInto)
		{
			if (!(mCurrentMovements & MovementBitField::JUMPING) && (mCurrentMovements & MovementBitField::MOVING_LEFT || mCurrentMovements & MovementBitField::MOVING_RIGHT))
			{
				mAnimationCurrentState = MovementBitField::RUNNING;

				mStartFrame = 3;
				mEndFrame   = 4;
				mCurrentFrame = mStartFrame;
			}
		}
		else
		{
			// If still moving left or right then fall back into that animation state
			if (mCurrentMovements & MovementBitField::MOVING_LEFT)
			{
				UpdateAnimationsSmallMario(MovementBitField::MOVING_LEFT, true);
			}
			else if (mCurrentMovements & MovementBitField::MOVING_RIGHT)
			{
				UpdateAnimationsSmallMario(MovementBitField::MOVING_RIGHT, true);
			}
		}
	break;

	case MovementBitField::SWIMMING:
	break;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsLargeMario(MovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsFrogMario(MovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsHammerMario(MovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsFireMario(MovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsTanookiMario(MovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsLeafMario(MovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsStarMario(MovementBitField newMovement, bool goingInto)
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
