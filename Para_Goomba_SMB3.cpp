#include "Para_Goomba_SMB3.h"

// ------------------------------------------------------------------------------------------------ //

ParaGoomba::ParaGoomba(const Vector2D      spawnPosition,
	const bool          startSpawnedInLevel,
	SDL_Renderer* renderer,
	const std::string   filePathToSpriteSheet,
	const unsigned int  spritesOnWidth,
	const unsigned int  spritesOnHeight,
	const double        collisionBoxWidth,
	const double        collisionBoxHeight,
	const float		   timePerFrame,
	const bool          canMove,
	const bool          canJump,
	const bool          startFacingLeft)

: Goomba(spawnPosition
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

// ------------------------------------------------------------------------------------------------ //

ParaGoomba::~ParaGoomba()
{

}

// ------------------------------------------------------------------------------------------------ //

BaseObject* ParaGoomba::Clone(std::string data)
{
	return nullptr;
}

// ------------------------------------------------------------------------------------------------ //

bool ParaGoomba::Update(const float deltaTime, const Vector2D playerPosition)
{
	return false;
}

// ------------------------------------------------------------------------------------------------ //

void ParaGoomba::Move()
{

}

// ------------------------------------------------------------------------------------------------ //

void ParaGoomba::Jump()
{

}

// ------------------------------------------------------------------------------------------------ //

void ParaGoomba::Attack()
{

}

// ------------------------------------------------------------------------------------------------ //