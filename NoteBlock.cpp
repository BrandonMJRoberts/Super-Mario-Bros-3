#include "Note_Block.h"

#include <sstream>

unsigned int NoteBlock::mCurrentSpriteID = 0;
unsigned int NoteBlock::mStartSpriteID   = 0;
unsigned int NoteBlock::mEndSpriteID     = 3;

float        NoteBlock::mTimeRemainingTillNextFrame = 0.0f;

bool         NoteBlock::mUpdatedStaticVariables     = false;

// -------------------------------------------------------------------------------------- //

NoteBlock::NoteBlock(const Vector2D           spawnPosition,
					 const bool               startSpawnedInLevel,
					 SDL_Renderer*            renderer,
					 const std::string        filePathToSpriteSheet,
					 const unsigned int       spritesOnWidth,
					 const unsigned int       spritesOnHeight,
					 const double             collisionBoxWidth,
					 const double             collisionBoxHeight,
					 const float		      timePerFrame,
					 const unsigned int	      hitsBlockCanTake,
					 const POWER_UP_TYPE	  powerUpMinimumForDamage,
					 const bool               objectReleaseScales,
					 const CollectableObject* baseObjectReleased,
					 const CollectableObject* maxObjectReleased) 
: BlockObject(spawnPosition, startSpawnedInLevel, renderer, filePathToSpriteSheet, spritesOnWidth, spritesOnHeight, collisionBoxWidth, collisionBoxHeight, timePerFrame, hitsBlockCanTake, powerUpMinimumForDamage, objectReleaseScales, baseObjectReleased, maxObjectReleased)
, mTimePerFrame(timePerFrame)
, mCanTurnToCoin(false)
{

}


// -------------------------------------------------------------------------------------- //

NoteBlock::~NoteBlock()
{

}

// -------------------------------------------------------------------------------------- //

BaseObject* NoteBlock::Clone(std::string dataLine)
{
	std::stringstream extractionLine = std::stringstream(dataLine);

	Vector2D    position;
	std::string itemToDrop;

	extractionLine >> position.x >> position.y >> itemToDrop;

	if (mThisSpriteSheet)
		return new NoteBlock(position, false, mRenderer, mThisSpriteSheet->GetFilePath(), mSpritesOnWidth, mSpritesOnHeight, mCollisionBox.x, mCollisionBox.y, mTimePerFrame, mHitsBlockCanTake, mPowerUpTypeForDamage, mObjectReleasedScales, nullptr, nullptr);
	else
		return nullptr;
}

// -------------------------------------------------------------------------------------- //

void NoteBlock::UpdateStaticVariables(const float deltaTime)
{
	// Update the frame time and adjust the current frame if needed
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

// -------------------------------------------------------------------------------------- //

void NoteBlock::Render(const Vector2D renderReferencePoint)
{
	RenderSprite(renderReferencePoint, mCurrentSpriteID);
}

// -------------------------------------------------------------------------------------- //