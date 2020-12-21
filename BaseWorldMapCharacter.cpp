#include "BaseWorldMapCharacter.h"
#include "GameManager_SMB3.h"

#include "Constants_SMB3.h"

#include <SDL.h>

// ------------------------------------------------------------------------------------------------------------------------------- //

BaseWorldMapCharacter::BaseWorldMapCharacter(SDL_Renderer* renderer, std::string filePathToSpriteSheet, Vector2D startPosition, unsigned int spritesOnWidth, unsigned int spritesOnHeight, const float timePerAnimationFrame) : mTimePerAnimationFrame(timePerAnimationFrame)
{
	mSpriteSheet = new Texture2D(renderer);
	if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
	{
		std::cout << "Failed to load in the sprite sheet for the world map character." << std::endl;
		return;
	}

	mPosition                = startPosition;

	mAmountOfSpritesOnHeight = spritesOnHeight;
	mAmountOfSpritesOnWidth  = spritesOnWidth;

//	mCurrentPowerUpState     = GameManager_SMB3::GetInstance()->GetLuigiCurrentPowerUpState();


	// Making sure that the correct animation data is set based on the current power up state
	switch (mCurrentPowerUpState)
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
		mStartFrame   = 4;
		mCurrentFrame = 4;
		mEndFrame     = 5;
	break;
	}

	mTimeRemainingTillFrameChange = mTimePerAnimationFrame;

	mSingleSpriteWidth = mSpriteSheet->GetWidth() / mAmountOfSpritesOnWidth;
	mSingleSpriteHeight = mSpriteSheet->GetHeight() / mAmountOfSpritesOnHeight;
}

// ------------------------------------------------------------------------------------------------------------------------------- //

BaseWorldMapCharacter::~BaseWorldMapCharacter()
{
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
		portionOfSpriteSheet.y = (int)(mCurrentFrame / mAmountOfSpritesOnWidth) * RESOLUTION_OF_SPRITES;
		portionOfSpriteSheet.w = mSingleSpriteWidth;
		portionOfSpriteSheet.h = mSingleSpriteHeight;

		// First convert the actual position into a grid position
		Vector2D gridReferencePoint = Commons_SMB3::ConvertFromRealPositionToGridPositionReturn(GameManager_SMB3::GetInstance()->GetRenderReferencePoint(), RESOLUTION_OF_SPRITES);

		// Now get the distance from this position to the next one, so that we can scroll smoothly
		Vector2D interGridPositionOffset = Vector2D(((int(gridReferencePoint.x) - gridReferencePoint.x) * RESOLUTION_OF_SPRITES), ((int(gridReferencePoint.y) - gridReferencePoint.y) * RESOLUTION_OF_SPRITES));


		// Calculate where we need to draw the character
		destRect.x = (int)(mPosition.x - gridReferencePoint.x - interGridPositionOffset.x);
		destRect.y = (int)(mPosition.y - gridReferencePoint.y - interGridPositionOffset.y);
		destRect.w = mSingleSpriteWidth;
		destRect.h = mSingleSpriteHeight;

		mSpriteSheet->Render(portionOfSpriteSheet, destRect);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------- //

void BaseWorldMapCharacter::Update(const float deltaTime)
{
	// Just handle input for moving around the map

}

// ------------------------------------------------------------------------------------------------------------------------------- //