#include "PlayableCharacter.h"

#include "Constants_SMB3.h"

#include "InteractionLayer.h"
#include "ObjectLayer.h"

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
, kBaseMaxSpeed(3.5f)
, mMaxSpeed(kBaseMaxSpeed)
, kMaxSpeedOverall(8.0f)
, mApplyFriction(false)
, kFrictionMultiplier(5.0f)
, mCurrentMovements(MovementBitField::NONE)
, mPSpeedAccumulatorRate(1.5f)

, mTimePerFrame(timePerFrame)
, mTimeTillNextFrame(timePerFrame)
, mPriorFrameMovements(0)

, mPowerUpState(POWER_UP_TYPE::NONE)
, mRenderer(renderer)
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
			               int(mScreenGridPosition.y * RESOLUTION_OF_SPRITES) - int(float(RESOLUTION_OF_SPRITES) * mCollisionBox.y) + 1, // The + 1 makes mario render so that it looks like he is on the floor
			               int(mSingleSpriteWidth), 
			               int(mSingleSpriteHeight) };

		// If going right, or if we were going right and are now not moving
		if (mCurrentMovements & MovementBitField::MOVING_RIGHT || mVelocity.x > 0.0f)
		{
			// Render it facing Right
			mSpriteSheet->Render(portionOfSpriteSheet, destRect, SDL_FLIP_HORIZONTAL);
		}
		else
		{
			// Render it facing left
			mSpriteSheet->Render(portionOfSpriteSheet, destRect, SDL_FLIP_NONE);
		}
	}
	else
		std::cout << "Invalid sprite sheet trying to be rendered from: Base Character" << std::endl;
}

// ----------------------------------------------------- //

void PlayableCharacter::Update(const float deltaTime, SDL_Event e, const Vector2D levelBounds, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	// First handle input to see if the player wants to move in a direction
	HandleMovementInput(e);

	// Now update the player's physics
	UpdatePhysics(deltaTime);

	unsigned int collisionCount = 0;

	// Position to pass into the calculate position function to determine if the screen should scroll or not
	double potentialNewXPos = mRealGridPosition.x + (mVelocity.x * deltaTime);
	double potentialNewYPos = mRealGridPosition.y + (mVelocity.y * deltaTime);

	// First check if the player can move on the X axis
	Vector2D     newBottomPos, newTopPos;

	//if (mVelocity.x > 0.0f)
	if(mCurrentMovements & MovementBitField::MOVING_RIGHT)
	{
		// Going to the right
		newBottomPos = Vector2D(mRealGridPosition.x + mCollisionBox.x + (mVelocity.x * deltaTime), mRealGridPosition.y);
		newTopPos    = Vector2D(mRealGridPosition.x + mCollisionBox.x + (mVelocity.x * deltaTime), mRealGridPosition.y - mCollisionBox.y);
	}
	else if(mCurrentMovements & MovementBitField::MOVING_LEFT)
	{
		// Going to the left
		newBottomPos = Vector2D(mRealGridPosition.x + (mVelocity.x * deltaTime), mRealGridPosition.y);
		newTopPos    = Vector2D(mRealGridPosition.x + (mVelocity.x * deltaTime), mRealGridPosition.y - mCollisionBox.y);
	}

	if (CheckXCollision(newBottomPos, newTopPos, interactionLayer, objectLayer, potentialNewXPos))
		collisionCount++;

	// Now handle the Y axis
	if (mVelocity.y != 0.0f)
	{
		Vector2D     newLeftPos, newRightPos;

		if (mVelocity.y > 0.0f)
		{
			// Going downwards
			newLeftPos  = Vector2D(mRealGridPosition.x,                   mRealGridPosition.y + (mVelocity.y * deltaTime));
			newRightPos = Vector2D(mRealGridPosition.x + mCollisionBox.x, mRealGridPosition.y + (mVelocity.y * deltaTime));
		}
		else
		{
			// Going upwards
			newLeftPos  = Vector2D(mRealGridPosition.x,                   mRealGridPosition.y - mCollisionBox.y + (mVelocity.y * deltaTime));
			newRightPos = Vector2D(mRealGridPosition.x + mCollisionBox.x, mRealGridPosition.y - mCollisionBox.y + (mVelocity.y * deltaTime));
		}

		if (CheckYCollision(newLeftPos, newRightPos, interactionLayer, objectLayer, potentialNewYPos))
			collisionCount++;
	}

	// If we are not prevented from moving in both directions then calculate where we are going to be next frame
	if(collisionCount < 2)
		CalculateNewPosition(deltaTime, Vector2D(potentialNewXPos, potentialNewYPos));

	UpdateAnimations(deltaTime);
}

