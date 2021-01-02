#include "Goomba_SMB3.h"

// ------------------------------------------------------------- //

Goomba::Goomba(const Vector2D      spawnPosition,
	           const bool          startSpawnedInLevel,
	           SDL_Renderer* renderer,
	           const std::string   filePathToSpriteSheet,
			   const unsigned int  spritesOnWidth,
			   const unsigned int  spritesOnHeight,
			   const double        collisionBoxWidth,
			   const double        collisionBoxHeight,
		       const float		    timePerFrame,
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

// ------------------------------------------------------------- //

Goomba::~Goomba()
{

}

// ------------------------------------------------------------- //

BaseObject* Goomba::Clone(std::string data)
{
	return nullptr;
}

// ------------------------------------------------------------- //

bool Goomba::Update(const float deltaTime, const Vector2D playerPosition)
{
	return false;
}

// ------------------------------------------------------------- //

void Goomba::Move()
{

}

// ------------------------------------------------------------- //

void Goomba::Jump()
{

}

// ------------------------------------------------------------- //

void Goomba::Attack()
{

}

// ------------------------------------------------------------- //