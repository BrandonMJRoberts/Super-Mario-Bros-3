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
, mMaxHorizontalSpeed(8.5f)
, kForcedMovementSpeed(1.8f)

, kFrictionMultiplier(9.0f)

, kJumpHeldAccelerationDepreciationRate(16.0f)
, mJumpInitialBoost(-14.0f)
, kJumpHeldInitialBoost(-17.0f)
, mJumpHeldCurrentBoost(kJumpHeldInitialBoost)

, mCurrentMovements(PlayerMovementBitField::NONE)

, mTimePerFrame(timePerFrame)
, mTimeTillNextFrame(timePerFrame)
, mAnimationCurrentState(PlayerMovementBitField::NONE)
, mForcedMovementDirection(MOVEMENT_DIRECTION::DOWN)

, kMaxYVelocity(60.0f)

, mPowerUpState(POWER_UP_TYPE::NONE)
, mRenderer(renderer)
, mWasFacingRight(true)
, mGrounded(false)
, mHasControl(true)

, mJumpTimerLeway(0.0f)
, mForcedMovementDistanceTravelled(0.0f)

, mExitingPipe(false)
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
		if (mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT)
		{
			// Render it facing Right
			mSpriteSheet->Render(portionOfSpriteSheet, destRect, SDL_FLIP_HORIZONTAL);
		}
		else if (mCurrentMovements & PlayerMovementBitField::MOVING_LEFT) // If going left
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
	{
		ForcedMovementUpdate(deltaTime);

		return;
	}

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

