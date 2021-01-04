#include "Para_Koopa_Trooper_SMB3.h"

// --------------------------------------------------------------------------------------------- //

ParaKoopaTrooper::ParaKoopaTrooper(const Vector2D      spawnPosition,
	const bool          startSpawnedInLevel,
	SDL_Renderer* renderer,
	const std::string   filePathToSpriteSheet,
	const unsigned int  spritesOnWidth,
	const unsigned int  spritesOnHeight,
	const double        collisionBoxWidth,
	const double        collisionBoxHeight,
	const float		 timePerFrame,
	const bool          canMove,
	const bool          canJump,
	const bool          startFacingLeft)

: KoopaTrooper(spawnPosition
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

// --------------------------------------------------------------------------------------------- //

ParaKoopaTrooper::~ParaKoopaTrooper()
{

}

// --------------------------------------------------------------------------------------------- //

BaseObject* ParaKoopaTrooper::Clone(std::string dataLine)
{
	return nullptr;
}

// --------------------------------------------------------------------------------------------- //

bool ParaKoopaTrooper::Update(const float deltaTime, const Vector2D playerPosition)
{
	return false;
}

// --------------------------------------------------------------------------------------------- //

void ParaKoopaTrooper::Move()
{

}

// --------------------------------------------------------------------------------------------- //

void ParaKoopaTrooper::Jump()
{

}

// --------------------------------------------------------------------------------------------- //

void ParaKoopaTrooper::Attack()
{

}

// --------------------------------------------------------------------------------------------- //