// ----------------------------------------------------- //

bool PlayableCharacter::CheckXCollision(const Vector2D positionToCheck1, const Vector2D positionToCheck2, InteractableLayer* interactionLayer, ObjectLayer* objectLayer, double& newXPosRef)
{
	// Check to see if we have hit the terrain on the X movement
	if (   HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck1)  || HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck2)
		|| HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck1) || HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck2))
	{
		// Then dont move to the new position
		newXPosRef      = mRealGridPosition.x;

		// Stop the player from moving
		mVelocity.x     = 0.0f;

		return true;
	}

	return false;
}

// ----------------------------------------------------- //

bool PlayableCharacter::CheckYCollision(const Vector2D positionToCheck1, const Vector2D positionToCheck2, InteractableLayer* interactionLayer, ObjectLayer* objectLayer, double& newYPosRef)
{
	if (   HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck1)  || HandleCollisionsWithInteractionLayer(interactionLayer, positionToCheck2)
		|| HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck1) || HandleCollisionsWithInteractionObjectLayer(objectLayer, positionToCheck2))
	{
		// Then dont move in the y axis
		newYPosRef      = mRealGridPosition.y;

		// Stop the player from moving
		mVelocity.y     = 0.0f;

		mApplyFriction = true;

		return true;
	}

	mApplyFriction = false;
	return false;
}

// ----------------------------------------------------- //

