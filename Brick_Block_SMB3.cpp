#include "Brick_Block_SMB3.h"

#include <sstream>

#include "Constants_SMB3.h"

Texture2D* BrickBlock::mBlockBreakingAnimation = nullptr;

unsigned int BrickBlock::mNumberOfBlocksRemaining = 0;

unsigned int BrickBlock::mCurrentSpriteID = 0;
unsigned int BrickBlock::mStartSpriteID   = 0;
unsigned int BrickBlock::mEndSpriteID     = 3;

float        BrickBlock::mTimeRemainingTillNextFrame = 0.0f;

bool         BrickBlock::mUpdatedStaticVariables = false;

// -------------------------------------------------------------------------------------------------- //

BrickBlock::BrickBlock(const Vector2D      spawnPosition,
	const bool          startSpawnedInLevel,
	SDL_Renderer*        renderer,
	const std::string   filePathToSpriteSheet,
	const unsigned int  spritesOnWidth,
	const unsigned int  spritesOnHeight,
	const double        collisionBoxWidth,
	const double        collisionBoxHeight,
	const float		    timePerFrame,
	const unsigned int  hitsBlockCanTake,
	const POWER_UP_TYPE powerUpMinimumForDamage,
	const bool          objectReleaseScales,
	const CollectableObject* baseObjectReleased,
	const CollectableObject* maxObjectReleased,
	const bool               canTurnToCoin)

: BlockObject(spawnPosition
, startSpawnedInLevel
, renderer
, filePathToSpriteSheet
, spritesOnWidth
, spritesOnHeight
, collisionBoxWidth
, collisionBoxHeight
, timePerFrame
, hitsBlockCanTake
, powerUpMinimumForDamage
, objectReleaseScales
, baseObjectReleased
, maxObjectReleased)
, mTimePerFrame(timePerFrame)
, mCanTurnToCoin(canTurnToCoin)
, mBlockColourID(0)
, mBlockBreakingAnimationPlaying(false)
{
	mHitsBlockCanTake = 1;
	mNumberOfBlocksRemaining++;

	if (!mBlockBreakingAnimation)
	{
		mBlockBreakingAnimation = new Texture2D(renderer);
		if(!mBlockBreakingAnimation->LoadFromFile("SDL_Mario_Project/Objects/BrickBlockBreakingAnimation.png"))
		{
			std::cout << "Failed to load in the sprite sheet for the brick block breaking" << std::endl;
			return;
		}
	}
}

// -------------------------------------------------------------------------------------------------- //

BrickBlock::~BrickBlock() 
{
	mNumberOfBlocksRemaining--;

	if (mNumberOfBlocksRemaining == 0)
	{
		delete mBlockBreakingAnimation;
		mBlockBreakingAnimation = nullptr;
	}
}

// -------------------------------------------------------------------------------------------------- //

BaseObject* BrickBlock::Clone(std::string dataForNewObject)
{
	// Variables needed for this cloning
	Vector2D            newPos;
	bool                canTurnToCoin;
	char                canTurnToCoinChar;
	std::string         newItemTypeString;
	CollectableObject*  newItemType;

	// Convert the line to something useful
	std::stringstream dataLine(dataForNewObject);

	// Extract the data
	dataLine >> newPos.x >> newPos.y;
	dataLine >> canTurnToCoinChar;
	dataLine >> newItemTypeString;

	if (canTurnToCoinChar == 'T')
		canTurnToCoin = true;
	else 
		canTurnToCoin = false;
	
	newItemType = ConvertFromStringToItemType[newItemTypeString];

	if(mThisSpriteSheet)
	{
	// Return the new instance
	return new BrickBlock(newPos, 
		                  false, 
		                  mRenderer, 
		                  mThisSpriteSheet->GetFilePath(), 
		                  mSpritesOnWidth, 
						  mSpritesOnHeight, 
		                  mCollisionBox.x, 
		                  mCollisionBox.y, 
		                  mTimePerFrame, 
		                  1, 
		                  mPowerUpTypeForDamage, 
		                  false, 
		                  newItemType,
						  nullptr,
		                  mCanTurnToCoin);
	}
	else 
		return nullptr;

}

// -------------------------------------------------------------------------------------------------- //

bool BrickBlock::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer)
{
	if(!mUpdatedStaticVariables)
		UpdateStaticVariables(deltaTime);

	if (mBlockBreakingAnimationPlaying)
	{
		mCurrentPosition.x -= 0.5f * deltaTime;
		mCurrentPosition.y -= 0.5f * deltaTime;
	}

	if (mHitsBlockCanTake == 0 && !mBlockBreakingAnimationPlaying)
		return false;

	return false;
}

// -------------------------------------------------------------------------------------------------- //

void BrickBlock::UpdateStaticVariables(const float deltaTime)
{
	// Update the frame time and adjust the current frame if needed
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

// -------------------------------------------------------------------------------------------------- //

void BrickBlock::Render(const Vector2D renderReferencePoint)
{
	if (mBlockBreakingAnimationPlaying)
	{
		int width = (int)mBlockBreakingAnimation->GetWidth() / 4, height = (int)mBlockBreakingAnimation->GetHeight();

		Vector2D renderPos = Vector2D(mCurrentPosition.x - renderReferencePoint.x, mCurrentPosition.y - renderReferencePoint.y);

		SDL_Rect portionOfSpriteSheet{ (int)(mBlockColourID % mSpritesOnWidth) * width,
									   (int)(mBlockColourID / mSpritesOnWidth) * height,
									   width,
									   height };

		SDL_Rect destRect{            int(renderPos.x * RESOLUTION_OF_SPRITES),
									  int(renderPos.y * RESOLUTION_OF_SPRITES) - height + 1,
									  width,
									  height };

		// First render the left side
		mThisSpriteSheet->Render(portionOfSpriteSheet, destRect);

		// Now render the right side
		destRect.x -= 1.0f;
		destRect.y -= 1.0f;

		mThisSpriteSheet->Render(portionOfSpriteSheet, destRect);
	}
	else
		RenderSprite(renderReferencePoint, mCurrentSpriteID);
}

// -------------------------------------------------------------------------------------------------- //

ObjectCollisionHandleData BrickBlock::SetIsCollidedWith(TwoDimensionalCollision collisionData)
{
	if (collisionData.collisionDataPrimary == MOVEMENT_DIRECTION::UP)
	{
		if(mHitsBlockCanTake > 0)
			mHitsBlockCanTake--;

		// Now trigger the block breaking animation
		mBlockBreakingAnimationPlaying = true;
	}

	return ObjectCollisionHandleData();
}

// -------------------------------------------------------------------------------------------------- //