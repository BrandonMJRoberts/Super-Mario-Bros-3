#include "Invisible_Block_SMB3.h"

#include "CollectableObject.h"

#include <sstream>

unsigned int InvisibleBlock::mCurrentSpriteID(0);
unsigned int InvisibleBlock::mStartSpriteID(0);
unsigned int InvisibleBlock::mEndSpriteID(0);

float        InvisibleBlock::mTimeRemainingTillNextFrame(0.0f);
bool		 InvisibleBlock::mUpdatedStaticVariables(false);

// ------------------------------------------------------------------------------ //

InvisibleBlock::InvisibleBlock(const Vector2D           spawnPosition,
	const bool				 startSpawnedInLevel,
	SDL_Renderer*            renderer,
	const std::string		 filePathToSpriteSheet,
	const unsigned int		 spritesOnWidth,
	const unsigned int	     spritesOnHeight,
	const double		     collisionBoxWidth,
	const double			 collisionBoxHeight,
	const float				 timePerFrame,
	const unsigned int		 hitsBlockCanTake,
	const POWER_UP_TYPE      powerUpMinimumForDamage,
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
{
	
}

// ------------------------------------------------------------------------------ //

InvisibleBlock::~InvisibleBlock()
{

}

// ------------------------------------------------------------------------------ //

BaseObject* InvisibleBlock::Clone(std::string dataLine)
{
	// First extract the data
	std::stringstream data(dataLine);

	Vector2D     newPos;
	std::string  itemToDrop;
	CollectableObject* objectToDrop = nullptr;

	data >> newPos.x >> newPos.y >> itemToDrop;

	if(ConvertFromStringToItemType[itemToDrop])
		objectToDrop = ConvertFromStringToItemType[itemToDrop];

	if (mThisSpriteSheet)
	{
		return new InvisibleBlock(newPos, false, mRenderer, mThisSpriteSheet->GetFilePath(), mSpritesOnWidth, mSpritesOnHeight, mCollisionBox.x, mCollisionBox.y, mTimePerFrame, mHitsBlockCanTake, mPowerUpTypeForDamage, false, objectToDrop, nullptr);
	}
	else 
		return nullptr;
}

// ------------------------------------------------------------------------------ //

bool InvisibleBlock::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer)
{
	PhysicalObject::Update(deltaTime, playerPosition, interactionLayer);

	return false;
}

// ------------------------------------------------------------------------------ //

void InvisibleBlock::UpdateStaticVariables(const float deltaTime)
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

void InvisibleBlock::Render(const Vector2D renderReferencePoint)
{
	RenderSprite(renderReferencePoint, mCurrentSpriteID);
}

// ------------------------------------------------------------- //