void PlayableCharacter::ForcedMovementUpdate(const float deltaTime)
{
	if (mForceMovementInDirectionSet)
	{
		double movementDistance = double(kForcedMovementSpeed) * double(deltaTime);

		mForcedMovementDistanceTravelled += float(movementDistance);

		switch (mForcedMovementDirection)
		{
		case MOVEMENT_DIRECTION::DOWN:
			mScreenGridPosition.y += movementDistance;

			if (mForcedMovementDistanceTravelled > (mCollisionBox.y * 1.2f))
			{
				mForceMovementInDirectionSet     = false;
				mForcedMovementDistanceTravelled = 0.0f;

				if (mExitingPipe)
				{
					mHasControl = true;
					mExitingPipe = false;
				}
			}
		break;

		case MOVEMENT_DIRECTION::UP:
			mScreenGridPosition.y -= movementDistance;

			if (mForcedMovementDistanceTravelled > (mCollisionBox.y * 1.2f))
			{
				mForceMovementInDirectionSet     = false;
				mForcedMovementDistanceTravelled = 0.0f;

				if (mExitingPipe)
				{
					mExitingPipe = false;
					mHasControl  = true;
				}
			}
		break;

		case MOVEMENT_DIRECTION::RIGHT:
			mScreenGridPosition.x += movementDistance;

			if (mForcedMovementDistanceTravelled > (mCollisionBox.x * 1.2f))
			{
				mForceMovementInDirectionSet     = false;
				mForcedMovementDistanceTravelled = 0.0f;

				if (mExitingPipe)
				{
					mHasControl = true;
					mExitingPipe = false;
				}
			}
		break;

		case MOVEMENT_DIRECTION::LEFT:
			mScreenGridPosition.x -= movementDistance;

			if (mForcedMovementDistanceTravelled > (mCollisionBox.x * 1.2f))
			{
				mForceMovementInDirectionSet     = false;
				mForcedMovementDistanceTravelled = 0.0f;

				if (mExitingPipe)
				{
					mHasControl = true;
					mExitingPipe = false;
				}
			}
		break;
		}
	}
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
		headPos = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x + (mVelocity.x * deltaTime), mRealGridPosition.y - mCollisionBox.y + mCollisionBoxOffset.y);
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
			mCurrentMovements &= ~(PlayerMovementBitField::HOLDING_JUMP);

			if (!(mCurrentMovements & PlayerMovementBitField::JUMPING))
			{
				mCurrentMovements &= ~(PlayerMovementBitField::JUMPING);
				mCurrentMovements &= ~(PlayerMovementBitField::HOLDING_JUMP);
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
	// Distance potentially moved
	Vector2D movementDistance = (mVelocity * deltaTime);                

	// First calculate the new position in the world based on collisions
	Vector2D newRealGridPos   = mRealGridPosition   + movementDistance; // Real position in the world according to this movement

	// Check for bounds
	if (newRealGridPos.y <= 0.0f)
		newRealGridPos.y = 0.0f;
	else if (newRealGridPos.y >= mLevelBounds.y + 2)
		newRealGridPos.y = mLevelBounds.y + 2;

	if (newRealGridPos.x <= 0.0f)
		newRealGridPos.x = 0.0f;
	else if (newRealGridPos.x >= mLevelBounds.x - 1)
		newRealGridPos.x = mLevelBounds.x - 1;

	// Now for the Y
	if (!yCollision.collisionOccured)
	{
		// No collision so move freely to the new position
		mRealGridPosition.y = newRealGridPos.y;
	}
	else
	{
		// Check what the collision was with
		if (yCollision.collisionWithInteractionLayer && mVelocity.y >= 0.0f)
		{
			mRealGridPosition.y = int(mRealGridPosition.y) + 0.9;
		}

		mVelocity.y = 0.0f;
	}

	if(!xCollision.collisionOccured)
	{
		// No collision so move the player in that direction
		mRealGridPosition.x = newRealGridPos.x;
	}
	else
	{
		mVelocity.x = 0.0f;

		if (xCollision.collisionWithInteractionLayer)
		{
			// We have the guarentee that this collision will obey grid lines
			if (movementDistance.x > 0.0f)
			{
				mRealGridPosition.x = int(mRealGridPosition.x) + mCollisionBoxOffset.x;
			}
			else if(movementDistance.x < 0.0f)
			{ 
				mRealGridPosition.x = int(mRealGridPosition.x) + mCollisionBoxOffset.x + mCollisionBox.x + 0.01;
			}
		}
	}

	//std::cout << "Real X: " << mRealGridPosition.x << "\tReal Y :" << mRealGridPosition.y << std::endl;

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

	if (mCurrentMovements & PlayerMovementBitField::RUNNING && abs(mVelocity.x) > kBaseMaxHorizontalSpeed)
	{
		multiplier = 0.7f;
	}

	if (mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT)
	{
		mAcceleration.x = speed * multiplier;
	}
	else if (mCurrentMovements & PlayerMovementBitField::MOVING_LEFT)
	{
		mAcceleration.x = -speed * multiplier;
	}

	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_w:
			if (!(mCurrentMovements & PlayerMovementBitField::JUMPING))
			{
				HandleChangeInAnimations(PlayerMovementBitField::JUMPING, true);

				// Set the extra boost to be relative to the x speed the player is going
				mJumpHeldCurrentBoost = kJumpHeldInitialBoost * std::max(float(abs(mVelocity.x) / mMaxHorizontalSpeed), 0.1f);

				mCurrentMovements |= PlayerMovementBitField::JUMPING;
			}

			if (!(mCurrentMovements & PlayerMovementBitField::HOLDING_JUMP))
			{
				// Otherwise check if you are jumping and if so state that you are holding down jump
				if (mCurrentMovements & PlayerMovementBitField::JUMPING)
					mCurrentMovements |= PlayerMovementBitField::HOLDING_JUMP;
			}
		break;

		case SDLK_d:
			if (!(mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT))
			{
				mWasFacingRight = false;
				HandleChangeInAnimations(PlayerMovementBitField::MOVING_RIGHT, true);
				mCurrentMovements |= PlayerMovementBitField::MOVING_RIGHT;
				mAcceleration.x = multiplier * speed;
			}
			
		break;

		case SDLK_a:

			if (!(mCurrentMovements & PlayerMovementBitField::MOVING_LEFT))
			{
				HandleChangeInAnimations(PlayerMovementBitField::MOVING_LEFT, true);
				mCurrentMovements |= PlayerMovementBitField::MOVING_LEFT;
				mAcceleration.x = -multiplier * speed;
			}
		break;

		case SDLK_s:
			if (!(mCurrentMovements & PlayerMovementBitField::CROUCHING))
			{
				HandleChangeInAnimations(PlayerMovementBitField::CROUCHING, true);
				mCurrentMovements |= PlayerMovementBitField::CROUCHING;
			}
		break;

		case SDLK_RSHIFT:
			// Store that the player is now running
			if (!(mCurrentMovements & PlayerMovementBitField::RUNNING))
			{
				HandleChangeInAnimations(PlayerMovementBitField::RUNNING, true);
				mCurrentMovements |= PlayerMovementBitField::RUNNING;
			}
		break;
		}
	break;

	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_RSHIFT:

			if (mCurrentMovements & PlayerMovementBitField::RUNNING)
			{
				// Set the player to not be running
				HandleChangeInAnimations(PlayerMovementBitField::RUNNING, false);
				mCurrentMovements &= ~(PlayerMovementBitField::RUNNING);
			}
		break;

		case SDLK_d:
			if (mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT)
			{
				mWasFacingRight = true;

				HandleChangeInAnimations(PlayerMovementBitField::MOVING_RIGHT, false);
				mCurrentMovements &= ~(PlayerMovementBitField::MOVING_RIGHT);

				mAcceleration.x = 0.0f;
			}
		break;

		case SDLK_a:
			if (mCurrentMovements & PlayerMovementBitField::MOVING_LEFT)
			{
				mWasFacingRight = false;

				HandleChangeInAnimations(PlayerMovementBitField::MOVING_LEFT, false);
				mCurrentMovements &= ~(PlayerMovementBitField::MOVING_LEFT);

				mAcceleration.x = 0.0f;
			}
		break;

		case SDLK_s:
			if (mCurrentMovements & PlayerMovementBitField::CROUCHING)
			{
				HandleChangeInAnimations(PlayerMovementBitField::CROUCHING, false);
				mCurrentMovements &= ~(PlayerMovementBitField::CROUCHING);
			}
		break;

		case SDLK_w:

			// Not holding jump anymore
			if (mCurrentMovements & PlayerMovementBitField::JUMPING)
			{
				mCurrentMovements &= ~(PlayerMovementBitField::JUMPING);
				mCurrentMovements &= ~(PlayerMovementBitField::HOLDING_JUMP);

				// If you release jump whilst jumping you cannot continue the extra boost after the release
				mJumpHeldCurrentBoost = 0.0f;

				mAcceleration.y = 0.0f;
			}
		break;
		}
	break;
	}

	// If grounded then set that the player is jumping
	if (mCurrentMovements & PlayerMovementBitField::JUMPING && (mGrounded || mJumpTimerLeway > 0.0f))
	{
		mGrounded = false;

		// Give the minimum jump height of boost upwards
		mVelocity.y = mJumpInitialBoost;

		mJumpTimerLeway = 0.0f;

		Notify(SUBJECT_NOTIFICATION_TYPES::PLAYER_JUMPED, "");
	}

	if (mGrounded 
		&& !(mCurrentMovements & PlayerMovementBitField::JUMPING)
		&& (mAnimationCurrentState == PlayerMovementBitField::JUMPING))
	{
		HandleChangeInAnimations(PlayerMovementBitField::JUMPING, false);
	}

	if (mCurrentMovements == 0 
		&& mAnimationCurrentState != PlayerMovementBitField::JUMPING)
	{
		HandleChangeInAnimations(PlayerMovementBitField::NONE, true);
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::HandleChangeInAnimations(PlayerMovementBitField newMovement, bool goingInto)
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
	else
	{
		mRenderRefencePoint.x = mRealGridPosition.x - (PLAYABLE_SCREEN_AREA_WIDTH / 2.0f) + 0.5f;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::SpawnIntoNewArea(const Vector2D newPos, const Vector2D newLevelBounds, bool pipeTransition)
{
	mRealGridPosition = newPos; 

	mLevelBounds      = newLevelBounds;

	CalculateScreenBoundsPosition(newPos);

	// Now calculate the starting render reference point
	CalculateInitialRenderReferencePoint();

	if (pipeTransition)
	{
		mForceMovementInDirectionSet     = true;
		mForcedMovementDistanceTravelled = 0.0f;

		switch (mForcedMovementDirection)
		{
		default:
		case MOVEMENT_DIRECTION::DOWN:
			mScreenGridPosition.y -= (mCollisionBox.y * 1.2f);
		break;

		case MOVEMENT_DIRECTION::UP:
			mScreenGridPosition.y += (mCollisionBox.y * 1.2f);
		break;

		case MOVEMENT_DIRECTION::LEFT:
			mScreenGridPosition.x += (mCollisionBox.x * 1.2f);
		break;

		case MOVEMENT_DIRECTION::RIGHT:
			mScreenGridPosition.x -= (mCollisionBox.x * 1.2f);
		break;
		}

		SetEnteringPipe(mForcedMovementDirection);

		mExitingPipe = true;
	}
	else
	{
		mHasControl = true;
	}
	
	mCurrentMovements                = 0;

	mVelocity     = Vector2D();
	mAcceleration = Vector2D();
}

// ----------------------------------------------------- //

bool PlayableCharacter::HandleCollisionsWithInteractionLayer(InteractableLayer* interactionLayer, const Vector2D newPos)
{
	return (interactionLayer->GetIsCollision((unsigned int)newPos.y, (unsigned int)newPos.x));
}

// ----------------------------------------------------- //

MovementPrevention PlayableCharacter::HandleCollisionsWithInteractionObjectLayer(ObjectLayer* objectLayer, const Vector2D newPos)
{
	return objectLayer->CheckCollision(newPos, mVelocity, mRealGridPosition, mCurrentMovements, true);
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
	if (mCurrentMovements & PlayerMovementBitField::HOLDING_JUMP && mCurrentMovements & PlayerMovementBitField::RUNNING)
	{
		// Reduce the current extra being added onto the jump
		mJumpHeldCurrentBoost += (kJumpHeldAccelerationDepreciationRate * deltaTime);

		// If holding down jump then keep mario going up until a point
		mAcceleration.y = mJumpHeldCurrentBoost;

		// Bounds check
		if (mJumpHeldCurrentBoost >= 0.0f)
			mJumpHeldCurrentBoost = 0.0f;
	}

	// Apply the acceleration of the player 
	mVelocity += Vector2D(mAcceleration.x * deltaTime, (mAcceleration.y + GRAVITY) * deltaTime);

	// If walking
	if (mCurrentMovements & PlayerMovementBitField::RUNNING)
	{
		if (mVelocity.x > mMaxHorizontalSpeed)
			mVelocity.x = mMaxHorizontalSpeed;
		else if (mVelocity.x < -mMaxHorizontalSpeed)
			mVelocity.x = -mMaxHorizontalSpeed;
	}
	else
	{
		// Cap the velocity to the max speed if it exceeds it
		if (mVelocity.x > kBaseMaxHorizontalSpeed)
			mVelocity.x = kBaseMaxHorizontalSpeed;
		else if (mVelocity.x < -kBaseMaxHorizontalSpeed)
			mVelocity.x = -kBaseMaxHorizontalSpeed;
	}

	if (mVelocity.y > kMaxYVelocity)
		mVelocity.y = kMaxYVelocity;
	else if (mVelocity.y < -kMaxYVelocity)
		mVelocity.y = -kMaxYVelocity;

	std::cout << "X: " << mVelocity.x << "\t Y :" << mVelocity.y << std::endl;
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

void PlayableCharacter::UpdateAnimationsSmallMario(PlayerMovementBitField newMovement, bool goingInto)
{
	// Switch through the states to see what we are going into
	switch (newMovement)
	{
	case PlayerMovementBitField::HOLDING_JUMP:
	default:
		
	break;

	case PlayerMovementBitField::JUMPING:
		if (goingInto)
		{
			// if the player is running then go into the sprint jump animation
			if (mCurrentMovements & PlayerMovementBitField::RUNNING)
			{
				mStartFrame = 5;
				mEndFrame   = 5;
				mCurrentFrame = mStartFrame;

				mAnimationCurrentState = PlayerMovementBitField::JUMPING;
			}
			else
			{
				// As jumping overrides everything else animation wise just go into the jumping state
				mStartFrame = 2;
				mEndFrame   = 2;
				mCurrentFrame = mStartFrame;

				mAnimationCurrentState = PlayerMovementBitField::JUMPING;
			}
		}
		else // Coming out of the jumping state, so check to see what state we should go into
		{
			if (mGrounded)
			{
				if (mCurrentMovements & PlayerMovementBitField::RUNNING)
				{
					if(mAnimationCurrentState == PlayerMovementBitField::JUMPING)
						UpdateAnimationsSmallMario(PlayerMovementBitField::RUNNING, true);
				}
				else if (mCurrentMovements & PlayerMovementBitField::MOVING_LEFT)
				{
					mAnimationCurrentState = PlayerMovementBitField::NONE;
					UpdateAnimationsSmallMario(PlayerMovementBitField::MOVING_LEFT, true);
				}
				else if (mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT)
				{
					mAnimationCurrentState = PlayerMovementBitField::NONE;
					UpdateAnimationsSmallMario(PlayerMovementBitField::MOVING_RIGHT, true);
				}
				else
					UpdateAnimationsSmallMario(PlayerMovementBitField::NONE, true);
			}
		}
	break;

	case PlayerMovementBitField::MOVING_RIGHT:
		if (goingInto)
		{
			// Go into this state if not jumping 
			if (!(mCurrentMovements & PlayerMovementBitField::JUMPING) && mAnimationCurrentState != PlayerMovementBitField::JUMPING)
			{
				mAnimationCurrentState = PlayerMovementBitField::MOVING_RIGHT;
				mStartFrame = 0;
				mEndFrame = 1;
				mCurrentFrame = mStartFrame;
			}
		}
		else
		{
			if (mCurrentMovements & PlayerMovementBitField::JUMPING)
			{
				UpdateAnimationsSmallMario(PlayerMovementBitField::JUMPING, true);
			}
			else
			{
				UpdateAnimationsSmallMario(PlayerMovementBitField::NONE, true);
			}
		}
	break;

	case PlayerMovementBitField::MOVING_LEFT:
		if (goingInto)
		{
			// Go into this state if not jumping 
			if (!(mCurrentMovements & PlayerMovementBitField::JUMPING) && mAnimationCurrentState != PlayerMovementBitField::JUMPING)
			{
				mAnimationCurrentState = PlayerMovementBitField::MOVING_LEFT;

				mStartFrame   = 0;
				mEndFrame     = 1;
				mCurrentFrame = mStartFrame;
			}
		}
		else
		{
			if (mCurrentMovements & PlayerMovementBitField::JUMPING)
			{
				UpdateAnimationsSmallMario(PlayerMovementBitField::JUMPING, true);
			}
			else
			{
				UpdateAnimationsSmallMario(PlayerMovementBitField::NONE, true);
			}
		}
	break;

	case PlayerMovementBitField::ENTERING_PIPE_VERTICALLY:
	case PlayerMovementBitField::CROUCHING:
	break;

	case PlayerMovementBitField::NONE:
		// If doing absolutly nothing then stop movements
		if (mCurrentMovements == 0)
		{
			mAnimationCurrentState = PlayerMovementBitField::NONE;

			mEndFrame     = 0;
			mStartFrame   = 0;
			mCurrentFrame = mStartFrame;
		}
	break;

	case PlayerMovementBitField::RUNNING:
		if (goingInto)
		{
			if (!(mCurrentMovements & PlayerMovementBitField::JUMPING) && (mCurrentMovements & PlayerMovementBitField::MOVING_LEFT || mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT))
			{
				mAnimationCurrentState = PlayerMovementBitField::RUNNING;

				mStartFrame = 3;
				mEndFrame   = 4;
				mCurrentFrame = mStartFrame;
			}
		}
		else
		{
			// If still moving left or right then fall back into that animation state
			if (mCurrentMovements & PlayerMovementBitField::MOVING_LEFT)
			{
				UpdateAnimationsSmallMario(PlayerMovementBitField::MOVING_LEFT, true);
			}
			else if (mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT)
			{
				UpdateAnimationsSmallMario(PlayerMovementBitField::MOVING_RIGHT, true);
			}
		}
	break;

	case PlayerMovementBitField::SWIMMING:
	break;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsLargeMario(PlayerMovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsFrogMario(PlayerMovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsHammerMario(PlayerMovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsFireMario(PlayerMovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsTanookiMario(PlayerMovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsLeafMario(PlayerMovementBitField newMovement, bool goingInto)
{

}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsStarMario(PlayerMovementBitField newMovement, bool goingInto)
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

void PlayableCharacter::SetEnteringPipe(MOVEMENT_DIRECTION direction)
{
	mExitingPipe = false;

	// Take control from the player and make the character animate correctly, and move in the correct direction properly
	mHasControl = false;

	// Remove all movements
	mCurrentMovements = 0;

	// Set animations
	switch (direction)
	{
	case MOVEMENT_DIRECTION::DOWN:
	case MOVEMENT_DIRECTION::UP:
		HandleChangeInAnimations(PlayerMovementBitField::ENTERING_PIPE_VERTICALLY, true);
	break;

	case MOVEMENT_DIRECTION::RIGHT:
		HandleChangeInAnimations(PlayerMovementBitField::MOVING_RIGHT, true);
	break;

	case MOVEMENT_DIRECTION::LEFT:
		HandleChangeInAnimations(PlayerMovementBitField::MOVING_LEFT, true);
	break;
	}

	// Set that we are forcing movement
	mForceMovementInDirectionSet = true;

	mForcedMovementDirection = direction;
}

// ----------------------------------------------------- //