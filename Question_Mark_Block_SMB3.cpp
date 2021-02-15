#include "Question_Mark_Block_SMB3.h"

#include <sstream>

unsigned int QuestionMarkBlock::mCurrentSpriteID(0);
unsigned int QuestionMarkBlock::mStartSpriteID(0);
unsigned int QuestionMarkBlock::mEndSpriteID(3);

float        QuestionMarkBlock::mTimeRemainingTillNextFrame(0.0f);
bool		 QuestionMarkBlock::mUpdatedStaticVariables(false);

// ---------------------------------------------------------------------------------------------- //

QuestionMarkBlock::QuestionMarkBlock(const Vector2D           spawnPosition,
	const bool			     startSpawnedInLevel,
	SDL_Renderer*			 renderer,
	const std::string		 filePathToSpriteSheet,
	const unsigned int	     spritesOnWidth,
	const unsigned int	     spritesOnHeight,
	const double			 collisionBoxWidth,
	const double			 collisionBoxHeight,
	const float			     timePerFrame,
	const unsigned int	     hitsBlockCanTake,
	const POWER_UP_TYPE	     powerUpMinimumForDamage,
	const bool               objectReleaseScales,
	const CollectableObject* baseObjectReleased,
	const CollectableObject* maxObjectReleased)

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
, mDoingBounceAnimation(false)
, mStartBounceYPos(0.0f)
{
	
}

// ---------------------------------------------------------------------------------------------- //

QuestionMarkBlock::~QuestionMarkBlock()
{

}

// ---------------------------------------------------------------------------------------------- //

bool QuestionMarkBlock::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer)
{
	if (!mUpdatedStaticVariables)
		UpdateStaticVariables(deltaTime);

	// Now handle the bounce animation
	if (mDoingBounceAnimation)
	{
		// See if the block should bounce upwards
		if (mHitPeakOfBounce)
		{
			// Move the block down
			mCurrentPosition.y += 6.0f * deltaTime;

			// Check to see if the animation should end
			if (mCurrentPosition.y > mStartBounceYPos)
			{
				mCurrentPosition.y    = mStartBounceYPos;
				mDoingBounceAnimation = false;
			}
		}
		else
		{
			mCurrentPosition.y -= 6.0f * deltaTime;

			if (mCurrentPosition.y < mStartBounceYPos - 0.4f)
			{
				mHitPeakOfBounce = true;
			}
		}
	}

	return false;
}

// ---------------------------------------------------------------------------------------------- //

BaseObject* QuestionMarkBlock::Clone(std::string dataForNewObject)
{
	std::stringstream streamLine(dataForNewObject);

	Vector2D    newPos;
	std::string nameOfItemToDrop, nameOfMaxItemToDrop;
	unsigned int quantity;
	char scalable;
	bool canScale = true;

	streamLine >> newPos.x >> newPos.y >> nameOfItemToDrop >> quantity >> scalable >> nameOfItemToDrop;

	if (scalable == 'F')
		canScale = false;

	if (mThisSpriteSheet)
	{
		return new QuestionMarkBlock(newPos, false, mRenderer, mThisSpriteSheet->GetFilePath(), mSpritesOnWidth, mSpritesOnHeight, mCollisionBox.x, mCollisionBox.y, mTimePerFrame, mHitsBlockCanTake, mPowerUpTypeForDamage, canScale, ConvertFromStringToItemType[nameOfItemToDrop], ConvertFromStringToItemType[nameOfMaxItemToDrop]);
	}
	else
		return nullptr;

}

// ---------------------------------------------------------------------------------------------- //

void QuestionMarkBlock::UpdateStaticVariables(const float deltaTime)
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

void QuestionMarkBlock::Render(const Vector2D renderReferencePoint)
{
	if(mHitsBlockCanTake == 0)
		RenderSprite(renderReferencePoint, 4);
	else
		RenderSprite(renderReferencePoint, mCurrentSpriteID);
}

// ------------------------------------------------------------- //

ObjectCollisionHandleData QuestionMarkBlock::SetIsCollidedWith(TwoDimensionalCollision collisionData)
{
	if (collisionData.collisionDataPrimary == MOVEMENT_DIRECTION::UP && collisionData.playerPriorPosition.y > mCurrentPosition.y)
	{
		if (mHitsBlockCanTake > 0)
		{
			mHitsBlockCanTake--;

			// Now trigger the bounce animation
			mDoingBounceAnimation = true;

			mStartBounceYPos = mCurrentPosition.y;
		}

		// If was the last hit then release the object stored in it
		if (mHitsBlockCanTake == 0)
		{
			if (mMinimumObjectReleased)
			{

			}
		}
	}

	return ObjectCollisionHandleData();
}

// ------------------------------------------------------------- //