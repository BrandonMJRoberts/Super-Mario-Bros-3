#include "BlockObject.h"

#include "CollectableObject.h"

#include "AllCollectablesInclude.h"
#include "Constants_SMB3.h"

// ------------------------------------------------------------- //

BlockObject::BlockObject(const Vector2D      spawnPosition,
	const bool				 startSpawnedInLevel,
	SDL_Renderer*			 renderer,
	const std::string		 filePathToSpriteSheet,
	const unsigned int		 spritesOnWidth,
	const unsigned int		 spritesOnHeight,
	const double		     collisionBoxWidth,
	const double			 collisionBoxHeight,
	const float				 timePerFrame,
	const unsigned int		 hitsBlockCanTake,
	const POWER_UP_TYPE      powerUpMinimumForDamage, 
	const bool               objectReleaseScales,
	const CollectableObject* baseObjectReleased,
	const CollectableObject* maxObjectReleased)

: PhysicalObject(spawnPosition
, startSpawnedInLevel
, renderer
, filePathToSpriteSheet
, spritesOnWidth
, spritesOnHeight
, collisionBoxWidth
, collisionBoxHeight
, timePerFrame)

, mHitsBlockCanTake(hitsBlockCanTake)
, mPowerUpTypeForDamage(powerUpMinimumForDamage)
, mObjectReleasedScales(objectReleaseScales)
, mMinimumObjectReleased(baseObjectReleased)
, mMaxObjectReleased(maxObjectReleased)
{
	SetupConversionTypes();
}

// ------------------------------------------------------------- //

BlockObject::~BlockObject()
{
	mMaxObjectReleased     = nullptr;
	mMinimumObjectReleased = nullptr;

	// Clear up the memory allocated
	if (!ConvertFromStringToItemType.empty())
	{
		DeleteAllConversionObjects();
		ConvertFromStringToItemType.clear();
	}
}

// ------------------------------------------------------------- //

bool BlockObject::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	return true;
}

// ------------------------------------------------------------- //

void BlockObject::ReleaseCollectableObject(const POWER_UP_TYPE powerUpStateWhenHit)
{

}

// ------------------------------------------------------------- //

void BlockObject::SetupConversionTypes()
{
	ConvertFromStringToItemType["COIN"] = new Coin_SMB3(Vector2D(), false, mRenderer, "SDL_Mario_Project/Objects/Coin.png", 6, 1, RESOLUTION_OF_SPRITES, RESOLUTION_OF_SPRITES, false, 0.3f);
}

// ------------------------------------------------------------- //

void BlockObject::DeleteAllConversionObjects()
{
	if (ConvertFromStringToItemType["COIN"])
	{
		ConvertFromStringToItemType.erase("COIN");
	}
}

// ------------------------------------------------------------- //