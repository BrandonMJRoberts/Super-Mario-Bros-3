#include "Spiny.h"

#include "Texture2D.h"

#include "LevelMap.h"

Texture2D*   Spiny::mSpriteSheet = nullptr;
unsigned int Spiny::mSpinyCount = 0;

// --------------------------------------------------------- //

Spiny::Spiny(SDL_Renderer* renderer, const char* filePathToSpriteSheet, const float timePerFrame, Vector2D startPos, Vector2D collisionBox, bool spawningOnLeftSide, const float movementSpeed, unsigned int colourID)
	: RenderObject(0, 2, 0, timePerFrame, startPos, 5, 3, collisionBox, movementSpeed)
{
	mFacingLeft = !spawningOnLeftSide;

	// Load in the sprite sheet
	if (mSpinyCount == 0)
	{
		mSpriteSheet = new Texture2D(renderer);
		if (!mSpriteSheet->LoadFromFile(filePathToSpriteSheet))
		{
			std::cout << "Failed to load sprite sheet for coins" << std::endl;
		}
	}

	mSpinyCount++;

	switch (colourID)
	{
	default:
	case 0:
	break;

	case 1:
		mCurrentSpriteID = 5;
		mEndSpriteID     = 7;
		mStartFrameID    = 5;
		mSpriteSheetStartPoint = 5;
	break;

	case 2:
		mCurrentSpriteID = 10;
		mEndSpriteID     = 12;
		mStartFrameID    = 10;
		mSpriteSheetStartPoint = 10;
	break;
	}

	SetupRenderRects();
}

// --------------------------------------------------------- //

Spiny::~Spiny()
{
	if (mSpinyCount == 1)
	{
		delete mSpriteSheet;
		mSpriteSheet = nullptr;
	}

	mSpinyCount--;
}

// --------------------------------------------------------- //

void Spiny::SetPOWHit()
{
	// Bounce up slightly
	AddVelocity(Vector2D(0.0f, -0.8f));

	mIsFlipped   = true;
	mFreezeTimer = 6.0f;

	mCurrentSpriteID = mSpriteSheetStartPoint;
	mEndSpriteID     = mSpriteSheetStartPoint;
	mStartFrameID    = mSpriteSheetStartPoint;

	mVelocity.x = 0.0f;
}

// ------------------------------------------------------ //

bool Spiny::ClassSpecificUpdate(const float deltaTime)
{
	if (mIsFlipped && mGrounded)
	{
		mFreezeTimer -= deltaTime;

		if (mFreezeTimer < 0.0f)
		{
			mCurrentSpriteID = mSpriteSheetStartPoint;
			mEndSpriteID     = mSpriteSheetStartPoint + 2;
			mStartFrameID    = mSpriteSheetStartPoint;

			mVelocity.x      = mMovementSpeed;

			mIsFlipped       = false;
		}
	}

	return false;
}

// ------------------------------------------------------ //