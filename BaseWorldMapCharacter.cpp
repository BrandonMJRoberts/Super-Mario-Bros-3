#include "BaseWorldMapCharacter.h"
#include "GameManager_SMB3.h"

#include "Constants_SMB3.h"

#include <SDL.h>

// ------------------------------------------------------------------------------------------------------------------------------- //

BaseWorldMapCharacter::BaseWorldMapCharacter(SDL_Renderer* renderer, const std::string filePathToSpriteSheet, const Vector2D startPosition, const unsigned int spritesOnWidth, const unsigned int spritesOnHeight, const float timePerAnimationFrame) : mTimePerAnimationFrame(timePerAnimationFrame)
{
	// Set the default values for the variables
	mPosition                     = startPosition;
	mAmountOfSpritesOnHeight      = spritesOnHeight;
	mAmountOfSpritesOnWidth       = spritesOnWidth;
	mTimeRemainingTillFrameChange = mTimePerAnimationFrame;
	mSingleSpriteWidth            = 0;
	mSingleSpriteHeight           = 0;

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

	mCurrentPowerUpState = CHARACTER_MAP_POWER_UP_STATE::FROG;
	ChangePowerUpState(CHARACTER_MAP_POWER_UP_STATE::LEAF);
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
		portionOfSpriteSheet.x = (mCurrentFrame % mAmountOfSpritesOnWidth) * RESOLUTION_OF_SPRITES;
		portionOfSpriteSheet.y = (mCurrentFrame / mAmountOfSpritesOnWidth) * DOUBLE_RESOLUTION_OF_SPRITES;
		portionOfSpriteSheet.w = mSingleSpriteWidth;
		portionOfSpriteSheet.h = mSingleSpriteHeight;

		// First convert the actual position into a grid position
		Vector2D renderOffset            = Commons_SMB3::ConvertFromGridPositionToRealPositionReturn(GameManager_SMB3::GetInstance()->GetWorldMapRenderOffset(), RESOLUTION_OF_SPRITES);

		// Calculate where we need to draw the character
		destRect.x = (int)(mPosition.x + renderOffset.x);
		destRect.y = (int)(mPosition.y + renderOffset.y - (mSingleSpriteHeight / 2));
		destRect.w = mSingleSpriteWidth;
		destRect.h = mSingleSpriteHeight;

		mSpriteSheet->Render(portionOfSpriteSheet, destRect);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------- //

void BaseWorldMapCharacter::Update(const float deltaTime)
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

	// Now handle input for moving around the map
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