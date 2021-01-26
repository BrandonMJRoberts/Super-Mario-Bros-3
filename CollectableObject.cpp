#include "CollectableObject.h"

// --------------------------------------------------------------------------------------------------- //

CollectableObject::CollectableObject(const Vector2D       spawnPosition
	, const bool           startSpawnedInLevel
	, SDL_Renderer*        renderer
	, const std::string    filePathToSpriteSheet
	, const unsigned int   spritesOnWidth
	, const unsigned int   spritesOnHeight
	, const double         collisionBoxWidth
	, const double         collisionBoxHeight
	, const float		   timePerFrame
	, const bool           collectableCanMove)

: PhysicalObject(spawnPosition
, startSpawnedInLevel
, renderer
, filePathToSpriteSheet
, spritesOnWidth
, spritesOnHeight
, collisionBoxWidth
, collisionBoxHeight
, timePerFrame)

, mCanMove(collectableCanMove)
, mFacingLeft(false)
, mVelocity(0, 0)
{


}

// --------------------------------------------------------------------------------------------------- //

CollectableObject::~CollectableObject()
{

}

// --------------------------------------------------------------------------------------------------- //

bool CollectableObject::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer)
{
	if (mCanMove)
	{
		Move();
	}

	return true;
}

// --------------------------------------------------------------------------------------------------- //

void CollectableObject::Move()
{

}

// --------------------------------------------------------------------------------------------------- //

void CollectableObject::Jump()
{

}

// --------------------------------------------------------------------------------------------------- //