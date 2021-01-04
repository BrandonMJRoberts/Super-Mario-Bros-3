#include "Koopa_Trooper_SMB3.h"

// ------------------------------------------------------------------------------ //

KoopaTrooper::KoopaTrooper(const Vector2D      spawnPosition,
	const bool          startSpawnedInLevel,
	SDL_Renderer*       renderer,
	const std::string   filePathToSpriteSheet,
	const unsigned int  spritesOnWidth,
	const unsigned int  spritesOnHeight,
	const double        collisionBoxWidth,
	const double        collisionBoxHeight,
	const float			timePerFrame,
	const bool          canMove,
	const bool          canJump,
	const bool          startFacingLeft)

: EnemyObject(spawnPosition
, startSpawnedInLevel
, renderer
, filePathToSpriteSheet
, spritesOnWidth
, spritesOnHeight
, collisionBoxWidth
, collisionBoxHeight
, timePerFrame
, canMove
, canJump
, startFacingLeft)
{

}

// ------------------------------------------------------------------------------ //

KoopaTrooper::~KoopaTrooper()
{

}

// ------------------------------------------------------------------------------ //

BaseObject* KoopaTrooper::Clone(std::string dataLine)
{
	return nullptr;
}

// ------------------------------------------------------------------------------ //

bool KoopaTrooper::Update(const float deltaTime, const Vector2D playerPosition)
{
	return false;
}

// ------------------------------------------------------------------------------ //