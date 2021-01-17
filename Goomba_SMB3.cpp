#include "Goomba_SMB3.h"

#include <sstream>

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
	mEndSpriteID   = 1;
	mStartSpriteID = 0;
}

// ------------------------------------------------------------- //

Goomba::~Goomba()
{

}

// ------------------------------------------------------------- //

BaseObject* Goomba::Clone(std::string data)
{
	// First extra the data from the line passed in
	std::stringstream dataLine(data);

	Vector2D newPos;
	char     facingDirection;

	dataLine >> newPos.x >> newPos.y >> facingDirection;

	bool startFacingLeft = facingDirection == 'L' ? true : false;

	if (mThisSpriteSheet)
		return new Goomba(newPos, false, mRenderer, mThisSpriteSheet->GetFilePath(), mSpritesOnWidth, mSpritesOnHeight, mCollisionBox.x, mCollisionBox.y, mTimePerFrame, mCanMove, mCanJump, startFacingLeft);
	else
		return nullptr;
}

// ------------------------------------------------------------- //

bool Goomba::Update(const float deltaTime, const Vector2D playerPosition)
{
	PhysicalObject::Update(deltaTime, playerPosition);

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