void PlayableCharacter::CalculateScreenBoundsPosition(const Vector2D spawnPoint)
{
	// If the player is near a boundary then they need specific positioning - starting with the Y axis
	if (spawnPoint.y + (PLAYABLE_SCREEN_AREA_HEIGHT / 2) > mLevelBounds.y)
	{
		mScreenGridPosition.y = (BACKGROUND_SPRITE_RENDER_HEIGHT - 1) - (mLevelBounds.y - spawnPoint.y);
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

void PlayableCharacter::CalculateNewPosition(const float deltaTime, const Vector2D newPos)
{
	// Calculate the new potential positions
	Vector2D movementDistance = (mVelocity * deltaTime);

	Vector2D newRealGridPos   = newPos;
	Vector2D newScreenGridPos = mScreenGridPosition + movementDistance;

	// First cap the new positions to their relative bounds - starting with the y axis
	if (newScreenGridPos.y <= 0.0f)
		newScreenGridPos.y = 0.0f;
	else if (newScreenGridPos.y >= PLAYABLE_SCREEN_AREA_HEIGHT + 4)
		newScreenGridPos.y = PLAYABLE_SCREEN_AREA_HEIGHT + 4;

	if (newRealGridPos.y <= 0.0f)
		newRealGridPos.y = 0.0f;
	else if (newRealGridPos.y >= mLevelBounds.y + 2)
		newRealGridPos.y = mLevelBounds.y + 2;

	// Now for the x axis
	if (newScreenGridPos.x <= 0.0f)
		newScreenGridPos.x = 0.0f;
	else if (newScreenGridPos.x >= (PLAYABLE_SCREEN_AREA_WIDTH - 1))
		newScreenGridPos.x = (PLAYABLE_SCREEN_AREA_HEIGHT - 1);

	if (newRealGridPos.x <= -1.0f)
		newRealGridPos.x = -1.0f;
	else if (newRealGridPos.x >= mLevelBounds.x + 1)
		newRealGridPos.x = mLevelBounds.x + 1;

	// First update the real grid position of the player so that when the player moves their actual point in the world moves as well
	mRealGridPosition = newRealGridPos;

	// ----------------------------------------------------------------------------------------------------------------------------------- //

	// If we want to scroll the screen then scroll - X axis
	if (newScreenGridPos.x > (PLAYABLE_SCREEN_AREA_WIDTH  - (PLAYABLE_SCREEN_AREA_WIDTH / 2.0f)) + 1 && movementDistance.x > 0.0f)
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

	if (mRenderRefencePoint.y < 0.0f)
	{
		mRenderRefencePoint.y = 0.0f;
	}
	else if (mRenderRefencePoint.y > mLevelBounds.y - BACKGROUND_SPRITE_RENDER_HEIGHT + 1)
	{
		mRenderRefencePoint.y = mLevelBounds.y - BACKGROUND_SPRITE_RENDER_HEIGHT + 1;
	}

	// ----------------------------------------------------------------------------------------------------------------------------------- //

	//std::cout << "Y Pos: " << mRealGridPosition.y << "\tScreen Y: " << mScreenGridPosition.y  << std::endl;
}

// ----------------------------------------------------- //

void PlayableCharacter::HandleMovementInput(SDL_Event e)
{
	double speed      = 10.0f;
	double multiplier = 1.0f;

	// If the player is currently running then set the speed to be correct
	if (mCurrentMovements & MovementBitField::RUNNING)
	{
		multiplier = 2.0f;
	}

	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_RSHIFT:
			// If not running
			if (!(mCurrentMovements & MovementBitField::RUNNING))
			{
				// Store that the player is now running
				mCurrentMovements |= MovementBitField::RUNNING;
			}
		break;

		case SDLK_d:
			mCurrentMovements |= MovementBitField::MOVING_RIGHT;

			mAcceleration.x = multiplier * speed;
		break;

		case SDLK_a:
			mCurrentMovements |= MovementBitField::MOVING_LEFT;

			mAcceleration.x = -multiplier * speed;
		break;

		case SDLK_s:
			mCurrentMovements |= MovementBitField::FALLING;

			mAcceleration.y = multiplier * speed;
		break;

		case SDLK_w:
			mCurrentMovements |= MovementBitField::JUMPING;

			mAcceleration.y = -multiplier * speed;
		break;
		}
	break;

	case SDL_KEYUP:
		switch (e.key.keysym.sym)
		{
		case SDLK_RSHIFT:
			// Set the player to not be running
			mCurrentMovements &= ~(MovementBitField::RUNNING);

			mMaxSpeed = kBaseMaxSpeed;
		break;

		case SDLK_d:
			mCurrentMovements &= ~(MovementBitField::MOVING_RIGHT);

			mAcceleration.x = 0.0f;
		break;

		case SDLK_a:
			mCurrentMovements &= ~(MovementBitField::MOVING_LEFT);

			mAcceleration.x = 0.0f;
		break;

		case SDLK_s:
			mCurrentMovements &= ~(MovementBitField::FALLING);
		break;

		case SDLK_w:
			mCurrentMovements &= ~(MovementBitField::JUMPING);

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

bool PlayableCharacter::HandleCollisionsWithInteractionObjectLayer(ObjectLayer* objectLayer, const Vector2D newPos)
{
	return false;
}

// ----------------------------------------------------- //

void PlayableCharacter::UpdatePhysics(const float deltaTime)
{
	// Apply friction to the player's movement 
	if (mApplyFriction)
	{
		float frictionReduction = (kFrictionMultiplier * deltaTime);

		if (abs(mVelocity.x) > 0.05f)
		{
			if (mVelocity.x > 0.0f)
				mVelocity.x -= frictionReduction;
			else if (mVelocity.x < 0.0f)
				mVelocity.x += frictionReduction;
		}
	}

	// Apply the acceleration of the player 
	mVelocity += Vector2D(mAcceleration.x * deltaTime, (mAcceleration.y + GRAVITY) * deltaTime);

	if (!(mCurrentMovements & MovementBitField::RUNNING))
	{
		// Cap the velocity to the max speed if it exceeds it
		if (abs(mVelocity.x) > kBaseMaxSpeed)
		{
			if (mVelocity.x > 0.0f)
				mVelocity.x = kBaseMaxSpeed;
			else
				mVelocity.x = -kBaseMaxSpeed;
		}

		if (abs(mVelocity.y) > kBaseMaxSpeed)
		{
			if(mVelocity.y > 0.0f)
				mVelocity.y = kBaseMaxSpeed;
			else
				mVelocity.y = -kBaseMaxSpeed;
		}
	}
	else
	{

		// We are running so we need to increase the P-Speed counter - and therefore increase the player's max speed
		mMaxSpeed += mPSpeedAccumulatorRate * deltaTime;

		if (mMaxSpeed > kMaxSpeedOverall)
			mMaxSpeed = kMaxSpeedOverall;

		// Cap the velocity to the max speed if it exceeds it
		if (abs(mVelocity.x) > mMaxSpeed)
		{
			if (mVelocity.x > 0.0f)
				mVelocity.x = mMaxSpeed;
			else
				mVelocity.x = -mMaxSpeed;
		}

		if (abs(mVelocity.y) > mMaxSpeed)
		{
			if (mVelocity.y > 0.0f)
				mVelocity.y = mMaxSpeed;
			else
				mVelocity.y = -mMaxSpeed;
		}
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
		mStartFrame = 0;
		mEndFrame = 0;
		mCurrentFrame = 0;

		mPriorFrameMovements = mCurrentMovements;

		return;
	}

	// If turning around sharply
	if ((mCurrentMovements & MovementBitField::MOVING_RIGHT && mPriorFrameMovements & MovementBitField::MOVING_LEFT) ||
		(mCurrentMovements & MovementBitField::MOVING_LEFT && mPriorFrameMovements & MovementBitField::MOVING_RIGHT))
	{
		mStartFrame = 6;
		mEndFrame = 6;
		mCurrentFrame = mStartFrame;

		mPriorFrameMovements = mCurrentMovements;

		return;
	}

	// Check to see if we need to start walking
	if (mCurrentMovements & MovementBitField::MOVING_RIGHT || mCurrentMovements & MovementBitField::MOVING_LEFT)
	{
		mStartFrame = 0;
		mEndFrame = 1;
		mCurrentFrame = mStartFrame;

		mPriorFrameMovements = mCurrentMovements;

		return;
	}

	if (mCurrentMovements & MovementBitField::JUMPING)
	{
		if (mCurrentMovements & MovementBitField::RUNNING)
		{
			// Full sprint jump
			mStartFrame = 5;
			mEndFrame = 5;
			mCurrentFrame = mStartFrame;
		}
		else
		{
			// Regular jump
			mStartFrame = 2;
			mEndFrame = 2;
			mCurrentFrame = mStartFrame;
		}

		mPriorFrameMovements = mCurrentMovements;

		return;
	}

	if (mCurrentMovements & MovementBitField::RUNNING)
	{
		mStartFrame = 3;
		mEndFrame = 4;
		mCurrentFrame = mStartFrame;

		mPriorFrameMovements = mCurrentMovements;

		return;
	}

	// If going down/up a pipe
	if (mCurrentMovements & MovementBitField::ENTERING_PIPE_VERTICALLY)
	{
		mStartFrame = 7;
		mEndFrame = 7;
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
