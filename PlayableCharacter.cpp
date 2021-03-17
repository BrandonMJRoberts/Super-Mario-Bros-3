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

, kMaxHorizontalSpeedWalking(5.0f)
, kMaxHorizontalSpeedRunning(8.5f)
, kForcedMovementSpeed(1.8f)

, kFrictionMultiplier(9.0f)

, kJumpHeldAccelerationDepreciationRate(15.0f)
, kJumpInitialBoost(-10.0f)
, kJumpHeldInitialBoost(-10.0f)
, mJumpHeldCurrentBoost(0.0f)

, mCurrentMovements(PlayerMovementBitField::NONE)

, kBaseTimePerFrame(timePerFrame)
, mTimeTillNextFrame(timePerFrame)
, mTimePerFrame(timePerFrame)

, mAnimationCurrentState(PlayerMovementBitField::NONE)
, mForcedMovementDirection(MOVEMENT_DIRECTION::DOWN)

, kMaxYVelocity(60.0f)

, mPowerUpState(POWER_UP_TYPE::NONE)
, mRenderer(renderer)
, mWasFacingRight(true)
, mGrounded(true)
, mHasControl(true)

, mDeathAnimationTime(4.5f)
, mForcedMovementDistanceTravelled(0.0f)

, mExitingPipe(false)
{
	// Load in the sprite sheet for this character
	LoadInCorrectSpriteSheet();

	// Now calculate where the starting screen space position the player should be
	CalculateScreenBoundsPosition(spawnPoint);

	// Now calculate the starting render reference point
	CalculateInitialRenderReferencePoint();

	// Set the collision box to default values
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

	mRenderer = nullptr;
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
			               int(mScreenGridPosition.y * RESOLUTION_OF_SPRITES) - int(float(RESOLUTION_OF_SPRITES) * mCollisionBox.y),
			               int(mSingleSpriteWidth), 
			               int(mSingleSpriteHeight) };


		// Render it facing right
		if (mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT)
		{
			mSpriteSheet->Render(portionOfSpriteSheet, destRect, SDL_FLIP_HORIZONTAL);
		}
		else if (mCurrentMovements & PlayerMovementBitField::MOVING_LEFT) // Render it facing left
		{
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
		std::cout << "Invalid sprite sheet trying to be rendered from: Playable Character" << std::endl;
}

// ----------------------------------------------------- //

