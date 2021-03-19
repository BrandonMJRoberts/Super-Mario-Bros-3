#include "Coin_SMB3.h"

#include <SDL.h>
#include "Texture2D.h"
#include "Constants_SMB3.h"

#include <sstream>

unsigned int Coin_SMB3::mCurrentSpriteID(0);
unsigned int Coin_SMB3::mStartSpriteID(0);
unsigned int Coin_SMB3::mEndSpriteID(5);

float        Coin_SMB3::mTimeRemainingTillNextFrame(0.0f);
bool		 Coin_SMB3::mUpdatedStaticVariables(false);

// ---------------------------------------------------------------------------------- //

Coin_SMB3::Coin_SMB3(const Vector2D spawnPosition
, const bool         startSpawnedInLevel
, SDL_Renderer*      renderer
, const char* const  filePathToSpriteSheet
, const unsigned int amountOfSpritesOnWidth
, const unsigned int amountOfSpritesOnHeight
, const double       collisionBoxWidth
, const double       collisionBoxHeight
, const bool         collectableCanMove
, const float        timePerAnimationFrame)

: CollectableObject( spawnPosition
, startSpawnedInLevel
, renderer
, filePathToSpriteSheet
, amountOfSpritesOnWidth
, amountOfSpritesOnHeight
, collisionBoxWidth
, collisionBoxHeight
, timePerAnimationFrame
, collectableCanMove)
, mTimePerFrame(timePerAnimationFrame)
{
	
}

// ---------------------------------------------------------------------------------- //

Coin_SMB3::~Coin_SMB3()
{
}

// ---------------------------------------------------------------------------------- //

BaseObject* Coin_SMB3::Clone(std::string dataLine)
{
	// First convert the line to being a stringstream 
	std::stringstream extractLine(dataLine);

	// Extract the data 
	unsigned int xPos, yPos;
	extractLine >> xPos >> yPos;

	if (mThisSpriteSheet)
		return new Coin_SMB3(Vector2D((double)xPos, (double)yPos), false, mRenderer, mThisSpriteSheet->GetFilePath().c_str(), mSpritesOnWidth, mSpritesOnHeight, (unsigned int)mCollisionBox.x, (unsigned int)mCollisionBox.y, false, mTimePerFrame);
	else
		return nullptr;
}

// ---------------------------------------------------------------------------------- //

void Coin_SMB3::Render(const Vector2D renderReferencePoint)
{
	RenderSprite(renderReferencePoint, mCurrentSpriteID);
}

// ---------------------------------------------------------------------------------- //

bool Coin_SMB3::Update(const float deltaTime, const Vector2D playerPosition, InteractableLayer* interactionLayer, ObjectLayer* objectLayer)
{
	if(!mUpdatedStaticVariables)
		UpdateStaticVariables(deltaTime);

	return false;
}

// ---------------------------------------------------------------------------------- //

void Coin_SMB3::UpdateStaticVariables(const float deltaTime)
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

// ---------------------------------------------------------------------------------- //

bool Coin_SMB3::UpdateReleaseAnimation(const float deltaTime, const Vector2D& startPos)
{
	if (mReleaseAnimationTimer == 0.0f)
	{
		// Play the coin SFX
		Notify(SUBJECT_NOTIFICATION_TYPES::COIN_COLLECTED, "");

		mAnimationStartPosition = startPos;
	}

	// Need to make the fall back down before destroying the coin
	if (mTraveledUpOnAnimation)
	{
		// Travel down
		mCurrentPosition.x -= 0.1 * deltaTime;

		// Check to see if it has completed the movement
		if (mCurrentPosition.x < startPos.y)
			return true;
	}
	else
	{
		// Need to travel up in a bounce kind of way
		mCurrentPosition.x += 0.1 * deltaTime;

		// Check to see if it has completed the movement up
		if (mCurrentPosition.x > startPos.y + 3)
			mTraveledUpOnAnimation = true;
	}

	// Add time to the timer
	mReleaseAnimationTimer += deltaTime;

	// Now determine if the sprite should change on the animation
	if (mReleaseAnimationTimer > mTimePerFrame)
	{
		mReleaseAnimationCurrentFrame++;

		if (mReleaseAnimationCurrentFrame > mReleaseAnimationEndFrame)
			mReleaseAnimationCurrentFrame = mReleaseAnimationStartFrame;
	}

	return false;
}

// ---------------------------------------------------------------------------------- //