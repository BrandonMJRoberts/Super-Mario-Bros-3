#include "Crab.h"

#include "Texture2D.h"
#include "LevelMap.h"

Texture2D*   Crab::mSpriteSheet = nullptr;
unsigned int Crab::mCrabCount = 0;

// ------------------------------------------------------ //

Crab::Crab(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox, bool spawningOnLeftSide, const float movementSpeed, unsigned int colourID)
	: RenderObject(0, 2, 0, timePerFrame, startPos, 7, 6, collisionBox, movementSpeed)

	, mAngry(false)
	, mFlipCountdown(0.0f)

	, mSpriteSheetStartPoint(0)
{
	mFacingLeft = !spawningOnLeftSide;

	// Load in the sprite sheet
	if (mCrabCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mCrabCount++;

	switch (colourID)
	{
	default:
	case 0:
	break;

	case 1:
		mCurrentSpriteID = 14;
		mEndSpriteID     = 16;
		mStartFrameID    = 14;
		mSpriteSheetStartPoint = 14;
	break;

	case 2:
		mCurrentSpriteID = 28;
		mEndSpriteID     = 30;
		mStartFrameID    = 28;

		mSpriteSheetStartPoint = 28;
	break;
	}

	SetupRenderRects();
}

// ------------------------------------------------------ //

Crab::~Crab() 
{
	if (mCrabCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mCrabCount--;
}

// ------------------------------------------------------ //

void Crab::SetPOWHit()
{
	// Bounce up slightly
	AddVelocity(Vector2D(0.0f, -1.0f));

	mIsFlipped         = true;
	mFlipCountdown     = 6.0f;

	mCurrentSpriteID   = mSpriteSheetStartPoint + 3;
	mEndSpriteID       = mSpriteSheetStartPoint + 4;
	mStartFrameID      = mSpriteSheetStartPoint + 3;

	mVelocity.x        = 0.0f;
}

// ------------------------------------------------------ //

bool Crab::ClassSpecificUpdate(const float deltaTime)
{
	if (mIsFlipped && mGrounded)
	{
		mFlipCountdown -= deltaTime;

		if (mFlipCountdown < 0.0f)
		{
			mCurrentSpriteID = mSpriteSheetStartPoint + 7;
			mEndSpriteID     = mSpriteSheetStartPoint + 9;
			mStartFrameID    = mSpriteSheetStartPoint + 7;

			mVelocity.x      = mMovementSpeed * 2.0;

			mAngry           = true;
			mIsFlipped       = false;
		}
	}

	return false;
}

// ------------------------------------------------------ //