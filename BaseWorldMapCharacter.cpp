#include "BaseWorldMapCharacter.h"
#include "GameManager_SMB3.h"

#include "Constants_SMB3.h"
#include "NodeMap_WorldMap.h"

#include <SDL.h>

Vector2D           BaseWorldMapCharacter::mPosition(0, 0);
MOVEMENT_DIRECTION BaseWorldMapCharacter::mPriorMovementDirection(MOVEMENT_DIRECTION::NONE);

// ------------------------------------------------------------------------------------------------------------------------------- //

BaseWorldMapCharacter::BaseWorldMapCharacter(SDL_Renderer* renderer
	, const std::string  filePathToSpriteSheet
	, const Vector2D     startPosition
	, const unsigned int spritesOnWidth
	, const unsigned int spritesOnHeight
	, const float        timePerAnimationFrame)

	: Subject()
	, mTimePerAnimationFrame(timePerAnimationFrame)
	, mMoveToPosition(mPosition)
	, mAmountOfSpritesOnWidth(spritesOnWidth)
	, mAmountOfSpritesOnHeight(spritesOnHeight)
	, mTimeRemainingTillFrameChange(timePerAnimationFrame)
	, mSingleSpriteWidth(0)
	, mSingleSpriteHeight(0)
	, mMovementDirection(MOVEMENT_DIRECTION::NONE)
	, mRequestedMovementDirection(MOVEMENT_DIRECTION::NONE)
	, mTimeTillNextMove(MOVEMENT_DELAY_WORLD_MAP)
	, mButtonIsPressed(false)
	, mCurrentPowerUpState(CHARACTER_MAP_POWER_UP_STATE::TANOOKI)
	, mCurrentFrame(0)
	, mEndFrame(0)
	, mStartFrame(0)
{
	if (mPosition.x == 0 && mPosition.y == 0)
		mPosition = startPosition;

	// Load in the sprite sheet
	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
	{
		std::cout << "Failed to load in the sprite sheet for the world map character." << std::endl;
		return;
	}

	// Now calculate the default sprite width and height
	mSingleSpriteWidth  = mSpriteSheet->GetWidth() / mAmountOfSpritesOnWidth;
	mSingleSpriteHeight = mSpriteSheet->GetHeight() / mAmountOfSpritesOnHeight;

	// Set the starting state to be small mario
	ChangePowerUpState(CHARACTER_MAP_POWER_UP_STATE::SMALL);
}

// ------------------------------------------------------------------------------------------------------------------------------- //

BaseWorldMapCharacter::~BaseWorldMapCharacter()
{
	// Clean up the memory used
	delete mSpriteSheet;
	mSpriteSheet = nullptr;
}

// ------------------------------------------------------------------------------------------------------------------------------- //

