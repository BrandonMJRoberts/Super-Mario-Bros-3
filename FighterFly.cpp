#include "FighterFly.h"

#include "Texture2D.h"

#include "LevelMap.h"

Texture2D*   FighterFly::mSpriteSheet      = nullptr;
unsigned int FighterFly::mFighterFlyCount  = 0;

// --------------------------------------------------------- //

FighterFly::FighterFly(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox, bool spawningOnLeftSide, const float movementSpeed, unsigned int colourID)
	: RenderObject(0, 1, 0, timePerFrame, startPos, 7, 3, collisionBox, movementSpeed)

	, mFlipCountdown(0.0f)
{
	mFacingLeft = !spawningOnLeftSide;

	if (mFacingLeft)
		mVelocity.x = -mMovementSpeed;
	else
		mVelocity.x = mMovementSpeed;

	// Load in the sprite sheet
	if (mFighterFlyCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mFighterFlyCount++;

	switch (colourID)
	{
	default:
	case 0:
	break;

	case 1:
		mCurrentSpriteID = 7;
		mEndSpriteID     = 8;
		mStartFrameID    = 7;
		mSpriteSheetStartPoint = 7;
	break;

	case 2:
		mCurrentSpriteID = 14;
		mEndSpriteID     = 15;
		mStartFrameID    = 14;
		mSpriteSheetStartPoint = 14;
	break;
	}

	SetupRenderRects();
}

// --------------------------------------------------------- //

FighterFly::~FighterFly()
{
	if (mFighterFlyCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mFighterFlyCount--;
}

// --------------------------------------------------------- //

void FighterFly::SetPOWHit()
{
	// Bounce up slightly
	AddVelocity(Vector2D(0.0f, -1.5f));

	mIsFlipped = true;
	mFlipCountdown = 6.0f;

	mCurrentSpriteID = mSpriteSheetStartPoint + 3;
	mEndSpriteID     = mSpriteSheetStartPoint + 4;
	mStartFrameID    = mSpriteSheetStartPoint + 3;

	mVelocity.x = 0.0f;
}

// ------------------------------------------------------ //

bool FighterFly::ClassSpecificUpdate(const float deltaTime)
{
	if (mIsFlipped && mGrounded)
	{
		mFlipCountdown -= deltaTime;

		if (mFlipCountdown < 0.0f)
		{
			mCurrentSpriteID = mSpriteSheetStartPoint;
			mEndSpriteID     = mSpriteSheetStartPoint + 1;
			mStartFrameID    = mSpriteSheetStartPoint;

			mVelocity.x      = mMovementSpeed * 2.0;

			mIsFlipped       = false;
		}
	}

	return false;
}

// ------------------------------------------------------ //