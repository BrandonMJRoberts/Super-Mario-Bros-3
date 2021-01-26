#include "Koopa_Trooper_SMB3.h"

#include <sstream>

unsigned int KoopaTrooper::mCurrentSpriteID(0);
unsigned int KoopaTrooper::mStartSpriteID(0);
unsigned int KoopaTrooper::mEndSpriteID(1);

float        KoopaTrooper::mTimeRemainingTillNextFrame(0.0f);
bool		 KoopaTrooper::mUpdatedStaticVariables(false);


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
, mTimePerFrame(timePerFrame)
{

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

bool KoopaTrooper::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer)
{
	PhysicalObject::Update(deltaTime, playerPosition, interactionLayer);

	return false;
}

// ------------------------------------------------------------------------------ //

void KoopaTrooper::UpdateStaticVariables(const float deltaTime)
{
	mTimeRemainingTillNextFrame -= deltaTime;

	if (mTimeRemainingTillNextFrame <= 0.0f)
	{
		mTimeRemainingTillNextFrame = mTimePerFrame;

		mCurrentSpriteID++;

		if (mCurrentSpriteID > mEndSpriteID)
		{
			mCurrentSpriteID = mStartSpriteID;
		}
	}

	mUpdatedStaticVariables = true;
}

// ------------------------------------------------------------- //

void KoopaTrooper::Render(const Vector2D renderReferencePoint)
{
	RenderSprite(renderReferencePoint, mCurrentSpriteID);
}

// ------------------------------------------------------------- //