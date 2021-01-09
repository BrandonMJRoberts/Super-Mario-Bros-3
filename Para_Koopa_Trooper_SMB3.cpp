#include "Para_Koopa_Trooper_SMB3.h"

#include <sstream>

// --------------------------------------------------------------------------------------------- //

ParaKoopaTrooper::ParaKoopaTrooper(const Vector2D      spawnPosition,
	const bool          startSpawnedInLevel,
	SDL_Renderer*       renderer,
	const std::string   filePathToSpriteSheet,
	const unsigned int  spritesOnWidth,
	const unsigned int  spritesOnHeight,
	const double        collisionBoxWidth,
	const double        collisionBoxHeight,
	const float		    timePerFrame,
	const bool          canMove,
	const bool          canJump,
	const bool          startFacingLeft,
	const char          colourIndexOfKoopa)

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
, startFacingLeft
, colourIndexOfKoopa)
{

}

// --------------------------------------------------------------------------------------------- //

ParaKoopaTrooper::~ParaKoopaTrooper()
{

}

// --------------------------------------------------------------------------------------------- //

BaseObject* ParaKoopaTrooper::Clone(std::string dataLine)
{
	std::stringstream streamLine(dataLine);

	Vector2D newPos;
	char facingDir, colourChar;

	streamLine >> newPos.x >> newPos.y >> facingDir >> colourChar;

	bool startFacingLeft = true;
	if (facingDir == 'R')
		startFacingLeft = false;

	return new ParaKoopaTrooper(newPos, false, mRenderer, mThisSpriteSheet->GetFilePath(), mSpritesOnWidth, mSpritesOnHeight, mCollisionBox.x, mCollisionBox.y, mTimePerFrame, mCanMove, mCanJump, startFacingLeft, colourChar);
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