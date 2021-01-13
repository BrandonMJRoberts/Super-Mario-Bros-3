#include "Question_Mark_Block_SMB3.h"

#include <sstream>

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
{

}

// ---------------------------------------------------------------------------------------------- //

QuestionMarkBlock::~QuestionMarkBlock()
{

}

// ---------------------------------------------------------------------------------------------- //

bool QuestionMarkBlock::Update(const float deltaTime, const Vector2D playerPosition)
{
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