void BaseWorldMapCharacter::Render()
{
	// Just render the first sprite for now
	if (mSpriteSheet)
	{
		SDL_Rect portionOfSpriteSheet, destRect;

		// Get the correct position on the sprite sheet
		portionOfSpriteSheet.x = (mCurrentFrame % mAmountOfSpritesOnWidth) * mSingleSpriteWidth;
		portionOfSpriteSheet.y = (mCurrentFrame / mAmountOfSpritesOnWidth) * mSingleSpriteHeight;
		portionOfSpriteSheet.w = mSingleSpriteWidth;
		portionOfSpriteSheet.h = mSingleSpriteHeight;

		// First convert the actual position into a grid position
		Vector2D gridRenderOffset            = GameManager_SMB3::GetInstance()->GetWorldMapRenderOffset();

		// Calculate where we need to draw the character
		destRect.x = int((mPosition.x + gridRenderOffset.x) * RESOLUTION_OF_SPRITES);
		destRect.y = int((mPosition.y + gridRenderOffset.y) * RESOLUTION_OF_SPRITES) - (mSingleSpriteHeight / 2);
		destRect.w = mSingleSpriteWidth;
		destRect.h = mSingleSpriteHeight;

		mSpriteSheet->Render(portionOfSpriteSheet, destRect);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------- //

void BaseWorldMapCharacter::Update(const float deltaTime, NodeMap_WorldMap& nodeMapRef, SDL_Event e)
{
	// Add the time that has progressed onto the animation timer, so that the character animates
	mTimeRemainingTillFrameChange -= deltaTime;

	if (mTimeRemainingTillFrameChange <= 0.0f)
	{
		mTimeRemainingTillFrameChange = mTimePerAnimationFrame;

		mCurrentFrame++;

		if (mCurrentFrame > mEndFrame)
			mCurrentFrame = mStartFrame;
	}

	// If the player is moving then move them in the direction they have requested - if they can go that way
	if (GetIsMoving())
	{
		// Now move towards the taget position
		Vector2D movementDir = (mMoveToPosition - mPosition);

		double   accuaracy = 0.05f;

		if (movementDir.length() < accuaracy)
		{
			mPosition = mMoveToPosition;

			if (!CanTurnToDirection(mMovementDirection, nodeMapRef) || nodeMapRef.GetDataPointIsDot(nodeMapRef.GetSpecificDataPoint(mPosition)) || nodeMapRef.GetValueIsLevel(nodeMapRef.GetSpecificDataPoint(mPosition)))
			{
				mPriorMovementDirection     = mMovementDirection;
				mMovementDirection          = MOVEMENT_DIRECTION::NONE;
				mRequestedMovementDirection = MOVEMENT_DIRECTION::NONE;
				return;
			}
			else
			{
				SetNewMoveToPosition();
			}
		}

		// Now normalise the direction
		movementDir.normalise();

		// Now move in this direction
		mPosition = mPosition + (movementDir * deltaTime * WORLD_MAP_PLAYER_MOVEMENT_SPEED);
	}
	else // The player can only start moving if they are not currently moving
	{
		// If the movement button is held down the add a short delay to movement like the origionl does 
		if (mButtonIsPressed)
		{
			// Now take time off of the delay
			mTimeTillNextMove -= deltaTime;

			if (mTimeTillNextMove <= 0.0f)
			{
				mButtonIsPressed  = false;
				mTimeTillNextMove = MOVEMENT_DELAY_WORLD_MAP;
			}
		}

		// Check if the player has selected to move
		if(!mButtonIsPressed)
			CheckForMovementInput(e);

		// Now check if we can turn to this direction
		if (CanTurnToDirection(mRequestedMovementDirection, nodeMapRef))
		{
			Notify(SUBJECT_NOTIFICATION_TYPES::PLAYER_MOVED_ON_WORLD_MAP, "");

			mPriorMovementDirection     = mMovementDirection;
			mMovementDirection          = mRequestedMovementDirection;
			mRequestedMovementDirection = MOVEMENT_DIRECTION::NONE;

			SetNewMoveToPosition();
		}
		else
			mRequestedMovementDirection = MOVEMENT_DIRECTION::NONE;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------- //

void BaseWorldMapCharacter::SetNewMoveToPosition()
{
	// Set the move to position
	switch (mMovementDirection)
	{
	case MOVEMENT_DIRECTION::RIGHT:
		mMoveToPosition = Vector2D(double(int(mPosition.x)) + 1, int(mPosition.y));
		mPosition.y = (int)mPosition.y;
	break;

	case MOVEMENT_DIRECTION::LEFT:
		mMoveToPosition = Vector2D(double(int(mPosition.x)) - 1, int(mPosition.y));
		mPosition.y = (int)mPosition.y;
	break;

	case MOVEMENT_DIRECTION::UP:
		mMoveToPosition = Vector2D(int(mPosition.x), double(int(mPosition.y)) - 1);
		mPosition.x = (int)mPosition.x;
	break;

	case MOVEMENT_DIRECTION::DOWN:
		mMoveToPosition = Vector2D(int(mPosition.x), double(int(mPosition.y)) + 1);
		mPosition.x = (int)mPosition.x;
	break;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------- //

void BaseWorldMapCharacter::ChangePowerUpState(CHARACTER_MAP_POWER_UP_STATE newState)
{
	if (newState != mCurrentPowerUpState)
	{
		// Set the new state
		mCurrentPowerUpState = newState;

		// Making sure that the correct animation data is set based on the current power up state
		switch (newState)
		{
		default:
		case CHARACTER_MAP_POWER_UP_STATE::SMALL:
			mStartFrame   = 0;
			mCurrentFrame = 0;
			mEndFrame     = 1;
		break;

		case CHARACTER_MAP_POWER_UP_STATE::FIRE:
			mStartFrame   = 12;
			mCurrentFrame = 12;
			mEndFrame     = 13;
		break;

		case CHARACTER_MAP_POWER_UP_STATE::FROG:
			mStartFrame   = 16;
			mCurrentFrame = 16;
			mEndFrame     = 17;
		break;

		case CHARACTER_MAP_POWER_UP_STATE::HAMMER:
			mStartFrame   = 18;
			mCurrentFrame = 18;
			mEndFrame     = 19;
		break;

		case CHARACTER_MAP_POWER_UP_STATE::LEAF:
			mStartFrame   = 14;
			mCurrentFrame = 14;
			mEndFrame     = 15;
		break;

		case CHARACTER_MAP_POWER_UP_STATE::MUSHROOM:
			mStartFrame   = 2;
			mCurrentFrame = 2;
			mEndFrame     = 3;
		break;

		case CHARACTER_MAP_POWER_UP_STATE::P_TANOOKI:
			mStartFrame   = 6;
			mCurrentFrame = 6;
			mEndFrame     = 7;
		break;

		case CHARACTER_MAP_POWER_UP_STATE::TANOOKI:
			mStartFrame    = 4;
			mCurrentFrame  = 4;
			mEndFrame      = 5;
		break;
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------------- //

void BaseWorldMapCharacter::CheckForMovementInput(SDL_Event e)
{
	// Now handle input for moving around the map
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case SDLK_d:
			mRequestedMovementDirection = MOVEMENT_DIRECTION::RIGHT;
			mButtonIsPressed            = true;
		break;

		case SDLK_w:
			mRequestedMovementDirection = MOVEMENT_DIRECTION::UP;
			mButtonIsPressed            = true;
		break;

		case SDLK_s:
			mRequestedMovementDirection = MOVEMENT_DIRECTION::DOWN;
			mButtonIsPressed            = true;
		break;

		case SDLK_a:
			mRequestedMovementDirection = MOVEMENT_DIRECTION::LEFT;
			mButtonIsPressed            = true;
		break;
		}
	break;

	default:
	break;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------- //

bool BaseWorldMapCharacter::CanTurnToDirection(MOVEMENT_DIRECTION newDir, NodeMap_WorldMap& nodeMapRef)
{
	switch (newDir)
	{
	case MOVEMENT_DIRECTION::RIGHT:
		// if where we are going is clear
		if (nodeMapRef.GetPositionIsWalkable(Vector2D(int(mPosition.x + 1), int(mPosition.y))))
		{
			// If we are not currently stood on a level then we can move there
			if(!nodeMapRef.GetValueIsLevel(nodeMapRef.GetSpecificDataPoint(mPosition)))
				return true;
			else
			{
				// Then we are stood on a level, check to see if it is cleared
				if (nodeMapRef.GetPositionIsAClearedLevel(mPosition))
					return true;
				else
				{
					// Now we know we are going to a clear space, whilst stood on a non-cleared level
					// Now check to see if we are going back in the direction that we came from - if so then allow the movement
					if (mPriorMovementDirection == MOVEMENT_DIRECTION::LEFT)
						return true;
				}
			}
		}
	break;

	case MOVEMENT_DIRECTION::LEFT:
		// if where we are going is clear
		if (nodeMapRef.GetPositionIsWalkable(Vector2D(int(mPosition.x - 1), int(mPosition.y))))
		{
			// If we are not currently stood on a level then we can move there
			if (!nodeMapRef.GetValueIsLevel(nodeMapRef.GetSpecificDataPoint(mPosition)))
				return true;
			else
			{
				// Then we are stood on a level, check to see if it is cleared
				if (nodeMapRef.GetPositionIsAClearedLevel(mPosition))
					return true;
				else
				{
					// Now we know we are going to a clear space, whilst stood on a non-cleared level
					// Now check to see if we are going back in the direction that we came from - if so then allow the movement
					if (mPriorMovementDirection == MOVEMENT_DIRECTION::RIGHT)
						return true;
				}
			}
		}
	break;

	case MOVEMENT_DIRECTION::UP:
		// if where we are going is clear
		if (nodeMapRef.GetPositionIsWalkable(Vector2D(int(mPosition.x), double(int(mPosition.y)) - 1)))
		{
			// If we are not currently stood on a level then we can move there
			if (!nodeMapRef.GetValueIsLevel(nodeMapRef.GetSpecificDataPoint(mPosition)))
				return true;
			else
			{
				// Then we are stood on a level, check to see if it is cleared
				if (nodeMapRef.GetPositionIsAClearedLevel(mPosition))
					return true;
				else
				{
					// Now we know we are going to a clear space, whilst stood on a non-cleared level
					// Now check to see if we are going back in the direction that we came from - if so then allow the movement
					if (mPriorMovementDirection == MOVEMENT_DIRECTION::DOWN)
						return true;
				}
			}
		}
	break;

	case MOVEMENT_DIRECTION::DOWN:
		// if where we are going is clear
		if (nodeMapRef.GetPositionIsWalkable(Vector2D(int(mPosition.x), double(int(mPosition.y)) + 1)))
		{
			// If we are not currently stood on a level then we can move there
			if (!nodeMapRef.GetValueIsLevel(nodeMapRef.GetSpecificDataPoint(mPosition)))
				return true;
			else
			{
				// Then we are stood on a level, check to see if it is cleared
				if (nodeMapRef.GetPositionIsAClearedLevel(mPosition))
					return true;
				else
				{
					// Now we know we are going to a clear space, whilst stood on a non-cleared level
					// Now check to see if we are going back in the direction that we came from - if so then allow the movement
					if (mPriorMovementDirection == MOVEMENT_DIRECTION::UP)
						return true;
				}
			}
		}
	break;
	}

	return false;
}

// ------------------------------------------------------------------------------------------------------------------------------- //