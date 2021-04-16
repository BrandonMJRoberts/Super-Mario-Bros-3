#include "Question_Mark_Block_SMB3.h"
#include "CollectableObject.h"

#include "CoinRelaseAnimation.h"

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
	CollectableObject*       baseObjectReleased,
	CollectableObject*       maxObjectReleased)

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
, mPlayReleaseAnimation(false)
, mHitPeakOfBounce(false)
, mObjectReleasedAnimation(nullptr)
{
	mObjectReleasedAnimation = new CoinReleaseAnimation(Vector2D(spawnPosition.x, spawnPosition.y - 1), mRenderer);
}

// ---------------------------------------------------------------------------------------------- //

QuestionMarkBlock::~QuestionMarkBlock()
{
	delete mObjectReleasedAnimation;
	mObjectReleasedAnimation = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

bool QuestionMarkBlock::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
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
			mCurrentPosition.y += 6.0 * deltaTime;

			// Check to see if the animation should end
			if (mCurrentPosition.y > mStartBounceYPos)
			{
				mCurrentPosition.y    = mStartBounceYPos;
				mDoingBounceAnimation = false;
			}
		}
		else
		{
			mCurrentPosition.y -= 6.0 * deltaTime;

			if (mCurrentPosition.y < mStartBounceYPos - 0.4)
			{
				mHitPeakOfBounce = true;
			}
		}
	}

	if (mPlayReleaseAnimation)
	{
		if (mObjectReleasedAnimation)
		{
			if (mObjectReleasedAnimation->Update(deltaTime))
			{
				delete mObjectReleasedAnimation;
				mObjectReleasedAnimation = nullptr;
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

	streamLine >> newPos.x >> newPos.y >> nameOfItemToDrop >> quantity >> scalable >> nameOfMaxItemToDrop;

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

	if (mPlayReleaseAnimation)
	{
		if (mObjectReleasedAnimation)
		{
			mObjectReleasedAnimation->Render(renderReferencePoint);
		}
	}
}

// ------------------------------------------------------------- //

ObjectCollisionHandleData QuestionMarkBlock::SetIsCollidedWith(TwoDimensionalCollision collisionData, const unsigned int playerMovements, const bool isPlayer)
{
	if (!isPlayer)
		return ObjectCollisionHandleData();

	if (   collisionData.collisionDataPrimary == MOVEMENT_DIRECTION::UP 
		&& collisionData.playerPriorPosition.y > mCurrentPosition.y)
	{
		if (mHitsBlockCanTake > 0)
		{
			mHitsBlockCanTake--;

			// Now trigger the bounce animation
			mDoingBounceAnimation = true;

			mStartBounceYPos = (float)mCurrentPosition.y;

			// Release the object stored in it
			mPlayReleaseAnimation = true;

			Notify(SUBJECT_NOTIFICATION_TYPES::COIN_COLLECTED, "");
		}		
	}

	return ObjectCollisionHandleData();
}

// ------------------------------------------------------------- //