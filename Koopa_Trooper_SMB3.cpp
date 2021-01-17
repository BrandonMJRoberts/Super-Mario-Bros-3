#include "Koopa_Trooper_SMB3.h"

#include <sstream>

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
	const bool          startFacingLeft,
	const char          colourIndexOfKoopa)

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
, mColourIndexOfKoopa(colourIndexOfKoopa)
{
	mEndSpriteID   = 1;
	mStartSpriteID = 0;

	switch (colourIndexOfKoopa)
	{
	default:
	break;
	}
}

// ------------------------------------------------------------------------------ //

KoopaTrooper::~KoopaTrooper()
{

}

// ------------------------------------------------------------------------------ //

BaseObject* KoopaTrooper::Clone(std::string dataLine)
{
	std::stringstream data(dataLine);

	Vector2D newPos;
	char     charPlaceHolder;

	data >> newPos.x >> newPos.y >> charPlaceHolder;

	bool startFacingLeft = true;

	if (charPlaceHolder == 'R')
		startFacingLeft = false;

	data >> charPlaceHolder;

	if (mThisSpriteSheet)
	{
		return new KoopaTrooper(newPos, false, mRenderer, mThisSpriteSheet->GetFilePath(), mSpritesOnWidth, mSpritesOnHeight, mCollisionBox.x, mCollisionBox.y, mTimePerFrame, mCanJump, mCanJump, startFacingLeft, charPlaceHolder);
	}
	else 
		return nullptr;
}

// ------------------------------------------------------------------------------ //

bool KoopaTrooper::Update(const float deltaTime, const Vector2D playerPosition)
{
	PhysicalObject::Update(deltaTime, playerPosition);

	return false;
}

// ------------------------------------------------------------------------------ //