void PlayableCharacter::Update(const float deltaTime, SDL_Event e, const Vector2D levelBounds, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	if (mIsAlive)
	{
		// If the player has no control then just dont change anything
		if (!mHasControl)
		{
			// Move the player in the direction set before
			ForcedMovementUpdate(deltaTime);

			// Leave the update as nothing else matters if we are in a forced movement
			return;
		}

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
	else
	{
		// If the player has died then update the time left before leaving the screen
		mDeathAnimationTime -= deltaTime;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::ForcedMovementUpdate(const float deltaTime)
{
	// Quick out
	if (!mForceMovementInDirectionSet)
		return;

	// -----------------------------------------------------------------------------------------------

	// Distance to travel this frame
	float movementDistance = kForcedMovementSpeed * deltaTime;

	// -----------------------------------------------------------------------------------------------

	// See which direction we are being forced to move
	switch (mForcedMovementDirection)
	{

	// If moving down
	case MOVEMENT_DIRECTION::DOWN:
		mScreenGridPosition.y += movementDistance; // Move the player on the screen in this direction
	break;

	// If moving up
	case MOVEMENT_DIRECTION::UP:
		mScreenGridPosition.y -= movementDistance; // Move the player on the screen in this direction
	break;

	// If moving right
	case MOVEMENT_DIRECTION::RIGHT:
		mScreenGridPosition.x += movementDistance; // Move the player on the screen in this direction
	break;

	// If moving left
	case MOVEMENT_DIRECTION::LEFT:
		mScreenGridPosition.x -= movementDistance; // Move the player on the screen in this direction
	break;
	}

	// -----------------------------------------------------------------------------------------------

	// Add this distance to the running total
	mForcedMovementDistanceTravelled += float(movementDistance);

	// -----------------------------------------------------------------------------------------------

	float capDistance = 0.0f;

	if (mForcedMovementDirection == MOVEMENT_DIRECTION::UP || mForcedMovementDirection == MOVEMENT_DIRECTION::DOWN)
	{
		// Have the check based on the Y axis
		capDistance = float(mCollisionBox.y * 1.2);
	}
	else
	{
		// Have the check based on the X axis
		capDistance = float(mCollisionBox.x * 1.2);
	}

	// -----------------------------------------------------------------------------------------------

	// Now check to see if we have moved far enough to determine this is far enough
	if (mForcedMovementDistanceTravelled > capDistance)
	{
		mForceMovementInDirectionSet = false;
		mForcedMovementDistanceTravelled = 0.0f;

		// Pipe check
		if (mExitingPipe)
		{
			mHasControl = true;
			mExitingPipe = false;
		}
	}

	// -----------------------------------------------------------------------------------------------
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
			if (mVelocity.y >= 0.0f)
			{
				mCurrentMovements &= ~(PlayerMovementBitField::HOLDING_JUMP);

				if (!(mCurrentMovements & PlayerMovementBitField::JUMPING))
				{
					mCurrentMovements &= ~(PlayerMovementBitField::JUMPING);
					mCurrentMovements &= ~(PlayerMovementBitField::HOLDING_JUMP);
				}

				if (!(mCurrentMovements & PlayerMovementBitField::MOVING_LEFT || mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT)
					&& mCurrentMovements & PlayerMovementBitField::RUNNING
					&& !(mCurrentMovements & PlayerMovementBitField::JUMPING))
				{
					HandleChangeInAnimations(PlayerMovementBitField::NONE, true);
				}

				mGrounded = true;
			}
			else 
			{
				mGrounded = false;
			}

			mVelocity.y = 0.0f;
			return CollisionPositionalData(returnData.collisionOccured, leftPos, rightPos, returnData.collisionWithInteractionLayer, returnData.collisionWithObjectLayer);
		}
		else
		{
			mGrounded = false;
		}
	}
	else // Going upwards
	{
		leftPos  = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x,                   mRealGridPosition.y - mCollisionBox.y + mCollisionBoxOffset.y + (mVelocity.y * deltaTime));
		rightPos = Vector2D(mRealGridPosition.x + mCollisionBoxOffset.x + mCollisionBox.x, mRealGridPosition.y - mCollisionBox.y + mCollisionBoxOffset.y + (mVelocity.y * deltaTime));

		mGrounded = false;


		CollisionPositionalData returnData = CheckYCollision(leftPos, rightPos, interactionLayer, objectLayer);

		if (returnData.collisionOccured && mVelocity.y < 0.0f)
		{
			mVelocity.y = 4.0f;
		}

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

			if (!(mCurrentMovements & PlayerMovementBitField::HOLDING_JUMP))
			{
				mCurrentMovements &= ~(PlayerMovementBitField::JUMPING);
			}
		}

		// Return that there was a collision
		return CollisionPositionalData(true, Vector2D(), Vector2D(), true, false);
	}

	MovementPrevention returnData1 = HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck1), returnData2 = HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck2);

	if (returnData1.StopYMovement || returnData2.StopYMovement)
	{
		if (returnData1.givesJump || returnData2.givesJump)
		{
			mGrounded       = false;
			
			// Give the slight jump boost 
			mVelocity.y        = kJumpInitialBoost * 0.75;

			if (mCurrentMovements & PlayerMovementBitField::JUMPING)
				mCurrentMovements |= PlayerMovementBitField::HOLDING_JUMP;
			else
				mCurrentMovements |= PlayerMovementBitField::JUMPING;
		}
		else if(!(mCurrentMovements & PlayerMovementBitField::HOLDING_JUMP))
		{
			mCurrentMovements &= ~(PlayerMovementBitField::JUMPING);
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
	// ------------------------------------------------------------------------------------------------------------------------------ 

	// Distance potentially moved
	Vector2D movementDistance = (mVelocity * deltaTime);                

	// Calculate the new position in the world based on collisions
	Vector2D newRealGridPos   = mRealGridPosition   + movementDistance; // Real position in the world according to this movement

	// Check for bounds
	if (newRealGridPos.y <= 0.0f)
		newRealGridPos.y = 0.0f;
	else if (newRealGridPos.y >= mLevelBounds.y + 1.7f)
	{
		// Only do this check if mario is alive
		if (mIsAlive)
		{
			// Then mario has gone off the bottom of the screen then he dies
			mHasControl = false;
			mIsAlive    = false;

			// Send the notification
			Notify(SUBJECT_NOTIFICATION_TYPES::PLAYER_DIED, "");
		}
	}

	// X bounds check
	if (newRealGridPos.x <= 0.0f)
		newRealGridPos.x = 0.0f;
	else if (newRealGridPos.x >= mLevelBounds.x - 1)
		newRealGridPos.x = mLevelBounds.x - 1;

	// ------------------------------------------------------------------------------------------------------------------------------ 

	// Collisions
	if (yCollision.collisionOccured)
	{
		// Check what the collision was with
		if (yCollision.collisionWithInteractionLayer && mVelocity.y >= 0.0f)
		{
			mRealGridPosition.y = int(mRealGridPosition.y) + 0.9;
		}

		//if (mVelocity.y > 0.0f)
		//	mVelocity.y = 0.0f;

	}
	else
	{
		// No collision so move freely to the new position
		mRealGridPosition.y = newRealGridPos.y;
	}


	// X collisions
	if(xCollision.collisionOccured)
	{
		// Stop x movement
		mVelocity.x = 0.0f;

		// Check what we are colliding with
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
	else
	{
		// No collision so move the player in that direction
		mRealGridPosition.x = newRealGridPos.x;
	}

	// ----------------------------------------------------------------------------------------------------------------------------------- //

	// Now calculate the new screen position and render reference point
	CalculateNewScreenPosAndRenderPos(movementDistance, xCollision, yCollision);

	// ----------------------------------------------------------------------------------------------------------------------------------- //
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
	// Handle input from the player
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{

		// Pressing jump key
		case SDLK_w:

			// Check to see if we are already jumping
			if (!(mCurrentMovements & PlayerMovementBitField::JUMPING))
			{
				// Change the animations to be in the jump animation
				HandleChangeInAnimations(PlayerMovementBitField::JUMPING, true);

				// Save that we are now jumping
				mCurrentMovements |= PlayerMovementBitField::JUMPING;
			}

			// If we are currently jumping and not already holding jump
			if (    (mCurrentMovements & PlayerMovementBitField::JUMPING) 
				&& !(mCurrentMovements & PlayerMovementBitField::HOLDING_JUMP))
			{
				// Save that we are holding jump
				mCurrentMovements |= PlayerMovementBitField::HOLDING_JUMP;
			}
		break;

		// Pressing move to the right
		case SDLK_d:

			// If we are not already moving to the right
			if (!(mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT))
			{
				// State that we were facing left before
				mWasFacingRight    = false;

				// Change the animations to reflect this change 
				HandleChangeInAnimations(PlayerMovementBitField::MOVING_RIGHT, true);

				// Save that we are now currently moving right
				mCurrentMovements |= PlayerMovementBitField::MOVING_RIGHT;
			}
			
		break;

		// If we are pressing to move to the left
		case SDLK_a:

			// If not already moving left
			if (!(mCurrentMovements & PlayerMovementBitField::MOVING_LEFT))
			{
				// Handle the change in animations
				HandleChangeInAnimations(PlayerMovementBitField::MOVING_LEFT, true);

				// Save the change in movements
				mCurrentMovements |= PlayerMovementBitField::MOVING_LEFT;
			}
		break;

		// Pressing down
		case SDLK_s:

			// If not already crouching
			if (!(mCurrentMovements & PlayerMovementBitField::CROUCHING))
			{
				// Update the animations to relfect this
				HandleChangeInAnimations(PlayerMovementBitField::CROUCHING, true);

				// save the change in animations
				mCurrentMovements |= PlayerMovementBitField::CROUCHING;
			}
		break;

		// Pressing sprint button
		case SDLK_RSHIFT:

			// If not already running 
			if (!(mCurrentMovements & PlayerMovementBitField::RUNNING))
			{
				// Change the animations to reflect this
				HandleChangeInAnimations(PlayerMovementBitField::RUNNING, true);

				// Store that we are now running
				mCurrentMovements |= PlayerMovementBitField::RUNNING;
			}
		break;
		}
	break;

	// Releasing keys
	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{

		// Releasing sprint
		case SDLK_RSHIFT:

			// If we are running
			if (mCurrentMovements & PlayerMovementBitField::RUNNING)
			{
				// Set the player to not be running
				HandleChangeInAnimations(PlayerMovementBitField::RUNNING, false);

				// Store that we are now not running
				mCurrentMovements &= ~(PlayerMovementBitField::RUNNING);
			}
		break;

		// Releasing pressing right
		case SDLK_d:

			// If we are currently going right
			if (mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT)
			{
				// Store that we were facing right
				mWasFacingRight    = true;

				// Change the change in animations
				HandleChangeInAnimations(PlayerMovementBitField::MOVING_RIGHT, false);

				// Store that we are now not going right
				mCurrentMovements &= ~(PlayerMovementBitField::MOVING_RIGHT);
			}
		break;

		// Releasing pressing left
		case SDLK_a:

			// If we are currently going left
			if (mCurrentMovements & PlayerMovementBitField::MOVING_LEFT)
			{
				// Store that we were going left
				mWasFacingRight    = false;

				// update animations
				HandleChangeInAnimations(PlayerMovementBitField::MOVING_LEFT, false);

				// Store the change
				mCurrentMovements &= ~(PlayerMovementBitField::MOVING_LEFT);
			}
		break;

		// releasing crouch
		case SDLK_s:

			// If we are currently crouching
			if (mCurrentMovements & PlayerMovementBitField::CROUCHING)
			{
				// Update animations
				HandleChangeInAnimations(PlayerMovementBitField::CROUCHING, false);

				// Store the change
				mCurrentMovements &= ~(PlayerMovementBitField::CROUCHING);
			}
		break;

		// Released jump
		case SDLK_w:

			// Not holding jump anymore
			if (mCurrentMovements & PlayerMovementBitField::JUMPING)
			{
				mCurrentMovements &= ~(PlayerMovementBitField::JUMPING);
				mCurrentMovements &= ~(PlayerMovementBitField::HOLDING_JUMP);

				// If you release jump whilst jumping you cannot continue the extra boost after the release
				mJumpHeldCurrentBoost = 0.0f;
			}
		break;
		}
	break;
	}

	// If the player is stood on the ground and is not jumping, but the animation state is that they are jumping then swap the animation state
	if (     mGrounded
		&& !(mCurrentMovements       & PlayerMovementBitField::JUMPING)
		&&  (mAnimationCurrentState == PlayerMovementBitField::JUMPING))
	{
		HandleChangeInAnimations(PlayerMovementBitField::JUMPING, false);
	}

	// If the player is not pressing anything - and they are not currently flying through the air in a jump - then set them to not be doing any animation
	if (   mCurrentMovements      == 0
		&& mAnimationCurrentState != PlayerMovementBitField::JUMPING)
	{
		HandleChangeInAnimations(PlayerMovementBitField::NONE, true);
	}

	// Triggering the start of a jump
	if (   mGrounded 
		&& mCurrentMovements & PlayerMovementBitField::JUMPING)
	{
		// Set the extra boost to be relative to the x speed the player is going
		if (mCurrentMovements & PlayerMovementBitField::HOLDING_JUMP)
			mJumpHeldCurrentBoost = kJumpHeldInitialBoost * std::max(float(abs(mVelocity.x) / kMaxHorizontalSpeedRunning), 0.1f);
		else
			mJumpHeldCurrentBoost = 0.0f;

		// Set the initial jump force
		mVelocity.y           = kJumpInitialBoost;

		// Set the player is no longer touching the ground
		mGrounded             = false;

		// Notify observers that the player has jumped
		Notify(SUBJECT_NOTIFICATION_TYPES::PLAYER_JUMPED, "");
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

	mIsAlive = true;

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

		Notify(SUBJECT_NOTIFICATION_TYPES::EXIT_PIPE, "");

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
	// ------------------------------------------------------------------------------------------------------------------------ 

	// Calculate the amount of friction that will be applied
	float frictionReduction = (kFrictionMultiplier * deltaTime);

	// Apply friction to the player's X movement - checking to see if we should go to a dead stop instead of slowing down
	if (abs(mVelocity.x) - frictionReduction < 0.0f)
		mVelocity.x = 0.0f;
	else
	{
		// Slow down the player in the direction required
		if (mVelocity.x > 0.0f)
			mVelocity.x -= frictionReduction;
		else if (mVelocity.x < 0.0f)
			mVelocity.x += frictionReduction;
	}

	// ------------------------------------------------------------------------------------------------------------------------ 

	// Calculate what the player's acceleration should be
	if (   mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT 
		|| mCurrentMovements & PlayerMovementBitField::MOVING_LEFT)
	{
		float multiplier = 1.0f;
		double speed     = 15.0;

		// Running check
		if (mCurrentMovements & PlayerMovementBitField::RUNNING && abs(mVelocity.x) > kMaxHorizontalSpeedWalking)
		{
			// Change the animations to reflect that the player is running past the max speed
			if (mAnimationCurrentState != PlayerMovementBitField::RUNNING)
				HandleChangeInAnimations(PlayerMovementBitField::RUNNING, true);

			// Set the multipler to be less as the net acceleration is going to be more
			multiplier = 0.7f;
		}

		// Set the acceleration based on the player's current moving direction
		if(mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT)
			mAcceleration.x = speed * multiplier;
		else if(mCurrentMovements & PlayerMovementBitField::MOVING_LEFT)
			mAcceleration.x = -speed * multiplier;
	}
	else
	{
		// Not going left or right so dont accelerate in that direction
		mAcceleration.x = 0.0f;
	}

	// ------------------------------------------------------------------------------------------------------------------------ 

	// Jumping calculations - you only get the extra jump height if you are holding run, otherwise it is just the regular jump
	if (   (mCurrentMovements & PlayerMovementBitField::JUMPING) 
		&& (mCurrentMovements & PlayerMovementBitField::HOLDING_JUMP))
	{
		// Reduce the current extra being added onto the jump
		mJumpHeldCurrentBoost += (kJumpHeldAccelerationDepreciationRate * deltaTime);

		// If holding down jump then keep mario going up until a point
		mAcceleration.y = mJumpHeldCurrentBoost;

		// Bounds check
		if (mJumpHeldCurrentBoost >= 0.0f)
			mJumpHeldCurrentBoost = 0.0f;
	}

	// ------------------------------------------------------------------------------------------------------------------------ 

	// Apply the acceleration of the player 
	mVelocity += Vector2D(mAcceleration.x * deltaTime, (mAcceleration.y + GRAVITY) * deltaTime);

	// ------------------------------------------------------------------------------------------------------------------------ 

	// Check to see if the player is sprinting
	if (mCurrentMovements & PlayerMovementBitField::RUNNING)
	{
		// If sprinting then cap the player to being the max speed whilst running
		if (mVelocity.x > kMaxHorizontalSpeedRunning)
			mVelocity.x = kMaxHorizontalSpeedRunning;
		else if (mVelocity.x < -kMaxHorizontalSpeedRunning)
			mVelocity.x = -kMaxHorizontalSpeedRunning;
	}
	else
	{
		// Cap the velocity to be the walking max speed on the X
		if (mVelocity.x > kMaxHorizontalSpeedWalking)
			mVelocity.x = kMaxHorizontalSpeedWalking;
		else if (mVelocity.x < -kMaxHorizontalSpeedWalking)
			mVelocity.x = -kMaxHorizontalSpeedWalking;
	}

	// ------------------------------------------------------------------------------------------------------------------------ 

	// Terminal velocity on the Y checks
	if (mVelocity.y > kMaxYVelocity)
		mVelocity.y = kMaxYVelocity;
	else if (mVelocity.y < -kMaxYVelocity)
		mVelocity.y = -kMaxYVelocity;

	// ------------------------------------------------------------------------------------------------------------------------ 

	// Update the player's animation speed based on how fast they are current moving on the x
	mTimePerFrame = kBaseTimePerFrame - ((abs(mVelocity.x) / kMaxHorizontalSpeedRunning) * kBaseTimePerFrame);

	// Output the player's current velocity
	std::cout << "X: " << mVelocity.x << "\t Y :" << mVelocity.y << std::endl;
}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimations(const float deltaTime)
{
	// Apply the change in time
	mTimeTillNextFrame -= deltaTime;

	// Check to see if we should move the frame on
	if (mTimeTillNextFrame <= 0.0f)
	{
		// Move the frame on
		mCurrentFrame++;

		// Check to see if the animation should loop
		if (mCurrentFrame > mEndFrame)
		{
			// Apply the loop
			mCurrentFrame = mStartFrame;
		}

		// Reset the animation time
		mTimeTillNextFrame = mTimePerFrame;
	}
}

// ----------------------------------------------------- //

void PlayableCharacter::UpdateAnimationsSmallMario(PlayerMovementBitField newMovement, bool goingInto)
{
	// Switch through the states to see what we are going into
	switch (newMovement)
	{

	// Holding jump shouldn't change the animation
	case PlayerMovementBitField::HOLDING_JUMP:
	case PlayerMovementBitField::ENTERING_PIPE_VERTICALLY:
	case PlayerMovementBitField::CROUCHING:
	case PlayerMovementBitField::SWIMMING:
	default:
		
	break;


	case PlayerMovementBitField::JUMPING:	

		// Going into the jumping animation
		if (goingInto)
		{
			// Update the animation state
			mAnimationCurrentState = PlayerMovementBitField::JUMPING;

			// If the player is running then go into the sprint jump animation
			if (mCurrentMovements & PlayerMovementBitField::RUNNING && abs(mVelocity.x) > kMaxHorizontalSpeedWalking)
			{
				// Set the correct animation frame
				mStartFrame   = 5;
				mEndFrame     = 5;
				mCurrentFrame = mStartFrame;
			}
			else
			{
				// As jumping overrides everything else animation wise just go into the jumping state
				mStartFrame   = 2;
				mEndFrame     = 2;
				mCurrentFrame = mStartFrame;
			}
		}
		else // Coming out of the jumping state, so check to see what state we should go into
		{
			if (mGrounded)
			{
				// If the player is currently running and moving in a direction then update to that animation
				if (    mCurrentMovements & PlayerMovementBitField::RUNNING 
					&& (mCurrentMovements & PlayerMovementBitField::MOVING_LEFT || mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT))
				{
					//if(mAnimationCurrentState == PlayerMovementBitField::JUMPING)

					// Go into the running animation state
					UpdateAnimationsSmallMario(PlayerMovementBitField::RUNNING, true);
				}
				// If not running and going left then go into the moving left animation state
				else if (mCurrentMovements & PlayerMovementBitField::MOVING_LEFT)
				{
					// Reset the state
					mAnimationCurrentState = PlayerMovementBitField::NONE;

					// Go into moving left
					UpdateAnimationsSmallMario(PlayerMovementBitField::MOVING_LEFT, true);
				}
				// If not running and not going left then go into the moving right animation state
				else if (mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT)
				{
					// Reset the state
					mAnimationCurrentState = PlayerMovementBitField::NONE;

					// Go into moving right
					UpdateAnimationsSmallMario(PlayerMovementBitField::MOVING_RIGHT, true);
				}
				else
				{
				    // If coming out of the jumping state and not moving in a direction the just go into the none state
					UpdateAnimationsSmallMario(PlayerMovementBitField::NONE, true);
				}
			}
		}
	break;

	// Going right
	case PlayerMovementBitField::MOVING_RIGHT:

		// if going into this state
		if (goingInto)
		{
			// Go into this state if not jumping 
			if (!(mCurrentMovements & PlayerMovementBitField::JUMPING) && mAnimationCurrentState != PlayerMovementBitField::JUMPING)
			{
				mAnimationCurrentState = PlayerMovementBitField::MOVING_RIGHT;
				mStartFrame            = 0;
				mEndFrame              = 1;
				mCurrentFrame          = mStartFrame;
			}
		}
		// Going out of this state
		else
		{
			if (mCurrentMovements & PlayerMovementBitField::JUMPING || mAnimationCurrentState == PlayerMovementBitField::JUMPING)
			{
				UpdateAnimationsSmallMario(PlayerMovementBitField::JUMPING, true);
			}
			else
			{
				UpdateAnimationsSmallMario(PlayerMovementBitField::NONE, true);
			}
		}
	break;

	// Going left
	case PlayerMovementBitField::MOVING_LEFT:

		// Going into the going left state
		if (goingInto)
		{
			// Go into this state if not jumping 
			if (!(mCurrentMovements & PlayerMovementBitField::JUMPING) && mAnimationCurrentState != PlayerMovementBitField::JUMPING)
			{
				mAnimationCurrentState = PlayerMovementBitField::MOVING_LEFT;
				mStartFrame            = 0;
				mEndFrame              = 1;
				mCurrentFrame          = mStartFrame;
			}
		}
		// Going out of this state
		else
		{
			if (mCurrentMovements & PlayerMovementBitField::JUMPING || mAnimationCurrentState == PlayerMovementBitField::JUMPING)
			{
				UpdateAnimationsSmallMario(PlayerMovementBitField::JUMPING, true);
			}
			else
			{
				UpdateAnimationsSmallMario(PlayerMovementBitField::NONE, true);
			}
		}
	break;

	case PlayerMovementBitField::NONE:

		if(goingInto)
		{
			// If going into this state then just remove all of the animations
			mAnimationCurrentState = PlayerMovementBitField::NONE;
			mEndFrame              = 0;
			mStartFrame            = 0;
			mCurrentFrame          = mStartFrame;
		}
	break;

	// Running
	case PlayerMovementBitField::RUNNING:

		// If going into the running state
		if (goingInto)
		{
			// if grounded and not moving in a direction then re-direct to the none state
			if (     mGrounded 
				&& !(mCurrentMovements & PlayerMovementBitField::MOVING_LEFT || mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT))
			{
				UpdateAnimationsSmallMario(PlayerMovementBitField::NONE, true);
				break;
			}

			// If not jumping and moving left or right, and your speed is high enough and grounded - then go into the sprint animation
			if (   !(mCurrentMovements & PlayerMovementBitField::JUMPING) 
				&&  (mCurrentMovements & PlayerMovementBitField::MOVING_LEFT || mCurrentMovements & PlayerMovementBitField::MOVING_RIGHT) 
				&& abs(mVelocity.x) > kMaxHorizontalSpeedWalking
				&& mGrounded)
			{
				mAnimationCurrentState = PlayerMovementBitField::RUNNING;
				mStartFrame            = 3;
				mEndFrame			   = 4;
				mCurrentFrame		   = mStartFrame;
			}
		}
		// Going out of the state
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

	Notify(SUBJECT_NOTIFICATION_TYPES::ENTERING_PIPE, "");
}

// ----------------------------------------------------- //